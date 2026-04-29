import FieldOfChaosEngine
import Foundation

enum GameConstants {
    static let defaultClips = Int(FOC_DEFAULT_CLIPS)
    static let defaultClipSize = Int(FOC_DEFAULT_CLIP_SIZE)
}

func stringFromCArray<T>(_ value: T) -> String {
    var copy = value
    return withUnsafePointer(to: &copy) { pointer in
        pointer.withMemoryRebound(to: CChar.self, capacity: MemoryLayout<T>.size) { chars in
            String(cString: chars)
        }
    }
}

func writeStringToCArray<T>(_ string: String, _ value: inout T) {
    withUnsafeMutableBytes(of: &value) { buffer in
        buffer.initializeMemory(as: UInt8.self, repeating: 0)
        let bytes = Array(string.utf8.prefix(max(0, buffer.count - 1)))
        buffer.copyBytes(from: bytes)
    }
}

func totalWounds(_ wounds: FocWounds) -> Int {
    Int(wounds.head + wounds.body + wounds.left_arm + wounds.right_arm + wounds.left_leg + wounds.right_leg)
}

enum WeaponChoice: String, CaseIterable, Identifiable, Codable {
    case none
    case sniper
    case rifle
    case submg
    case shotgun

    var id: String { rawValue }

    var label: String {
        switch self {
        case .none: return "None"
        case .sniper: return "Sniper"
        case .rifle: return "Rifle"
        case .submg: return "SubMG"
        case .shotgun: return "Shotgun"
        }
    }

    var cValue: FocWeaponType {
        rawValue.withCString { foc_parse_weapon($0) }
    }

    static func from(_ value: FocWeaponType) -> WeaponChoice {
        let raw = String(cString: foc_weapon_to_string(value))
        return WeaponChoice(rawValue: raw) ?? .none
    }
}

enum MedicalChoice: String, CaseIterable, Identifiable, Codable {
    case none
    case firstAid = "first_aid"
    case paramedic
    case pharma

    var id: String { rawValue }

    var label: String {
        switch self {
        case .none: return "None"
        case .firstAid: return "First Aid"
        case .paramedic: return "Paramedic"
        case .pharma: return "Pharma"
        }
    }

    var cValue: FocMedicalLevel {
        rawValue.withCString { foc_parse_medical($0) }
    }

    static func from(_ value: FocMedicalLevel) -> MedicalChoice {
        let raw = String(cString: foc_medical_to_string(value))
        return MedicalChoice(rawValue: raw) ?? .none
    }
}

struct CharacterSnapshot: Equatable {
    var name: String
    var schemaVersion: Int
    var stats: [String: Int]
    var wounds: [String: Int]
    var statTotal: Int
    var woundTotal: Int
    var weapon: WeaponChoice
    var medical: MedicalChoice
    var clips: Int
    var roundsInClip: Int
    var initiativeModifier: Int
    var skills: [String]
    var armorSave: Bool
    var inCover: Bool
    var jammed: Bool
    var unconscious: Bool
    var dead: Bool

    init(character: FocCharacter) {
        var mutableCharacter = character
        var snapshot = FocCharacterSnapshot()
        foc_make_character_snapshot(&mutableCharacter, &snapshot)

        name = stringFromCArray(snapshot.name)
        schemaVersion = Int(snapshot.schema_version)
        stats = [
            "RE": Int(snapshot.stats.re),
            "IR": Int(snapshot.stats.ir),
            "AP": Int(snapshot.stats.ap),
            "PH": Int(snapshot.stats.ph),
            "ME": Int(snapshot.stats.me)
        ]
        wounds = [
            "Head": Int(snapshot.wounds.head),
            "Body": Int(snapshot.wounds.body),
            "Left Arm": Int(snapshot.wounds.left_arm),
            "Right Arm": Int(snapshot.wounds.right_arm),
            "Left Leg": Int(snapshot.wounds.left_leg),
            "Right Leg": Int(snapshot.wounds.right_leg)
        ]
        statTotal = Int(snapshot.stat_total)
        woundTotal = Int(snapshot.wound_total)
        weapon = WeaponChoice.from(snapshot.weapon)
        medical = MedicalChoice.from(snapshot.medical)
        clips = Int(snapshot.clips)
        roundsInClip = Int(snapshot.rounds_in_clip)
        initiativeModifier = Int(snapshot.initiative_modifier)
        armorSave = snapshot.armor_save
        inCover = snapshot.in_cover
        jammed = snapshot.jammed
        unconscious = snapshot.unconscious
        dead = snapshot.dead

        var skillNames: [String] = []
        if snapshot.firearm_basic { skillNames.append("Firearm Basic") }
        if snapshot.firearm_advanced { skillNames.append("Firearm Advanced") }
        if snapshot.firearm_sniper { skillNames.append("Firearm Sniper") }
        if snapshot.evade { skillNames.append("Evade") }
        if snapshot.marching { skillNames.append("Marching") }
        if snapshot.running { skillNames.append("Running") }
        if snapshot.close_combat { skillNames.append("Close Combat") }
        skills = skillNames
    }
}

struct CombatLogEvent: Identifiable, Equatable {
    var id: Int
    var seed: UInt32
    var round: Int
    var type: String
    var actorID: Int
    var targetID: Int
    var actor: String
    var target: String
    var summary: String
    var roll: Int
    var bonus: Int
    var total: Int
    var threshold: Int
    var location: String
    var amount: Int

    var ruleQuery: String {
        switch type {
        case "attack": return "Ranged"
        case "wound": return "Wound"
        case "ammo": return "Ammo"
        case "status": return summary.localizedCaseInsensitiveContains("heal") ? "Healing" : "Combat"
        case "roll": return "Initiative"
        default: return "Combat"
        }
    }

    init(event: FocEvent) {
        id = Int(event.event_id)
        seed = event.seed
        round = Int(event.round)
        type = String(cString: foc_event_type_to_string(event.type))
        actorID = Int(event.actor_id)
        targetID = Int(event.target_id)
        actor = stringFromCArray(event.actor)
        target = stringFromCArray(event.target)
        summary = stringFromCArray(event.summary)
        roll = Int(event.roll)
        bonus = Int(event.bonus)
        total = Int(event.total)
        threshold = Int(event.threshold)
        location = String(cString: foc_location_to_string(event.location))
        amount = Int(event.amount)
    }
}

enum MovementPace: String, CaseIterable, Identifiable, Codable {
    case slow
    case standard
    case fast

    var id: String { rawValue }

    var label: String {
        switch self {
        case .slow: return "Slow"
        case .standard: return "Standard"
        case .fast: return "Fast"
        }
    }

    var cValue: FocMovementPace {
        switch self {
        case .slow: return FOC_PACE_SLOW
        case .standard: return FOC_PACE_STANDARD
        case .fast: return FOC_PACE_FAST
        }
    }
}

struct TargetingPreview: Equatable {
    var inRange: Bool
    var canAttack: Bool
    var range: String
    var distanceYards: Int
    var maxRangeYards: Int
    var diceCount: Int
    var bonus: Int
    var threshold: Int
    var explanation: String

    init(attacker: FocCharacter, target: FocCharacter, distanceYards: Int) {
        var attackerCopy = attacker
        var targetCopy = target
        var preview = FocTargetingPreview()
        foc_make_targeting_preview(&attackerCopy, &targetCopy, CInt(distanceYards), &preview)

        inRange = preview.in_range
        canAttack = preview.can_attack
        range = String(cString: foc_range_to_string(preview.range))
        self.distanceYards = Int(preview.distance_yards)
        maxRangeYards = Int(preview.max_range_yards)
        diceCount = Int(preview.dice_count)
        bonus = Int(preview.bonus)
        threshold = Int(preview.threshold)
        explanation = stringFromCArray(preview.explanation)
    }
}

extension FocEventBuffer {
    func swiftEvents() -> [CombatLogEvent] {
        var copy = self
        var events: [CombatLogEvent] = []
        let count = Int(foc_event_buffer_count(&copy))
        for index in 0..<count {
            if let eventPointer = foc_event_buffer_get(&copy, CInt(index)) {
                events.append(CombatLogEvent(event: eventPointer.pointee))
            }
        }
        return events
    }
}

struct ScenarioSummary {
    var seed: UInt32
    var difficulty: Int
    var width: Int
    var height: Int
    var objective: String
    var playerStart: BoardPoint
    var opponentStart: BoardPoint
    var objectivePoint: BoardPoint
    var coverCells: Set<BoardPoint>
    var title: String
    var briefing: String
    var cScenario: FocScenario

    init(scenario: FocScenario, difficulty: Int = 1) {
        cScenario = scenario
        seed = scenario.seed
        self.difficulty = max(1, difficulty)
        width = Int(scenario.width)
        height = Int(scenario.height)
        objective = String(cString: foc_objective_to_string(scenario.objective))
        playerStart = BoardPoint(x: Int(scenario.player_x), y: Int(scenario.player_y))
        opponentStart = BoardPoint(x: Int(scenario.opponent_x), y: Int(scenario.opponent_y))
        objectivePoint = BoardPoint(x: Int(scenario.objective_x), y: Int(scenario.objective_y))
        coverCells = ScenarioSummary.makeCoverCells(seed: scenario.seed, width: Int(scenario.width), height: Int(scenario.height), objective: objectivePoint)
        title = stringFromCArray(scenario.title)
        briefing = stringFromCArray(scenario.briefing)
    }

    private static func makeCoverCells(seed: UInt32, width: Int, height: Int, objective: BoardPoint) -> Set<BoardPoint> {
        var cells: Set<BoardPoint> = []
        var value = Int(seed % 997)
        let count = max(6, min(18, (width * height) / 12))
        for index in 0..<count {
            value = (value * 37 + 17 + index) % max(1, width * height)
            let x = value % max(1, width)
            let y = (value / max(1, width)) % max(1, height)
            let point = BoardPoint(x: x, y: y)
            if point != objective && x > 0 && x < width - 1 {
                cells.insert(point)
            }
        }
        return cells
    }
}

struct AIChoice: Equatable {
    var type: String
    var move: BoardPoint
    var explanation: String
    var cType: FocAIActionType

    init(action: FocAIAction) {
        cType = action.type
        type = String(cString: foc_ai_action_to_string(action.type))
        move = BoardPoint(x: Int(action.move_x), y: Int(action.move_y))
        explanation = stringFromCArray(action.explanation)
    }
}

struct CampaignSummary: Equatable {
    var missions: Int
    var wins: Int
    var losses: Int
    var xp: Int
    var advancesAvailable: Int
    var injuries: Int
    var lastSummary: String

    init(record: FocCampaignRecord) {
        missions = Int(record.missions)
        wins = Int(record.wins)
        losses = Int(record.losses)
        xp = Int(record.xp)
        advancesAvailable = Int(record.advances_available)
        injuries = Int(record.injuries)
        lastSummary = stringFromCArray(record.last_summary)
    }
}
