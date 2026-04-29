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
