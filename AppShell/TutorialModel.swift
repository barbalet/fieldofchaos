import Foundation

struct TutorialStep: Identifiable, Equatable {
    let id = UUID()
    var title: String
    var instruction: String
    var ruleQuery: String
}

final class TutorialState: ObservableObject {
    @Published var stepIndex = 0
    @Published var completed = false
    @Published var practiceStarted = false

    let steps = [
        TutorialStep(title: "Select", instruction: "Select your trainee on the board and inspect the character panel.", ruleQuery: "Character"),
        TutorialStep(title: "Move", instruction: "Use a highlighted movement cell to reposition. Try standard pace first.", ruleQuery: "Movement"),
        TutorialStep(title: "Target", instruction: "Select the opponent and read the range, dice, and threshold preview.", ruleQuery: "Ranged"),
        TutorialStep(title: "Attack", instruction: "Use Attack to resolve one rules-backed shot and inspect the dice log.", ruleQuery: "Combat"),
        TutorialStep(title: "Recover", instruction: "Try Reload, Clear, Wait, Heal, or Grenade to see action constraints.", ruleQuery: "Healing"),
        TutorialStep(title: "Finish", instruction: "Complete the duel or advance the campaign once an outcome appears.", ruleQuery: "Campaign")
    ]

    var current: TutorialStep {
        steps[min(stepIndex, steps.count - 1)]
    }

    func next() {
        if stepIndex + 1 < steps.count {
            stepIndex += 1
        } else {
            completed = true
        }
    }

    func skip() {
        stepIndex = steps.count - 1
        completed = true
    }

    func startPractice() {
        practiceStarted = true
        completed = false
        stepIndex = steps.count - 1
    }

    func previous() {
        stepIndex = max(0, stepIndex - 1)
    }

    func reset() {
        stepIndex = 0
        completed = false
        practiceStarted = false
    }
}
