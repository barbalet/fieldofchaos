import SwiftUI

struct RuleReferenceView: View {
    @EnvironmentObject private var store: GameStore

    var body: some View {
        RuleReferenceContent(rules: store.rules)
            .environmentObject(store)
            .padding()
    }
}

struct RuleReferenceOverlay: View {
    @EnvironmentObject private var store: GameStore
    @Environment(\.dismiss) private var dismiss

    var body: some View {
        VStack(spacing: 0) {
            ToolbarRow {
                Text("Rule Reference")
                    .font(.headline)
                Spacer()
                Button {
                    dismiss()
                } label: {
                    Label("Close", systemImage: "xmark")
                }
            }

            RuleReferenceContent(rules: store.rules)
                .environmentObject(store)
        }
        .frame(minWidth: 760, minHeight: 520)
    }
}

struct RuleReferenceContent: View {
    @EnvironmentObject private var store: GameStore
    @ObservedObject var rules: RuleReferenceStore

    var body: some View {
        HSplitView {
            VStack(spacing: 10) {
                HStack {
                    ForEach(rules.quickQueries.prefix(3), id: \.self) { query in
                        Button(query) {
                            rules.focus(query)
                        }
                    }
                }
                .padding([.top, .horizontal])

                TextField("Search rules", text: $rules.query)
                    .textFieldStyle(.roundedBorder)
                    .padding(.horizontal)

                List(selection: $rules.selectedTopicID) {
                    ForEach(rules.filteredTopics) { topic in
                        VStack(alignment: .leading, spacing: 4) {
                            Text(topic.title)
                                .font(.headline)
                            Text(topic.preview)
                                .font(.caption)
                                .foregroundStyle(.secondary)
                                .lineLimit(2)
                        }
                        .tag(Optional(topic.id))
                    }
                }
            }
            .frame(minWidth: 260, idealWidth: 320)

            ScrollView {
                VStack(alignment: .leading, spacing: 14) {
                    if let topic = rules.selectedTopic {
                        Text(topic.title)
                            .font(.title)
                            .fontWeight(.semibold)
                        Text(topic.body)
                            .textSelection(.enabled)
                    } else {
                        Text("No matching rule topic.")
                            .foregroundStyle(.secondary)
                    }
                }
                .padding(20)
                .frame(maxWidth: 780, alignment: .leading)
            }
        }
    }
}
