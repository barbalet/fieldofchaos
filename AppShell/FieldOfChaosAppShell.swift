import SwiftUI

@main
struct FieldOfChaosAppShell: App {
    @StateObject private var store = GameStore()

    var body: some Scene {
        WindowGroup("Field of Chaos") {
            AppShellView()
                .environmentObject(store)
        }
        .commands {
            CommandMenu("Game") {
                Button("Play") {
                    store.section = .start
                }
                .keyboardShortcut("0", modifiers: [.command])

                Button("New Character") {
                    store.presentCreation = true
                }
                .keyboardShortcut("n")

                Button("Save Character") {
                    store.saveSelectedCharacter()
                }
                .keyboardShortcut("s")

                Button("Import Character") {
                    store.importCharacter()
                }
                .keyboardShortcut("i")

                Button("Export Character") {
                    store.exportSelectedCharacter()
                }
                .keyboardShortcut("e")

                Divider()

                Button("Start Tutorial") {
                    store.startTutorialDuel()
                }
                .keyboardShortcut("t")

                Button("Tutorial Practice Fight") {
                    store.startTutorialPracticeFight()
                }

                Button("Reset Skirmish") {
                    store.resetSkirmish()
                    store.section = .skirmish
                }
                .keyboardShortcut("r")

                Button("Scenario Setup") {
                    store.section = .scenario
                }
                .keyboardShortcut("1", modifiers: [.command])

                Button("Generate Skirmish") {
                    store.generateScenario()
                }
                .keyboardShortcut("g", modifiers: [.command])

                Button("Save Skirmish") {
                    store.saveSkirmish()
                }
                .keyboardShortcut("s", modifiers: [.command, .shift])

                Button("Load Skirmish") {
                    store.loadSkirmish()
                }
                .keyboardShortcut("o", modifiers: [.command])

                Button("Wait Active Actor") {
                    store.waitCurrentActor()
                }
                .keyboardShortcut("w", modifiers: [.command, .option])

                Button("Reload Active Actor") {
                    store.reloadCurrentActor()
                }
                .keyboardShortcut("r", modifiers: [.command, .option])

                Button("Clear Active Jam") {
                    store.clearCurrentActor()
                }
                .keyboardShortcut("j", modifiers: [.command, .option])

                Button("Run AI Turn") {
                    store.runManualAITurn()
                }
                .keyboardShortcut("a", modifiers: [.command, .option])

                Button("Run Duel Log") {
                    store.runDuelLog()
                    store.section = .log
                }
                .keyboardShortcut("l")

                Button("Export Log") {
                    store.exportLog()
                }

                Button("Export Campaign Backup") {
                    store.exportCampaignBackup()
                }

                Button("Import Campaign Backup") {
                    store.importCampaignBackup()
                }

                Button("Rule Reference") {
                    store.showRuleOverlay = true
                }
                .keyboardShortcut("/")
            }
        }

        Settings {
            SettingsView()
                .environmentObject(store)
                .frame(width: 520)
        }
    }
}
