import SwiftUI

struct SkirmishWorkspaceView: View {
    @EnvironmentObject private var store: GameStore
    @ObservedObject var skirmish: SkirmishState

    var body: some View {
        HSplitView {
            VStack(spacing: 0) {
                ToolbarRow {
                    Picker("Pace", selection: $skirmish.movementPace) {
                        ForEach(MovementPace.allCases) { pace in
                            Text(pace.label).tag(pace)
                        }
                    }
                    .frame(width: 180)

                    Picker("AI", selection: $store.aiDifficulty) {
                        ForEach(AIDifficulty.allCases) { difficulty in
                            Text(difficulty.label).tag(difficulty)
                        }
                    }
                    .frame(width: 150)

                    Button {
                        store.resetSkirmish()
                    } label: {
                        Label("Reset", systemImage: "arrow.clockwise")
                    }

                    Stepper("Seed \(store.scenarioSeed)", value: $store.scenarioSeed, in: 0...999_999)
                        .frame(width: 150)

                    Stepper("Difficulty \(store.scenarioDifficulty)", value: $store.scenarioDifficulty, in: 1...5)
                        .frame(width: 150)

                    Button {
                        store.generateScenario()
                    } label: {
                        Label("Generate", systemImage: "shuffle")
                    }

                    Button {
                        store.showRuleOverlay = true
                    } label: {
                        Label("Rules", systemImage: "book")
                    }
                }

                ZStack {
                    SkirmishMetalView(renderState: skirmish.renderState)
                    BoardTapLayer(skirmish: skirmish) { result in
                        if let result {
                            store.appendBoardEvents(result.events)
                            store.statusMessage = result.message
                        }
                    }
                }
                .aspectRatio(CGFloat(skirmish.columns) / CGFloat(skirmish.rows), contentMode: .fit)
                .padding(16)
            }
            .frame(minWidth: 640)

            SkirmishInspectorView(skirmish: skirmish)
                .frame(minWidth: 300, idealWidth: 340)
        }
    }
}

struct BoardTapLayer: View {
    @ObservedObject var skirmish: SkirmishState
    var onAction: (BoardActionResult?) -> Void

    var body: some View {
        GeometryReader { proxy in
            let cellWidth = proxy.size.width / CGFloat(skirmish.columns)
            let cellHeight = proxy.size.height / CGFloat(skirmish.rows)

            ZStack(alignment: .topLeading) {
                ForEach(0..<skirmish.columns, id: \.self) { x in
                    ForEach(0..<skirmish.rows, id: \.self) { y in
                        let point = BoardPoint(x: x, y: y)
                        Rectangle()
                            .fill(Color.clear)
                            .contentShape(Rectangle())
                            .frame(width: cellWidth, height: cellHeight)
                            .position(x: CGFloat(x) * cellWidth + cellWidth / 2, y: CGFloat(y) * cellHeight + cellHeight / 2)
                            .onTapGesture {
                                onAction(skirmish.selectCell(point))
                            }
                            .accessibilityLabel("Board cell \(x + 1), \(y + 1)")
                    }
                }
            }
        }
    }
}

struct SkirmishInspectorView: View {
    @EnvironmentObject private var store: GameStore
    @ObservedObject var skirmish: SkirmishState

    var body: some View {
        VStack(alignment: .leading, spacing: 16) {
            VStack(alignment: .leading, spacing: 6) {
                Text(skirmish.scenario?.title ?? "Skirmish")
                    .font(.title2)
                    .fontWeight(.semibold)
                Text("Turn \(skirmish.turnNumber)")
                    .font(.headline)
                Text(skirmish.message)
                    .foregroundStyle(.secondary)
                    .fixedSize(horizontal: false, vertical: true)
            }

            if let scenario = skirmish.scenario {
                VStack(alignment: .leading, spacing: 4) {
                    Text(scenario.objective.replacingOccurrences(of: "_", with: " ").capitalized)
                        .font(.headline)
                    Text(scenario.briefing)
                        .foregroundStyle(.secondary)
                }
            }

            if skirmish.outcome.isFinished {
                VStack(alignment: .leading, spacing: 8) {
                    Text(skirmish.outcome == .playerWon ? "Victory" : "Defeat")
                        .font(.title)
                        .fontWeight(.semibold)
                        .foregroundStyle(skirmish.outcome == .playerWon ? .green : .orange)
                Button {
                    store.completeCampaignMission()
                } label: {
                    Label("Advance Campaign", systemImage: "flag.checkered")
                }
                .accessibilityHint("Applies the skirmish result to the campaign record.")
            }
            }

            if let actor = skirmish.selectedActor {
                ActorPanel(actor: actor, current: actor.id == skirmish.currentActorID)

                HStack {
                    Button {
                        let result = skirmish.reloadSelected()
                        store.appendBoardEvents(result.events)
                        store.statusMessage = result.message
                    } label: {
                        Label("Reload", systemImage: "arrow.clockwise")
                    }

                    Button {
                        let result = skirmish.clearJamSelected()
                        store.appendBoardEvents(result.events)
                        store.statusMessage = result.message
                    } label: {
                        Label("Clear", systemImage: "wrench.adjustable")
                    }

                    Button {
                        let result = skirmish.waitSelected()
                        store.appendBoardEvents(result.events)
                        store.statusMessage = result.message
                    } label: {
                        Label("Wait", systemImage: "pause")
                    }

                    if actor.side == .opponent {
                        Button {
                            store.runAITurnIfReady()
                        } label: {
                            Label("AI", systemImage: "cpu")
                        }
                    }
                }
                .disabled(actor.id != skirmish.currentActorID)

                Divider()

                Text("Targets")
                    .font(.headline)

                ForEach(skirmish.actors.filter { $0.id != actor.id }) { target in
                    TargetRow(target: target, preview: skirmish.targetingPreview(for: target.id)) {
                        let result = skirmish.attack(targetID: target.id)
                        store.appendBoardEvents(result.events)
                        store.statusMessage = result.message
                    } heal: {
                        let result = skirmish.heal(targetID: target.id)
                        store.appendBoardEvents(result.events)
                        store.statusMessage = result.message
                    } grenade: {
                        let result = skirmish.grenade(targetID: target.id)
                        store.appendBoardEvents(result.events)
                        store.statusMessage = result.message
                    }
                    .disabled(actor.id != skirmish.currentActorID)
                }
            }

            Divider()

            Text("Order")
                .font(.headline)

            ForEach(skirmish.actors) { actor in
                HStack {
                    Text(actor.snapshot.name)
                    Spacer()
                    Text("\(actor.initiative)")
                        .foregroundStyle(.secondary)
                }
            }

            Spacer()
        }
        .padding(16)
    }
}

struct ActorPanel: View {
    let actor: BoardActor
    let current: Bool

    var body: some View {
        let snapshot = actor.snapshot
        VStack(alignment: .leading, spacing: 8) {
            HStack {
                Text(snapshot.name)
                    .font(.headline)
                Spacer()
                Text(current ? "Current" : actor.acted ? "Acted" : "Waiting")
                    .foregroundStyle(current ? .green : .secondary)
            }
            Grid(alignment: .leading, horizontalSpacing: 12, verticalSpacing: 4) {
                GridRow {
                    Text("Side").foregroundStyle(.secondary)
                    Text(actor.side.rawValue.capitalized)
                }
                GridRow {
                    Text("Position").foregroundStyle(.secondary)
                    Text("\(actor.position.x), \(actor.position.y)")
                }
                GridRow {
                    Text("Weapon").foregroundStyle(.secondary)
                    Text(snapshot.weapon.label)
                }
                GridRow {
                    Text("Ammo").foregroundStyle(.secondary)
                    Text("\(snapshot.roundsInClip) / \(snapshot.clips)")
                }
                GridRow {
                    Text("Wounds").foregroundStyle(.secondary)
                    Text("\(snapshot.woundTotal)")
                }
            }
        }
    }
}

struct TargetRow: View {
    let target: BoardActor
    let preview: TargetingPreview?
    var attack: () -> Void
    var heal: () -> Void
    var grenade: () -> Void

    var body: some View {
        VStack(alignment: .leading, spacing: 6) {
            HStack {
                VStack(alignment: .leading) {
                    Text(target.snapshot.name)
                        .font(.headline)
                    if let preview {
                        Text("\(preview.distanceYards)y, \(preview.range)")
                            .foregroundStyle(.secondary)
                    }
                }
                Spacer()
                Button {
                    attack()
                } label: {
                    Label("Attack", systemImage: "scope")
                }
                .disabled(preview?.canAttack != true)

                Menu {
                    Button("Heal", action: heal)
                    Button("Grenade", action: grenade)
                } label: {
                    Image(systemName: "ellipsis.circle")
                }
            }
            if let preview {
                Text(preview.explanation)
                    .font(.caption)
                    .foregroundStyle(preview.canAttack ? Color.secondary : Color.orange)
                    .fixedSize(horizontal: false, vertical: true)
            }
        }
        .padding(.vertical, 6)
    }
}
