import SwiftUI

struct AppShellView: View {
    @EnvironmentObject private var store: GameStore

    var body: some View {
        NavigationSplitView {
            List(selection: $store.section) {
                ForEach(AppSection.allCases) { section in
                    Label(section.label, systemImage: section.symbol)
                        .tag(section)
                }
            }
            .navigationTitle("Field of Chaos")
            .frame(minWidth: 180)
        } detail: {
            detailView
                .sheet(isPresented: $store.presentCreation) {
                    CreateCharacterView()
                        .environmentObject(store)
                }
                .sheet(item: $store.importConflict) { _ in
                    ImportConflictView()
                        .environmentObject(store)
                }
                .sheet(isPresented: $store.showRuleOverlay) {
                    RuleReferenceOverlay()
                        .environmentObject(store)
                }
        }
    }

    @ViewBuilder
    private var detailView: some View {
        switch store.section {
        case .roster:
            RosterWorkspaceView()
        case .character:
            CharacterWorkspaceView()
        case .tutorial:
            TutorialWorkspaceView()
        case .skirmish:
            SkirmishWorkspaceView(skirmish: store.skirmish)
        case .log:
            DiceLogView()
        case .rules:
            RuleReferenceView()
        case .campaign:
            CampaignView()
        case .settings:
            SettingsView()
        }
    }
}

struct RosterWorkspaceView: View {
    @EnvironmentObject private var store: GameStore

    var body: some View {
        HSplitView {
            VStack(spacing: 0) {
                ToolbarRow {
                    Button {
                        store.presentCreation = true
                    } label: {
                        Label("New", systemImage: "plus")
                    }

                    Button {
                        store.saveSelectedCharacter()
                    } label: {
                        Label("Save", systemImage: "square.and.arrow.down")
                    }
                    .disabled(store.selectedRecord == nil)

                    Button {
                        store.importCharacter()
                    } label: {
                        Label("Import", systemImage: "square.and.arrow.down.on.square")
                    }

                    Button {
                        store.exportSelectedCharacter()
                    } label: {
                        Label("Export", systemImage: "square.and.arrow.up")
                    }
                    .disabled(store.selectedRecord == nil)
                }

                List(selection: $store.selectedCharacterID) {
                    ForEach(store.characters) { record in
                        CharacterRow(record: record)
                            .tag(Optional(record.id))
                    }
                }
                .onChange(of: store.selectedCharacterID) { id in
                    store.selectCharacter(id)
                }
            }
            .frame(minWidth: 280, idealWidth: 340)

            CharacterWorkspaceView()
                .frame(minWidth: 520)
        }
    }
}

struct ImportConflictView: View {
    @EnvironmentObject private var store: GameStore
    @Environment(\.dismiss) private var dismiss

    var body: some View {
        VStack(alignment: .leading, spacing: 16) {
            Text("Import Conflict")
                .font(.title2)
                .fontWeight(.semibold)
            Text("A character with this name already exists.")
                .foregroundStyle(.secondary)

            HStack {
                Spacer()
                Button("Cancel") {
                    store.importConflict = nil
                    dismiss()
                }
                Button("Duplicate") {
                    store.resolveImportConflict(.duplicate)
                    dismiss()
                }
                Button("Replace") {
                    store.resolveImportConflict(.replace)
                    dismiss()
                }
                .keyboardShortcut(.defaultAction)
            }
        }
        .padding(24)
        .frame(width: 420)
    }
}

struct TutorialWorkspaceView: View {
    @EnvironmentObject private var store: GameStore

    var body: some View {
        HSplitView {
            SkirmishWorkspaceView(skirmish: store.skirmish)
                .frame(minWidth: 700)

            VStack(alignment: .leading, spacing: 16) {
                Text("Tutorial Duel")
                    .font(.largeTitle)
                    .fontWeight(.semibold)

                Text(store.tutorial.current.title)
                    .font(.title2)
                    .fontWeight(.semibold)

                Text(store.tutorial.current.instruction)
                    .foregroundStyle(.secondary)
                    .fixedSize(horizontal: false, vertical: true)

                HStack {
                    Button {
                        store.tutorial.previous()
                    } label: {
                        Label("Back", systemImage: "chevron.left")
                    }
                    .disabled(store.tutorial.stepIndex == 0)

                    Button {
                        store.tutorial.next()
                    } label: {
                        Label(store.tutorial.completed ? "Done" : "Next", systemImage: "chevron.right")
                    }

                    Button {
                        store.focusRule(store.tutorial.current.ruleQuery)
                    } label: {
                        Label("Rule", systemImage: "book")
                    }
                }

                HStack {
                    Button {
                        store.startTutorialDuel()
                    } label: {
                        Label("Restart", systemImage: "arrow.clockwise")
                    }

                    Button {
                        store.startTutorialPracticeFight()
                    } label: {
                        Label("Practice Fight", systemImage: "scope")
                    }

                    Button {
                        store.skipTutorial()
                    } label: {
                        Label("Skip", systemImage: "forward.end")
                    }
                }

                Divider()

                if store.tutorial.completed {
                    Label("Tutorial complete", systemImage: "checkmark.seal.fill")
                        .foregroundStyle(.green)
                } else if store.tutorial.practiceStarted {
                    Label("Practice fight active", systemImage: "scope")
                        .foregroundStyle(.secondary)
                }

                Text("Progress")
                    .font(.headline)
                ForEach(Array(store.tutorial.steps.enumerated()), id: \.offset) { index, step in
                    HStack {
                        Image(systemName: index <= store.tutorial.stepIndex ? "checkmark.circle.fill" : "circle")
                            .foregroundStyle(index <= store.tutorial.stepIndex ? .green : .secondary)
                        Text(step.title)
                    }
                }

                Spacer()
            }
            .padding(24)
            .frame(minWidth: 300, idealWidth: 340)
        }
    }
}

struct CharacterRow: View {
    let record: CharacterRecord

    var body: some View {
        let snapshot = record.snapshot
        VStack(alignment: .leading, spacing: 4) {
            HStack {
                Text(snapshot.name)
                    .font(.headline)
                Spacer()
                Text(record.source.label)
                    .font(.caption)
                    .foregroundStyle(.secondary)
            }
            HStack(spacing: 12) {
                Label(snapshot.weapon.label, systemImage: "scope")
                Label("\(snapshot.woundTotal)W", systemImage: "cross.case")
                Label("\(snapshot.statTotal)S", systemImage: "number")
            }
            .font(.caption)
            .foregroundStyle(.secondary)
        }
        .padding(.vertical, 4)
    }
}

struct CharacterWorkspaceView: View {
    @EnvironmentObject private var store: GameStore

    var body: some View {
        if let record = store.selectedRecord {
            ScrollView {
                VStack(alignment: .leading, spacing: 18) {
                    HeaderView(snapshot: record.snapshot)
                    CharacterEditorView()
                }
                .padding(24)
                .frame(maxWidth: 880, alignment: .leading)
            }
        } else {
            VStack(spacing: 12) {
                Image(systemName: "person.crop.circle.badge.questionmark")
                    .font(.largeTitle)
                    .foregroundStyle(.secondary)
                Text("No Character")
                    .font(.headline)
            }
            .frame(maxWidth: .infinity, maxHeight: .infinity)
        }
    }
}

struct HeaderView: View {
    let snapshot: CharacterSnapshot

    var body: some View {
        VStack(alignment: .leading, spacing: 10) {
            HStack(alignment: .firstTextBaseline) {
                Text(snapshot.name)
                    .font(.largeTitle)
                    .fontWeight(.semibold)
                Spacer()
                StatusPills(snapshot: snapshot)
            }

            Grid(alignment: .leading, horizontalSpacing: 18, verticalSpacing: 6) {
                GridRow {
                    Text("Weapon").foregroundStyle(.secondary)
                    Text(snapshot.weapon.label)
                    Text("Medical").foregroundStyle(.secondary)
                    Text(snapshot.medical.label)
                    Text("Initiative").foregroundStyle(.secondary)
                    Text("+\(snapshot.initiativeModifier)")
                }
                GridRow {
                    Text("Ammo").foregroundStyle(.secondary)
                    Text("\(snapshot.roundsInClip) / \(GameConstants.defaultClipSize), \(snapshot.clips) clips")
                    Text("Wounds").foregroundStyle(.secondary)
                    Text("\(snapshot.woundTotal)")
                    Text("Stats").foregroundStyle(.secondary)
                    Text("\(snapshot.statTotal)")
                }
            }
        }
    }
}

struct StatusPills: View {
    let snapshot: CharacterSnapshot

    var body: some View {
        HStack(spacing: 8) {
            if snapshot.dead {
                Text("Dead").foregroundStyle(.red)
            } else if snapshot.unconscious {
                Text("Unconscious").foregroundStyle(.orange)
            } else {
                Text("Ready").foregroundStyle(.green)
            }
            if snapshot.jammed {
                Text("Jammed").foregroundStyle(.orange)
            }
            if snapshot.inCover {
                Text("Cover").foregroundStyle(.secondary)
            }
        }
        .font(.callout)
    }
}

struct CharacterEditorView: View {
    @EnvironmentObject private var store: GameStore

    var body: some View {
        VStack(alignment: .leading, spacing: 18) {
            HStack {
                TextField("Name", text: $store.draft.name)
                    .textFieldStyle(.roundedBorder)
                    .frame(maxWidth: 320)

                Spacer()

                Button {
                    store.resetDraft()
                } label: {
                    Label("Reset", systemImage: "arrow.counterclockwise")
                }

                Button {
                    store.applyDraft()
                } label: {
                    Label("Apply", systemImage: "checkmark")
                }
                .disabled(store.validationMessage != nil)
            }

            if let validationMessage = store.validationMessage {
                Text(validationMessage)
                    .foregroundStyle(.red)
            }

            Divider()

            Form {
                Section("Stats") {
                    StatStepper(label: "RE", value: $store.draft.re)
                    StatStepper(label: "IR", value: $store.draft.ir)
                    StatStepper(label: "AP", value: $store.draft.ap)
                    StatStepper(label: "PH", value: $store.draft.ph)
                    StatStepper(label: "ME", value: $store.draft.me)
                    LabeledContent("Total", value: "\(store.draft.statTotal)")
                }

                Section("Loadout") {
                    Picker("Weapon", selection: $store.draft.weapon) {
                        ForEach(WeaponChoice.allCases) { weapon in
                            Text(weapon.label).tag(weapon)
                        }
                    }
                    Picker("Medical", selection: $store.draft.medical) {
                        ForEach(MedicalChoice.allCases) { medical in
                            Text(medical.label).tag(medical)
                        }
                    }
                    Stepper("Clips \(store.draft.clips)", value: $store.draft.clips, in: 0...20)
                    Stepper("Rounds \(store.draft.roundsInClip)", value: $store.draft.roundsInClip, in: 0...GameConstants.defaultClipSize)
                    Toggle("Armor Save", isOn: $store.draft.armorSave)
                    Toggle("In Cover", isOn: $store.draft.inCover)
                }

                Section("Skills") {
                    Toggle("Firearm Basic", isOn: $store.draft.firearmBasic)
                    Toggle("Firearm Advanced", isOn: $store.draft.firearmAdvanced)
                    Toggle("Firearm Sniper", isOn: $store.draft.firearmSniper)
                    Toggle("Evade", isOn: $store.draft.evade)
                    Toggle("Marching", isOn: $store.draft.marching)
                    Toggle("Running", isOn: $store.draft.running)
                    Toggle("Close Combat", isOn: $store.draft.closeCombat)
                }

                Section("Wounds") {
                    WoundStepper(label: "Head", value: $store.draft.head, range: 0...1)
                    WoundStepper(label: "Body", value: $store.draft.body, range: 0...4)
                    WoundStepper(label: "Left Arm", value: $store.draft.leftArm, range: 0...1)
                    WoundStepper(label: "Right Arm", value: $store.draft.rightArm, range: 0...1)
                    WoundStepper(label: "Left Leg", value: $store.draft.leftLeg, range: 0...2)
                    WoundStepper(label: "Right Leg", value: $store.draft.rightLeg, range: 0...2)
                    LabeledContent("Total", value: "\(store.draft.woundTotal)")
                }

                Section("Campaign") {
                    let summary = store.campaign.summary
                    LabeledContent("Missions", value: "\(summary.missions)")
                    LabeledContent("XP", value: "\(summary.xp)")
                    LabeledContent("Advances", value: "\(summary.advancesAvailable)")
                    Button {
                        store.spendCampaignAdvancementOnSelected()
                    } label: {
                        Label("Apply Advancement", systemImage: "arrow.up.circle")
                    }
                    .disabled(summary.advancesAvailable <= 0 || store.selectedRecord == nil)
                }
            }
            .formStyle(.grouped)
        }
    }
}

struct StatStepper: View {
    let label: String
    @Binding var value: Int

    var body: some View {
        Stepper("\(label) \(value)", value: $value, in: 0...10)
    }
}

struct WoundStepper: View {
    let label: String
    @Binding var value: Int
    let range: ClosedRange<Int>

    var body: some View {
        Stepper("\(label) \(value)", value: $value, in: range)
    }
}

struct DiceLogView: View {
    @EnvironmentObject private var store: GameStore

    var body: some View {
        VStack(alignment: .leading, spacing: 0) {
            ToolbarRow {
                Picker("Actor", selection: $store.logActorFilter) {
                    ForEach(store.logActors, id: \.self) { actor in
                        Text(actor).tag(actor)
                    }
                }
                .frame(width: 180)

                Picker("Type", selection: $store.logTypeFilter) {
                    ForEach(store.logTypes, id: \.self) { type in
                        Text(type).tag(type)
                    }
                }
                .frame(width: 180)

                Spacer()

                Button {
                    store.runDuelLog()
                } label: {
                    Label("Run", systemImage: "dice")
                }

                Button {
                    store.exportLog()
                } label: {
                    Label("Export", systemImage: "square.and.arrow.up")
                }
            }

            Table(store.filteredLogEvents) {
                TableColumn("Round") { event in
                    Text("\(event.round)")
                }
                .width(54)

                TableColumn("Type") { event in
                    Text(event.type)
                }
                .width(110)

                TableColumn("Actor") { event in
                    Text(event.actor.isEmpty ? "-" : event.actor)
                }
                .width(120)

                TableColumn("Result") { event in
                    Text(event.summary)
                        .lineLimit(2)
                }

                TableColumn("Roll") { event in
                    Text(event.threshold > 0 ? "\(event.roll)+\(event.bonus)=\(event.total) / \(event.threshold)" : "\(event.total)")
                }
                .width(120)

                TableColumn("Rule") { event in
                    Button {
                        store.focusRule(event.ruleQuery)
                    } label: {
                        Image(systemName: "book")
                    }
                    .buttonStyle(.borderless)
                }
                .width(52)
            }
        }
        .navigationTitle("Dice Log")
    }
}

struct CreateCharacterView: View {
    @EnvironmentObject private var store: GameStore
    @Environment(\.dismiss) private var dismiss

    var body: some View {
        VStack(alignment: .leading, spacing: 18) {
            Text("New Character")
                .font(.title2)
                .fontWeight(.semibold)

            TextField("Name", text: $store.creationName)
                .textFieldStyle(.roundedBorder)

            Stepper("Seed \(store.creationSeed)", value: $store.creationSeed, in: 0...999_999)

            HStack {
                Spacer()
                Button("Cancel") {
                    dismiss()
                }
                Button("Create") {
                    store.createCharacter()
                }
                .keyboardShortcut(.defaultAction)
            }
        }
        .padding(24)
        .frame(width: 360)
    }
}

struct SettingsView: View {
    @EnvironmentObject private var store: GameStore

    var body: some View {
        Form {
            Section("Application") {
                HStack(spacing: 14) {
                    AppIconPlaceholder()
                    VStack(alignment: .leading) {
                        Text("Field of Chaos")
                            .font(.headline)
                        Text("Playable app shell")
                            .foregroundStyle(.secondary)
                    }
                }
            }

            Section("Runtime") {
                LabeledContent("C Engine", value: store.engineVersion)
                LabeledContent("Metal", value: store.metalDeviceName)
                LabeledContent("Support Folder", value: store.supportDirectory.path)
                LabeledContent("Roster Folder", value: store.rosterDirectory.path)
            }

            Section("Status") {
                Text(store.statusMessage.isEmpty ? "Ready" : store.statusMessage)
            }

            Section("Campaign") {
                let summary = store.campaign.summary
                LabeledContent("Missions", value: "\(summary.missions)")
                LabeledContent("Wins", value: "\(summary.wins)")
                LabeledContent("XP", value: "\(summary.xp)")
                Button {
                    store.exportCampaignBackup()
                } label: {
                    Label("Export Backup", systemImage: "square.and.arrow.up")
                }
            }
        }
        .formStyle(.grouped)
        .padding()
    }
}

struct AppIconPlaceholder: View {
    var body: some View {
        ZStack {
            RoundedRectangle(cornerRadius: 8)
                .fill(.gray.opacity(0.18))
            Text("FoC")
                .font(.headline)
                .fontWeight(.bold)
        }
        .frame(width: 48, height: 48)
    }
}

struct ToolbarRow<Content: View>: View {
    @ViewBuilder var content: Content

    var body: some View {
        HStack(spacing: 10) {
            content
        }
        .padding(10)
        .background(.bar)
    }
}
