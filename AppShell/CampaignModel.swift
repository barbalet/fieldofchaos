import FieldOfChaosEngine
import Foundation

struct MissionRecord: Identifiable, Equatable {
    let id = UUID()
    var title: String
    var result: String
    var summary: String
}

final class CampaignState: ObservableObject {
    @Published private(set) var record = FocCampaignRecord()
    @Published var missions: [MissionRecord] = []

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
        var scenario = skirmish.scenario?.cScenario ?? FocScenario()
        let playerWon = skirmish.outcome == .playerWon
        var character = skirmish.actors.first(where: { $0.side == .player })?.character ?? FocCharacter()
        foc_campaign_apply_result(&record, playerWon, &character, &scenario)

        let summary = CampaignSummary(record: record)
        missions.insert(
            MissionRecord(
                title: skirmish.scenario?.title ?? "Mission \(summary.missions)",
                result: playerWon ? "Win" : "Loss",
                summary: summary.lastSummary
            ),
            at: 0
        )
    }

    func spendAdvancement(on character: inout FocCharacter) -> String {
        var message = [CChar](repeating: 0, count: Int(FOC_ACTION_MESSAGE_LEN))
        _ = message.withUnsafeMutableBufferPointer { buffer in
            foc_campaign_spend_advancement(&record, &character, buffer.baseAddress, buffer.count)
        }
        return String(cString: message)
    }
}

