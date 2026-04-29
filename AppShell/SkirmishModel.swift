import FieldOfChaosEngine
import Foundation

struct BoardPoint: Hashable, Identifiable, Codable {
    var x: Int
    var y: Int

    var id: String { "\(x),\(y)" }

    func distance(to other: BoardPoint) -> Int {
        abs(x - other.x) + abs(y - other.y)
    }
}

enum ActorSide: String, Codable {
    case player
    case opponent
}

enum SkirmishOutcome: String, Codable {
    case active
    case playerWon
    case opponentWon
    case draw

    var isFinished: Bool {
        self != .active
    }

    var label: String {
        switch self {
        case .active: return "Active"
        case .playerWon: return "Victory"
        case .opponentWon: return "Defeat"
        case .draw: return "Draw"
        }
    }
}

enum AIDifficulty: String, CaseIterable, Identifiable, Codable {
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

struct ActionHint: Identifiable, Equatable {
    var id: String { title }
    var title: String
    var detail: String
    var available: Bool
}

struct CharacterSaveState: Codable {
    var name: String
    var stats: [String: Int]
    var maxWounds: [String: Int]
    var wounds: [String: Int]
    var weapon: WeaponChoice
    var medical: MedicalChoice
    var clips: Int
    var roundsInClip: Int
    var skills: [String]
    var armorSave: Bool
    var inCover: Bool
    var jammed: Bool
    var unconscious: Bool
    var dead: Bool
    var stunnedTurns: Int

    init(character: FocCharacter) {
        let snapshot = CharacterSnapshot(character: character)
        name = snapshot.name
        stats = snapshot.stats
        maxWounds = [
            "Head": Int(character.max_wounds.head),
            "Body": Int(character.max_wounds.body),
            "Left Arm": Int(character.max_wounds.left_arm),
            "Right Arm": Int(character.max_wounds.right_arm),
            "Left Leg": Int(character.max_wounds.left_leg),
            "Right Leg": Int(character.max_wounds.right_leg)
        ]
        wounds = snapshot.wounds
        weapon = snapshot.weapon
        medical = snapshot.medical
        clips = snapshot.clips
        roundsInClip = snapshot.roundsInClip
        skills = snapshot.skills
        armorSave = snapshot.armorSave
        inCover = snapshot.inCover
        jammed = snapshot.jammed
        unconscious = snapshot.unconscious
        dead = snapshot.dead
        stunnedTurns = Int(character.stunned_turns)
    }

    func makeCharacter() -> FocCharacter {
        var character = FocCharacter()
        name.withCString { foc_init_default_character(&character, $0) }
        character.stats.re = CInt(stats["RE"] ?? 0)
        character.stats.ir = CInt(stats["IR"] ?? 0)
        character.stats.ap = CInt(stats["AP"] ?? 0)
        character.stats.ph = CInt(stats["PH"] ?? 0)
        character.stats.me = CInt(stats["ME"] ?? 0)
        character.max_wounds.head = CInt(maxWounds["Head"] ?? 1)
        character.max_wounds.body = CInt(maxWounds["Body"] ?? 4)
        character.max_wounds.left_arm = CInt(maxWounds["Left Arm"] ?? 1)
        character.max_wounds.right_arm = CInt(maxWounds["Right Arm"] ?? 1)
        character.max_wounds.left_leg = CInt(maxWounds["Left Leg"] ?? 2)
        character.max_wounds.right_leg = CInt(maxWounds["Right Leg"] ?? 2)
        character.wounds.head = CInt(wounds["Head"] ?? Int(character.max_wounds.head))
        character.wounds.body = CInt(wounds["Body"] ?? Int(character.max_wounds.body))
        character.wounds.left_arm = CInt(wounds["Left Arm"] ?? Int(character.max_wounds.left_arm))
        character.wounds.right_arm = CInt(wounds["Right Arm"] ?? Int(character.max_wounds.right_arm))
        character.wounds.left_leg = CInt(wounds["Left Leg"] ?? Int(character.max_wounds.left_leg))
        character.wounds.right_leg = CInt(wounds["Right Leg"] ?? Int(character.max_wounds.right_leg))
        character.loadout.weapon = weapon.cValue
        character.loadout.medical = medical.cValue
        character.loadout.clips = CInt(clips)
        character.loadout.rounds_in_clip = CInt(roundsInClip)
        character.loadout.armor_save = armorSave
        character.loadout.in_cover = inCover
        character.skills.firearm_basic = skills.contains("Firearm Basic")
        character.skills.firearm_advanced = skills.contains("Firearm Advanced")
        character.skills.firearm_sniper = skills.contains("Firearm Sniper")
        character.skills.evade = skills.contains("Evade")
        character.skills.marching = skills.contains("Marching")
        character.skills.running = skills.contains("Running")
        character.skills.close_combat = skills.contains("Close Combat")
        character.stunned_turns = CInt(stunnedTurns)
        character.jammed = jammed
        character.unconscious = unconscious
        character.dead = dead
        foc_refresh_character_state(&character)
        character.jammed = jammed
        character.unconscious = unconscious
        character.dead = dead
        return character
    }
}

struct BoardActorSaveState: Codable {
    var id: UUID
    var character: CharacterSaveState
    var side: ActorSide
    var position: BoardPoint
    var acted: Bool
    var initiative: Int

    init(actor: BoardActor) {
        id = actor.id
        character = CharacterSaveState(character: actor.character)
        side = actor.side
        position = actor.position
        acted = actor.acted
        initiative = actor.initiative
    }
}

struct SkirmishSaveState: Codable {
    var schemaVersion = 1
    var seed: UInt32
    var difficulty: Int
    var columns: Int
    var rows: Int
    var cellYards: Int
    var movementPace: MovementPace
    var turnIndex: Int
    var turnNumber: Int
    var maxTurns: Int
    var selectedActorID: UUID?
    var selectedCell: BoardPoint?
    var outcome: SkirmishOutcome
    var actors: [BoardActorSaveState]
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

    var isCurrentActorSelected: Bool {
        selectedActorID == currentActorID && currentActor?.snapshot.dead == false && currentActor?.snapshot.unconscious == false
    }

    var actionHints: [ActionHint] {
        guard let actor = selectedActor else {
            return [ActionHint(title: "Select", detail: "Select the active actor to see actions.", available: false)]
        }
        guard actor.id == currentActorID else {
            return [ActionHint(title: "Waiting", detail: "\(actor.snapshot.name) is not the active actor.", available: false)]
        }
        let snapshot = actor.snapshot
        let living = !snapshot.dead && !snapshot.unconscious && !outcome.isFinished
        let targets = actors.filter { $0.id != actor.id && !$0.snapshot.dead && !$0.snapshot.unconscious }
        let attackCount = targets.filter { targetingPreview(for: $0.id)?.canAttack == true }.count
        return [
            ActionHint(title: "Move", detail: legalMoveCells.isEmpty ? "No legal movement cells." : "\(legalMoveCells.count) legal cells.", available: living && !legalMoveCells.isEmpty),
            ActionHint(title: "Attack", detail: attackCount == 0 ? "No target in range." : "\(attackCount) target(s) in range.", available: living && attackCount > 0),
            ActionHint(title: "Reload", detail: snapshot.clips > 0 ? "Clip available." : "No spare clips.", available: living && snapshot.clips > 0),
            ActionHint(title: "Clear", detail: snapshot.jammed ? "Weapon is jammed." : "Weapon is not jammed.", available: living && snapshot.jammed),
            ActionHint(title: "Recover", detail: snapshot.medical == .none ? "No medical skill." : snapshot.medical.label, available: living && snapshot.medical != .none),
            ActionHint(title: "Wait", detail: "End this actor's action.", available: living)
        ]
    }

    var boardAccessibilitySummary: String {
        let actorSummary = actors.map { actor in
            "\(actor.snapshot.name), \(actor.side.rawValue), column \(actor.position.x + 1), row \(actor.position.y + 1)"
        }.joined(separator: ". ")
        let scenarioTitle = scenario?.title ?? "skirmish"
        return "\(scenarioTitle), \(columns) columns by \(rows) rows. Turn \(turnNumber). \(actorSummary)"
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
        guard !outcome.isFinished,
              let selectedActor,
              selectedActor.id == currentActorID,
              !selectedActor.snapshot.dead,
              !selectedActor.snapshot.unconscious else {
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
        start(from: records, scenario: ScenarioSummary(scenario: generated, difficulty: 1))
    }

    func generateScenario(seed: Int, difficulty: Int, records: [CharacterRecord]) {
        var config = FocScenarioConfig()
        config.seed = UInt32(max(0, seed))
        config.difficulty = CInt(max(1, difficulty))
        config.width = CInt(columns)
        config.height = CInt(rows)
        var generated = FocScenario()
        foc_generate_scenario(&config, &generated)
        start(from: records, scenario: ScenarioSummary(scenario: generated, difficulty: max(1, difficulty)))
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
            if actor.id == currentActorID {
                selectedActorID = actor.id
                message = "\(actor.snapshot.name) selected."
            } else if let preview = targetingPreviewFromCurrent(for: actor.id) {
                message = preview.explanation
            } else {
                message = "\(actor.snapshot.name) is not the active actor."
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

    func targetingPreviewFromCurrent(for targetID: UUID) -> TargetingPreview? {
        guard let currentActor,
              let target = actors.first(where: { $0.id == targetID }),
              currentActor.id != target.id else {
            return nil
        }
        let distance = currentActor.position.distance(to: target.position) * cellYards
        return TargetingPreview(attacker: currentActor.character, target: target.character, distanceYards: distance)
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
        guard !actors[attackerIndex].snapshot.dead,
              !actors[attackerIndex].snapshot.unconscious,
              !actors[targetIndex].snapshot.dead,
              !actors[targetIndex].snapshot.unconscious else {
            message = "Only active actors can attack active targets."
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

    func saveState() -> SkirmishSaveState? {
        guard let scenario else {
            return nil
        }
        return SkirmishSaveState(
            seed: scenario.seed,
            difficulty: scenario.difficulty,
            columns: columns,
            rows: rows,
            cellYards: cellYards,
            movementPace: movementPace,
            turnIndex: turnIndex,
            turnNumber: turnNumber,
            maxTurns: maxTurns,
            selectedActorID: selectedActorID,
            selectedCell: selectedCell,
            outcome: outcome,
            actors: actors.map(BoardActorSaveState.init(actor:))
        )
    }

    func restore(from save: SkirmishSaveState) {
        var config = FocScenarioConfig()
        config.seed = save.seed
        config.difficulty = CInt(max(1, save.difficulty))
        config.width = CInt(max(4, save.columns))
        config.height = CInt(max(4, save.rows))
        var generated = FocScenario()
        foc_generate_scenario(&config, &generated)

        scenario = ScenarioSummary(scenario: generated, difficulty: max(1, save.difficulty))
        columns = max(4, save.columns)
        rows = max(4, save.rows)
        cellYards = max(1, save.cellYards)
        movementPace = save.movementPace
        actors = save.actors.map { saved in
            BoardActor(
                id: saved.id,
                character: saved.character.makeCharacter(),
                side: saved.side,
                position: saved.position,
                acted: saved.acted,
                initiative: saved.initiative
            )
        }
        turnIndex = min(max(0, save.turnIndex), max(0, actors.count - 1))
        turnNumber = max(1, save.turnNumber)
        maxTurns = max(1, save.maxTurns)
        selectedActorID = save.selectedActorID ?? currentActorID
        selectedCell = save.selectedCell ?? currentActor?.position
        outcome = save.outcome
        message = "Skirmish restored at turn \(turnNumber)."
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

        let cover = scenario?.coverCells ?? []
        let ordered = legal.sorted { left, right in
            let leftDistance = left.distance(to: enemy.position)
            let rightDistance = right.distance(to: enemy.position)
            let leftCover = cover.contains(left)
            let rightCover = cover.contains(right)
            if difficulty != .easy && leftCover != rightCover {
                return leftCover
            }
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

        updateOutcome()
        if !outcome.isFinished {
            outcome = .draw
            message = "Skirmish complete: no active actors can act."
        }
    }

    private func updateOutcome() {
        let playerActive = actors.contains { $0.side == .player && !$0.snapshot.dead && !$0.snapshot.unconscious }
        let opponentActive = actors.contains { $0.side == .opponent && !$0.snapshot.dead && !$0.snapshot.unconscious }

        if !playerActive && !opponentActive {
            outcome = .draw
            message = "Skirmish complete: both sides are down."
            return
        }
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
            resolveObjectiveTimeout()
        }
    }

    private func resolveObjectiveTimeout() {
        guard let scenario else {
            outcome = .draw
            message = "Skirmish complete: time expired."
            return
        }
        let player = actors.first { $0.side == .player }
        let opponent = actors.first { $0.side == .opponent }
        switch scenario.objective {
        case "breakthrough":
            if let player, player.position.x >= columns - 2 {
                outcome = .playerWon
                message = "Skirmish complete: breakthrough achieved."
            } else {
                outcome = .opponentWon
                message = "Skirmish complete: breakthrough failed."
            }
        case "hold_center":
            let playerDistance = player?.position.distance(to: scenario.objectivePoint) ?? Int.max
            let opponentDistance = opponent?.position.distance(to: scenario.objectivePoint) ?? Int.max
            if playerDistance < opponentDistance {
                outcome = .playerWon
                message = "Skirmish complete: center held."
            } else if opponentDistance < playerDistance {
                outcome = .opponentWon
                message = "Skirmish complete: center lost."
            } else {
                outcome = .draw
                message = "Skirmish complete: center contested."
            }
        default:
            outcome = .draw
            message = "Skirmish complete: time expired."
        }
    }
}
