import FieldOfChaosEngine
import Foundation

struct BoardPoint: Hashable, Identifiable {
    var x: Int
    var y: Int

    var id: String { "\(x),\(y)" }

    func distance(to other: BoardPoint) -> Int {
        abs(x - other.x) + abs(y - other.y)
    }
}

enum ActorSide: String {
    case player
    case opponent
}

enum SkirmishOutcome: String {
    case active
    case playerWon
    case opponentWon

    var isFinished: Bool {
        self != .active
    }
}

enum AIDifficulty: String, CaseIterable, Identifiable {
    case easy
    case standard
    case hard

    var id: String { rawValue }

    var label: String {
        rawValue.capitalized
    }
}

struct BoardActor: Identifiable {
    let id: UUID
    var character: FocCharacter
    var side: ActorSide
    var position: BoardPoint
    var acted = false
    var initiative = 0

    var snapshot: CharacterSnapshot {
        CharacterSnapshot(character: character)
    }
}

struct BoardRenderActor: Equatable {
    var id: UUID
    var x: Int
    var y: Int
    var isPlayer: Bool
    var isSelected: Bool
    var isTargeted: Bool
}

struct BoardRenderState: Equatable {
    var columns: Int
    var rows: Int
    var moveCells: Set<BoardPoint>
    var targetCells: Set<BoardPoint>
    var selectedCell: BoardPoint?
    var objectiveCell: BoardPoint?
    var coverCells: Set<BoardPoint>
    var actors: [BoardRenderActor]
}

struct BoardActionResult {
    var ok: Bool
    var message: String
    var events: [CombatLogEvent]
}

final class SkirmishState: ObservableObject {
    @Published var columns = 16
    @Published var rows = 10
    @Published var cellYards = 2
    @Published var actors: [BoardActor] = []
    @Published var selectedActorID: UUID?
    @Published var selectedCell: BoardPoint?
    @Published var movementPace: MovementPace = .standard
    @Published var turnIndex = 0
    @Published var turnNumber = 1
    @Published var message = "Select an actor."
    @Published var actionEvents: [CombatLogEvent] = []
    @Published var scenario: ScenarioSummary?
    @Published var outcome: SkirmishOutcome = .active
    @Published var aiEnabled = true
    @Published var maxTurns = 18

    var currentActorID: UUID? {
        guard actors.indices.contains(turnIndex) else {
            return nil
        }
        return actors[turnIndex].id
    }

    var currentActor: BoardActor? {
        guard let currentActorID else {
            return nil
        }
        return actors.first { $0.id == currentActorID }
    }

    var selectedActor: BoardActor? {
        guard let selectedActorID else {
            return nil
        }
        return actors.first { $0.id == selectedActorID }
    }

    var renderState: BoardRenderState {
        BoardRenderState(
            columns: columns,
            rows: rows,
            moveCells: legalMoveCells,
            targetCells: Set(actors.filter { actor in
                guard let selectedActor, actor.id != selectedActor.id else {
                    return false
                }
                return targetingPreview(for: actor.id)?.inRange == true
            }.map(\.position)),
            selectedCell: selectedActor?.position ?? selectedCell,
            objectiveCell: scenario?.objectivePoint,
            coverCells: scenario?.coverCells ?? [],
            actors: actors.map { actor in
                BoardRenderActor(
                    id: actor.id,
                    x: actor.position.x,
                    y: actor.position.y,
                    isPlayer: actor.side == .player,
                    isSelected: actor.id == selectedActorID,
                    isTargeted: selectedActorID != nil && actor.id != selectedActorID && targetingPreview(for: actor.id)?.inRange == true
                )
            }
        )
    }

    var legalMoveCells: Set<BoardPoint> {
        guard let selectedActor else {
            return []
        }

        var character = selectedActor.character
        let yards = Int(foc_character_movement_yards(&character, movementPace.cValue))
        let cells = max(0, yards / cellYards)
        guard cells > 0 else {
            return []
        }

        var result: Set<BoardPoint> = []
        for x in 0..<columns {
            for y in 0..<rows {
                let point = BoardPoint(x: x, y: y)
                guard selectedActor.position.distance(to: point) <= cells else {
                    continue
                }
                guard !actors.contains(where: { $0.position == point && $0.id != selectedActor.id }) else {
                    continue
                }
                result.insert(point)
            }
        }
        return result
    }

    func start(from records: [CharacterRecord]) {
        var config = FocScenarioConfig()
        config.seed = 40
        config.difficulty = 1
        config.width = CInt(columns)
        config.height = CInt(rows)
        var generated = FocScenario()
        foc_generate_scenario(&config, &generated)
        start(from: records, scenario: ScenarioSummary(scenario: generated))
    }

    func generateScenario(seed: Int, difficulty: Int, records: [CharacterRecord]) {
        var config = FocScenarioConfig()
        config.seed = UInt32(max(0, seed))
        config.difficulty = CInt(max(1, difficulty))
        config.width = CInt(columns)
        config.height = CInt(rows)
        var generated = FocScenario()
        foc_generate_scenario(&config, &generated)
        start(from: records, scenario: ScenarioSummary(scenario: generated))
    }

    func start(from records: [CharacterRecord], scenario: ScenarioSummary) {
        guard records.count >= 2 else {
            actors = []
            selectedActorID = nil
            message = "Two characters are required."
            return
        }

        self.scenario = scenario
        columns = scenario.width
        rows = scenario.height
        foc_seed(scenario.seed)
        var first = records[0].character
        var second = records[1].character
        let firstInitiative = Int(foc_roll_initiative(&first))
        let secondInitiative = Int(foc_roll_initiative(&second))

        actors = [
            BoardActor(id: UUID(), character: first, side: .player, position: scenario.playerStart, initiative: firstInitiative),
            BoardActor(id: UUID(), character: second, side: .opponent, position: scenario.opponentStart, initiative: secondInitiative)
        ].sorted { $0.initiative > $1.initiative }

        turnIndex = 0
        turnNumber = 1
        outcome = .active
        selectedActorID = currentActorID
        selectedCell = currentActor?.position
        actionEvents = []
        message = "\(scenario.title). Initiative: \(actors.map { "\($0.snapshot.name) \($0.initiative)" }.joined(separator: ", "))."
    }

    func selectCell(_ point: BoardPoint) -> BoardActionResult? {
        guard !outcome.isFinished else {
            message = "Skirmish is complete."
            return nil
        }
        selectedCell = point
        if let actor = actors.first(where: { $0.position == point }) {
            selectedActorID = actor.id
            if actor.id == currentActorID {
                message = "\(actor.snapshot.name) selected."
            } else if let preview = targetingPreview(for: actor.id) {
                message = preview.explanation
            }
            return nil
        }

        if legalMoveCells.contains(point) {
            return moveSelected(to: point)
        }

        message = "No legal action for that cell."
        return nil
    }

    func moveSelected(to point: BoardPoint) -> BoardActionResult {
        guard let selectedActorID, selectedActorID == currentActorID else {
            message = "Only the current actor can move."
            return BoardActionResult(ok: false, message: message, events: [])
        }
        guard legalMoveCells.contains(point) else {
            message = "Move is out of range."
            return BoardActionResult(ok: false, message: message, events: [])
        }
        guard let index = actors.firstIndex(where: { $0.id == selectedActorID }) else {
            message = "No actor selected."
            return BoardActionResult(ok: false, message: message, events: [])
        }

        actors[index].position = point
        actors[index].character.loadout.in_cover = scenario?.coverCells.contains(point) == true
        actors[index].acted = true
        message = "\(actors[index].snapshot.name) moves to \(point.x),\(point.y)."
        updateOutcome()
        advanceTurn()
        return BoardActionResult(ok: true, message: message, events: [])
    }

    func targetingPreview(for targetID: UUID) -> TargetingPreview? {
        guard let selectedActor,
              let target = actors.first(where: { $0.id == targetID }),
              selectedActor.id != target.id else {
            return nil
        }
        let distance = selectedActor.position.distance(to: target.position) * cellYards
        return TargetingPreview(attacker: selectedActor.character, target: target.character, distanceYards: distance)
    }

    func attack(targetID: UUID) -> BoardActionResult {
        guard !outcome.isFinished else {
            message = "Skirmish is complete."
            return BoardActionResult(ok: false, message: message, events: [])
        }
        guard let selectedActorID, selectedActorID == currentActorID else {
            message = "Only the current actor can attack."
            return BoardActionResult(ok: false, message: message, events: [])
        }
        guard let attackerIndex = actors.firstIndex(where: { $0.id == selectedActorID }),
              let targetIndex = actors.firstIndex(where: { $0.id == targetID }),
              attackerIndex != targetIndex else {
            message = "Choose a valid target."
            return BoardActionResult(ok: false, message: message, events: [])
        }

        let distance = actors[attackerIndex].position.distance(to: actors[targetIndex].position) * cellYards
        var attacker = actors[attackerIndex].character
        var target = actors[targetIndex].character
        var buffer = FocEventBuffer()
        var cMessage = [CChar](repeating: 0, count: Int(FOC_ACTION_MESSAGE_LEN))
        let ok = cMessage.withUnsafeMutableBufferPointer { messageBuffer in
            foc_board_ranged_attack(&attacker, &target, CInt(distance), &buffer, messageBuffer.baseAddress, messageBuffer.count)
        }

        actors[attackerIndex].character = attacker
        actors[targetIndex].character = target
        actors[attackerIndex].acted = ok
        let events = buffer.swiftEvents()
        actionEvents.append(contentsOf: events)
        message = String(cString: cMessage)
        if ok {
            updateOutcome()
            advanceTurn()
        }
        return BoardActionResult(ok: ok, message: message, events: events)
    }

    func reloadSelected() -> BoardActionResult {
        performCharacterAction { character, buffer, messageBuffer in
            foc_board_reload(&character, &buffer, messageBuffer.baseAddress, messageBuffer.count)
        }
    }

    func clearJamSelected() -> BoardActionResult {
        performCharacterAction { character, buffer, messageBuffer in
            foc_board_clear_jam(&character, &buffer, messageBuffer.baseAddress, messageBuffer.count)
        }
    }

    func waitSelected() -> BoardActionResult {
        guard let selectedActorID, selectedActorID == currentActorID else {
            message = "Only the current actor can wait."
            return BoardActionResult(ok: false, message: message, events: [])
        }
        if let index = actors.firstIndex(where: { $0.id == selectedActorID }) {
            actors[index].acted = true
            message = "\(actors[index].snapshot.name) waits."
        }
        updateOutcome()
        advanceTurn()
        return BoardActionResult(ok: true, message: message, events: [])
    }

    func runAITurn(difficulty: AIDifficulty) -> BoardActionResult {
        guard !outcome.isFinished else {
            message = "Skirmish is complete."
            return BoardActionResult(ok: false, message: message, events: [])
        }
        guard let currentActor, currentActor.side == .opponent else {
            message = "No AI actor is active."
            return BoardActionResult(ok: false, message: message, events: [])
        }
        guard let enemy = actors.first(where: { $0.side == .player && !$0.snapshot.dead && !$0.snapshot.unconscious }) else {
            updateOutcome()
            message = "AI has no active target."
            return BoardActionResult(ok: false, message: message, events: [])
        }

        var aiCharacter = currentActor.character
        var enemyCharacter = enemy.character
        var action = FocAIAction()
        foc_choose_ai_action(&aiCharacter,
                             &enemyCharacter,
                             CInt(currentActor.position.x),
                             CInt(currentActor.position.y),
                             CInt(enemy.position.x),
                             CInt(enemy.position.y),
                             CInt(columns),
                             CInt(rows),
                             &action)
        let choice = AIChoice(action: action)
        message = choice.explanation

        if difficulty == .easy && turnNumber.isMultiple(of: 3) {
            message = "\(currentActor.snapshot.name) hesitates."
            return waitSelected()
        }

        switch choice.cType {
        case FOC_AI_ATTACK:
            let result = attack(targetID: enemy.id)
            return result.ok ? result : aiMoveOrWait(toward: enemy, preferred: nil, difficulty: difficulty)
        case FOC_AI_RELOAD:
            let result = reloadSelected()
            return result.ok ? result : waitSelected()
        case FOC_AI_CLEAR_JAM:
            let result = clearJamSelected()
            return result.ok ? result : waitSelected()
        case FOC_AI_MOVE:
            if difficulty == .hard, let preview = targetingPreview(for: enemy.id), preview.inRange {
                return attack(targetID: enemy.id)
            }
            return aiMoveOrWait(toward: enemy, preferred: choice.move, difficulty: difficulty)
        default:
            return waitSelected()
        }
    }

    func heal(targetID: UUID) -> BoardActionResult {
        guard let selectedActorID, selectedActorID == currentActorID,
              let healerIndex = actors.firstIndex(where: { $0.id == selectedActorID }),
              let targetIndex = actors.firstIndex(where: { $0.id == targetID }) else {
            message = "Choose a valid healing target."
            return BoardActionResult(ok: false, message: message, events: [])
        }

        var healer = actors[healerIndex].character
        var target = actors[targetIndex].character
        var buffer = FocEventBuffer()
        var cMessage = [CChar](repeating: 0, count: Int(FOC_ACTION_MESSAGE_LEN))
        let ok = cMessage.withUnsafeMutableBufferPointer { messageBuffer in
            foc_board_heal(&healer, &target, &buffer, messageBuffer.baseAddress, messageBuffer.count)
        }

        actors[healerIndex].character = healer
        actors[targetIndex].character = target
        actors[healerIndex].acted = ok
        let events = buffer.swiftEvents()
        actionEvents.append(contentsOf: events)
        message = String(cString: cMessage)
        if ok {
            updateOutcome()
            advanceTurn()
        }
        return BoardActionResult(ok: ok, message: message, events: events)
    }

    func grenade(targetID: UUID) -> BoardActionResult {
        guard let selectedActorID, selectedActorID == currentActorID,
              let attackerIndex = actors.firstIndex(where: { $0.id == selectedActorID }),
              let targetIndex = actors.firstIndex(where: { $0.id == targetID }) else {
            message = "Choose a valid grenade target."
            return BoardActionResult(ok: false, message: message, events: [])
        }

        let distance = actors[attackerIndex].position.distance(to: actors[targetIndex].position) * cellYards
        var attacker = actors[attackerIndex].character
        var target = actors[targetIndex].character
        var buffer = FocEventBuffer()
        var cMessage = [CChar](repeating: 0, count: Int(FOC_ACTION_MESSAGE_LEN))
        let enclosed = scenario?.coverCells.contains(actors[targetIndex].position) == true
        let ok = cMessage.withUnsafeMutableBufferPointer { messageBuffer in
            foc_board_grenade_attack(&attacker, &target, CInt(distance), enclosed, &buffer, messageBuffer.baseAddress, messageBuffer.count)
        }

        actors[attackerIndex].character = attacker
        actors[targetIndex].character = target
        actors[attackerIndex].acted = ok
        let events = buffer.swiftEvents()
        actionEvents.append(contentsOf: events)
        message = String(cString: cMessage)
        if ok {
            updateOutcome()
            advanceTurn()
        }
        return BoardActionResult(ok: ok, message: message, events: events)
    }

    private func performCharacterAction(_ action: (inout FocCharacter, inout FocEventBuffer, UnsafeMutableBufferPointer<CChar>) -> Bool) -> BoardActionResult {
        guard let selectedActorID, selectedActorID == currentActorID,
              let actorIndex = actors.firstIndex(where: { $0.id == selectedActorID }) else {
            message = "Only the current actor can act."
            return BoardActionResult(ok: false, message: message, events: [])
        }

        var character = actors[actorIndex].character
        var buffer = FocEventBuffer()
        var cMessage = [CChar](repeating: 0, count: Int(FOC_ACTION_MESSAGE_LEN))
        let ok = cMessage.withUnsafeMutableBufferPointer { messageBuffer in
            action(&character, &buffer, messageBuffer)
        }
        actors[actorIndex].character = character
        actors[actorIndex].acted = ok
        let events = buffer.swiftEvents()
        actionEvents.append(contentsOf: events)
        message = String(cString: cMessage)
        if ok {
            updateOutcome()
            advanceTurn()
        }
        return BoardActionResult(ok: ok, message: message, events: events)
    }

    private func aiMoveOrWait(toward enemy: BoardActor, preferred: BoardPoint?, difficulty: AIDifficulty) -> BoardActionResult {
        let legal = legalMoveCells
        if let preferred, legal.contains(preferred) {
            return moveSelected(to: preferred)
        }

        guard !legal.isEmpty else {
            message = "\(currentActor?.snapshot.name ?? "AI") has no legal move."
            return waitSelected()
        }

        let ordered = legal.sorted { left, right in
            let leftDistance = left.distance(to: enemy.position)
            let rightDistance = right.distance(to: enemy.position)
            if leftDistance == rightDistance {
                return left.id < right.id
            }
            switch difficulty {
            case .easy:
                return leftDistance > rightDistance
            case .standard, .hard:
                return leftDistance < rightDistance
            }
        }
        return moveSelected(to: ordered[0])
    }

    private func advanceTurn() {
        guard !outcome.isFinished else {
            return
        }
        guard !actors.isEmpty else {
            return
        }
        if actors.allSatisfy(\.acted) {
            for index in actors.indices {
                actors[index].acted = false
            }
            turnNumber += 1
            updateOutcome()
            if outcome.isFinished {
                return
            }
        }

        var next = turnIndex
        for _ in 0..<actors.count {
            next = (next + 1) % actors.count
            if !actors[next].acted && !actors[next].snapshot.dead && !actors[next].snapshot.unconscious {
                turnIndex = next
                selectedActorID = actors[next].id
                selectedCell = actors[next].position
                return
            }
        }

        turnIndex = 0
        selectedActorID = actors.first?.id
        selectedCell = actors.first?.position
    }

    private func updateOutcome() {
        let playerActive = actors.contains { $0.side == .player && !$0.snapshot.dead && !$0.snapshot.unconscious }
        let opponentActive = actors.contains { $0.side == .opponent && !$0.snapshot.dead && !$0.snapshot.unconscious }

        if !opponentActive {
            outcome = .playerWon
            message = "Skirmish complete: player victory."
            return
        }
        if !playerActive {
            outcome = .opponentWon
            message = "Skirmish complete: opponent victory."
            return
        }
        if turnNumber > maxTurns {
            outcome = .opponentWon
            message = "Skirmish complete: time expired."
        }
    }
}
