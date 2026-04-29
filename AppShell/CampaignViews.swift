import SwiftUI

struct CampaignView: View {
    @EnvironmentObject private var store: GameStore

    var body: some View {
        HSplitView {
            VStack(alignment: .leading, spacing: 18) {
                let summary = store.campaign.summary

                Text("Campaign")
                    .font(.largeTitle)
                    .fontWeight(.semibold)

                Grid(alignment: .leading, horizontalSpacing: 20, verticalSpacing: 8) {
                    GridRow {
                        Text("Missions").foregroundStyle(.secondary)
                        Text("\(summary.missions)")
                        Text("Wins").foregroundStyle(.secondary)
                        Text("\(summary.wins)")
                    }
                    GridRow {
                        Text("Losses").foregroundStyle(.secondary)
                        Text("\(summary.losses)")
                        Text("XP").foregroundStyle(.secondary)
                        Text("\(summary.xp)")
                    }
                    GridRow {
                        Text("Advances").foregroundStyle(.secondary)
                        Text("\(summary.advancesAvailable)")
                        Text("Injuries").foregroundStyle(.secondary)
                        Text("\(summary.injuries)")
                    }
                }

                Text(summary.lastSummary)
                    .foregroundStyle(.secondary)

                HStack {
                    Button {
                        store.spendCampaignAdvancementOnSelected()
                    } label: {
                        Label("Apply Advancement", systemImage: "arrow.up.circle")
                    }
                    .disabled(summary.advancesAvailable <= 0 || store.selectedRecord == nil)

                    Button {
                        store.generateScenario()
                    } label: {
                        Label("Next Scenario", systemImage: "shuffle")
                    }

                    Button {
                        store.exportCampaignBackup()
                    } label: {
                        Label("Backup", systemImage: "square.and.arrow.up")
                    }
                }

                Divider()

                Text("Mission History")
                    .font(.headline)

                List(store.campaign.missions) { mission in
                    VStack(alignment: .leading, spacing: 4) {
                        HStack {
                            Text(mission.title)
                                .font(.headline)
                            Spacer()
                            Text(mission.result)
                                .foregroundStyle(mission.result == "Win" ? .green : .orange)
                        }
                        Text(mission.summary)
                            .foregroundStyle(.secondary)
                    }
                }
            }
            .padding(24)
            .frame(minWidth: 520)

            CharacterWorkspaceView()
                .frame(minWidth: 460)
        }
    }
}
