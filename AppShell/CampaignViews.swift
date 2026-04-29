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
                    Picker("Advance", selection: $store.campaignAdvancementChoice) {
                        ForEach(CampaignAdvancementChoice.allCases) { choice in
                            Text(choice.label).tag(choice)
                        }
                    }
                    .frame(width: 190)

                    Button {
                        store.spendCampaignAdvancementOnSelected()
                    } label: {
                        Label("Apply Advancement", systemImage: "arrow.up.circle")
                    }
                    .disabled(summary.advancesAvailable <= 0 || store.selectedRecord == nil)

                    Button {
                        store.generateCampaignScenario()
                    } label: {
                        Label("Next Mission", systemImage: "shuffle")
                    }

                    Button {
                        store.exportCampaignBackup()
                    } label: {
                        Label("Backup", systemImage: "square.and.arrow.up")
                    }

                    Button {
                        store.importCampaignBackup()
                    } label: {
                        Label("Import", systemImage: "square.and.arrow.down")
                    }
                }

                Divider()

                Text("Campaign Roster")
                    .font(.headline)

                List(store.campaign.roster) { record in
                    HStack {
                        VStack(alignment: .leading, spacing: 4) {
                            Text(record.name)
                                .font(.headline)
                            Text("\(record.missions) missions, \(record.xpEarned) XP earned, \(record.injuries) injuries")
                                .foregroundStyle(.secondary)
                        }
                        Spacer()
                        Text(record.status)
                            .foregroundStyle(record.recovery > 0 ? .orange : .green)
                    }
                }
                .frame(minHeight: 120)

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
                        Text("Seed \(mission.seed), difficulty \(mission.difficulty), \(mission.objective.replacingOccurrences(of: "_", with: " ")), +\(mission.rewardXP) XP, \(mission.injuries) injuries")
                            .font(.caption)
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
