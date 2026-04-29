import FieldOfChaosEngine
import Foundation

enum CampaignAdvancementChoice: String, CaseIterable, Identifiable, Codable {
    case physicalHealth
    case mentalEndurance
    case firearmAdvanced
    case evade
    case running

    var id: String { rawValue }

    var label: String {
        switch self {
        case .physicalHealth: return "+1 PH"
        case .mentalEndurance: return "+1 ME"
        case .firearmAdvanced: return "Firearm Advanced"
        case .evade: return "Evade"
        case .running: return "Running"
        }
    }
}

struct MissionRecord: Identifiable, Equatable, Codable {
    var id = UUID()
    var title: String
    var result: String
    var summary: String
    var seed: UInt32
    var difficulty: Int
    var objective: String
    var rewardXP: Int
    var injuries: Int
}

struct CampaignCharacterRecord: Identifiable, Equatable, Codable {
    var id = UUID()
    var name: String
    var missions: Int
    var xpEarned: Int
    var injuries: Int
    var recovery: Int
    var lastResult: String

    var status: String {
        recovery > 0 ? "Recovering \(recovery)" : "Ready"
    }
}

struct CampaignSaveState: Codable {
    var schemaVersion = 1
    var missions: Int
    var wins: Int
    var losses: Int
    var xp: Int
    var advancesAvailable: Int
    var injuries: Int
    var lastSummary: String
    var history: [MissionRecord]
    var roster: [CampaignCharacterRecord]
}

final class CampaignState: ObservableObject {
    @Published private(set) var record = FocCampaignRecord()
    @Published var missions: [MissionRecord] = []
    @Published var roster: [CampaignCharacterRecord] = []

    var summary: CampaignSummary {
        CampaignSummary(record: record)
    }

    init() {
        foc_campaign_init(&record)
    }

    func apply(skirmish: SkirmishState) {
        guard skirmish.outcome.isFinished else {
            return
        }
        tickRecovery()
        var scenario = skirmish.scenario?.cScenario ?? FocScenario()
        let playerWon = skirmish.outcome == .playerWon
        var character = skirmish.actors.first(where: { $0.side == .player })?.character ?? FocCharacter()
        let woundsLost = max(0, totalWounds(character.max_wounds) - totalWounds(character.wounds))
        foc_campaign_apply_result(&record, playerWon, &character, &scenario)
        let rewardXP = playerWon ? 3 : 1
        recordCharacter(character: character, result: skirmish.outcome.label, rewardXP: rewardXP, injuries: woundsLost)

        let summary = CampaignSummary(record: record)
        missions.insert(
            MissionRecord(
                title: skirmish.scenario?.title ?? "Mission \(summary.missions)",
                result: playerWon ? "Win" : "Loss",
                summary: summary.lastSummary,
                seed: skirmish.scenario?.seed ?? 0,
                difficulty: skirmish.scenario?.difficulty ?? 1,
                objective: skirmish.scenario?.objective ?? "eliminate",
                rewardXP: rewardXP,
                injuries: woundsLost
            ),
            at: 0
        )
    }

    func spendAdvancement(_ choice: CampaignAdvancementChoice, on character: inout FocCharacter) -> String {
        guard record.advances_available > 0 else {
            return "No advancement is available."
        }

        switch choice {
        case .physicalHealth:
            guard character.stats.ph < 10 else { return "PH is already capped." }
            character.stats.ph += 1
        case .mentalEndurance:
            guard character.stats.me < 10 else { return "ME is already capped." }
            character.stats.me += 1
        case .firearmAdvanced:
            guard character.skills.firearm_basic else { return "Firearm Basic is required." }
            guard !character.skills.firearm_advanced else { return "Firearm Advanced already learned." }
            character.skills.firearm_advanced = true
        case .evade:
            guard !character.skills.evade else { return "Evade already learned." }
            character.skills.evade = true
        case .running:
            guard !character.skills.running else { return "Running already learned." }
            character.skills.running = true
        }

        record.advances_available -= 1
        foc_refresh_character_state(&character)
        recordCharacter(character: character, result: "Advanced", rewardXP: 0, injuries: 0)
        return "Advancement applied: \(choice.label)."
    }

    func save(to url: URL) throws {
        let data = try JSONEncoder.prettySorted.encode(saveState())
        try data.write(to: url, options: .atomic)
    }

    static func load(from url: URL) -> CampaignState? {
        guard let data = try? Data(contentsOf: url),
              let save = try? JSONDecoder().decode(CampaignSaveState.self, from: data) else {
            return nil
        }
        let state = CampaignState()
        state.restore(from: save)
        return state
    }

    func restore(from save: CampaignSaveState) {
        record.missions = CInt(save.missions)
        record.wins = CInt(save.wins)
        record.losses = CInt(save.losses)
        record.xp = CInt(save.xp)
        record.advances_available = CInt(save.advancesAvailable)
        record.injuries = CInt(save.injuries)
        writeStringToCArray(save.lastSummary, &record.last_summary)
        missions = save.history
        roster = save.roster
    }

    func saveState() -> CampaignSaveState {
        let summary = CampaignSummary(record: record)
        return CampaignSaveState(
            missions: summary.missions,
            wins: summary.wins,
            losses: summary.losses,
            xp: summary.xp,
            advancesAvailable: summary.advancesAvailable,
            injuries: summary.injuries,
            lastSummary: summary.lastSummary,
            history: missions,
            roster: roster
        )
    }

    private func recordCharacter(character: FocCharacter, result: String, rewardXP: Int, injuries: Int) {
        let name = CharacterSnapshot(character: character).name
        let recovery = injuries > 0 ? max(1, min(3, (injuries + 1) / 2)) : 0
        if let index = roster.firstIndex(where: { $0.name.caseInsensitiveCompare(name) == .orderedSame }) {
            roster[index].missions += result == "Advanced" ? 0 : 1
            roster[index].xpEarned += rewardXP
            roster[index].injuries += injuries
            roster[index].recovery = max(roster[index].recovery, recovery)
            roster[index].lastResult = result
        } else {
            roster.append(
                CampaignCharacterRecord(
                    name: name,
                    missions: result == "Advanced" ? 0 : 1,
                    xpEarned: rewardXP,
                    injuries: injuries,
                    recovery: recovery,
                    lastResult: result
                )
            )
        }
    }

    private func tickRecovery() {
        for index in roster.indices {
            roster[index].recovery = max(0, roster[index].recovery - 1)
        }
    }
}

extension JSONEncoder {
    static var prettySorted: JSONEncoder {
        let encoder = JSONEncoder()
        encoder.outputFormatting = [.prettyPrinted, .sortedKeys]
        return encoder
    }
}
