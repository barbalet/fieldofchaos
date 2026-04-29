import Foundation

struct RuleTopic: Identifiable, Equatable {
    let id = UUID()
    var title: String
    var body: String

    var preview: String {
        body
            .split(separator: "\n")
            .prefix(2)
            .joined(separator: " ")
    }
}

final class RuleReferenceStore: ObservableObject {
    @Published var topics: [RuleTopic] = []
    @Published var query = ""
    @Published var selectedTopicID: UUID?

    let quickQueries = ["Combat", "Movement", "Wound", "Grenade", "Healing", "Character"]

    var filteredTopics: [RuleTopic] {
        let trimmed = query.trimmingCharacters(in: .whitespacesAndNewlines)
        guard !trimmed.isEmpty else {
            return topics
        }
        return topics.filter { topic in
            topic.title.localizedCaseInsensitiveContains(trimmed) ||
            topic.body.localizedCaseInsensitiveContains(trimmed)
        }
    }

    var selectedTopic: RuleTopic? {
        guard let selectedTopicID else {
            return filteredTopics.first
        }
        return topics.first { $0.id == selectedTopicID } ?? filteredTopics.first
    }

    func load() {
        guard let url = findRulesDocument() else {
            topics = [
                RuleTopic(title: "Combat", body: "Ranged attacks use range bands, dice pools, skill bonuses, and cover or movement penalties."),
                RuleTopic(title: "Wounds", body: "Head or body at zero wounds causes unconsciousness. Zero total wounds means dead."),
                RuleTopic(title: "Movement", body: "Slow, standard, and fast movement are adjusted by Evade, Marching, and Running.")
            ]
            selectedTopicID = topics.first?.id
            return
        }

        let text = (try? String(contentsOf: url)) ?? ""
        topics = parseMarkdownTopics(text)
        selectedTopicID = topics.first?.id
    }

    func focus(_ text: String) {
        query = text
        selectedTopicID = filteredTopics.first?.id
    }

    private func parseMarkdownTopics(_ text: String) -> [RuleTopic] {
        var parsed: [RuleTopic] = []
        var currentTitle: String?
        var currentLines: [String] = []

        for line in text.split(separator: "\n", omittingEmptySubsequences: false) {
            if line.hasPrefix("## ") || line.hasPrefix("### ") {
                if let currentTitle {
                    parsed.append(RuleTopic(title: currentTitle, body: currentLines.joined(separator: "\n").trimmingCharacters(in: .whitespacesAndNewlines)))
                }
                currentTitle = line.replacingOccurrences(of: "#", with: "").trimmingCharacters(in: .whitespaces)
                currentLines = []
            } else if currentTitle != nil {
                currentLines.append(String(line))
            }
        }

        if let currentTitle {
            parsed.append(RuleTopic(title: currentTitle, body: currentLines.joined(separator: "\n").trimmingCharacters(in: .whitespacesAndNewlines)))
        }

        return parsed.filter { !$0.body.isEmpty }
    }

    private func findRulesDocument() -> URL? {
        if let bundled = Bundle.main.url(forResource: "field-of-chaos-rules-natural-english", withExtension: "md") {
            return bundled
        }

        var current = URL(fileURLWithPath: FileManager.default.currentDirectoryPath)
        for _ in 0..<8 {
            let rules = current.appendingPathComponent("docs/field-of-chaos-rules-natural-english.md")
            if FileManager.default.fileExists(atPath: rules.path) {
                return rules
            }
            current.deleteLastPathComponent()
        }
        return nil
    }
}
