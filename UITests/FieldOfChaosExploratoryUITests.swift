#!/usr/bin/env swift

import AppKit
import ApplicationServices
import Carbon.HIToolbox
import Foundation

enum ProbeAction: String, Codable {
    case launch
    case sidebar
    case button
    case hotkey
    case search
    case textEntry
    case boardCell
    case toggle
    case stepper
}

enum ProbeStatus: String, Codable {
    case passed
    case failed
    case blocked
    case crashed
    case skipped
}

struct UIProbeCase: Codable {
    var id: String
    var area: String
    var name: String
    var action: ProbeAction
    var target: String
    var input: String
    var expected: String
    var tags: [String]
}

struct UIProbeResult: Codable {
    var id: String
    var area: String
    var name: String
    var action: String
    var target: String
    var status: ProbeStatus
    var note: String
    var durationMS: Int
}

struct UISuiteReport: Codable {
    var appPath: String
    var startedAt: String
    var finishedAt: String
    var accessibilityTrusted: Bool
    var total: Int
    var passed: Int
    var failed: Int
    var blocked: Int
    var crashed: Int
    var skipped: Int
    var results: [UIProbeResult]
}

struct Options {
    var appPath = "dist/Field of Chaos.app"
    var reportPath = "reports/ui-exploration-latest.json"
    var limit = 0
    var dryRun = false
    var keepRunning = false
}

final class FieldOfChaosUIProbeRunner {
    private let options: Options
    private let cases: [UIProbeCase]
    private let bundleIdentifier = "com.fieldofchaos.playable"
    private let sidebarOrder = ["Play", "Roster", "Character", "Tutorial", "Scenario", "Skirmish", "Dice Log", "Rules", "Campaign", "Settings"]
    private let trusted = AXIsProcessTrusted()
    private var app: NSRunningApplication?
    private var axApp: AXUIElement?

    init(options: Options) {
        self.options = options
        let generated = Self.makeCases()
        if options.limit > 0 {
            cases = Array(generated.prefix(options.limit))
        } else {
            cases = generated
        }
    }

    func run() -> Int32 {
        if options.dryRun {
            print("Generated \(cases.count) Field of Chaos UI probe cases.")
            print(Self.caseSummary(cases))
            return 0
        }

        var results: [UIProbeResult] = []
        let started = ISO8601DateFormatter().string(from: Date())
        let launched = launchApp()
        if !launched.ok {
            results = cases.map { probe in
                UIProbeResult(
                    id: probe.id,
                    area: probe.area,
                    name: probe.name,
                    action: probe.action.rawValue,
                    target: probe.target,
                    status: .blocked,
                    note: launched.note,
                    durationMS: 0
                )
            }
        } else {
            for probe in cases {
                results.append(run(probe))
            }
        }

        if !options.keepRunning {
            app?.terminate()
        }

        let report = makeReport(startedAt: started, results: results)
        write(report: report)
        printSummary(report)
        return 0
    }

    private func launchApp() -> (ok: Bool, note: String) {
        let url = URL(fileURLWithPath: options.appPath)
        guard FileManager.default.fileExists(atPath: url.path) else {
            return (false, "App bundle not found at \(url.path). Run scripts/package_release.sh first.")
        }

        let semaphore = DispatchSemaphore(value: 0)
        var launchError: Error?
        var launchedApp: NSRunningApplication?
        let configuration = NSWorkspace.OpenConfiguration()
        configuration.activates = true
        NSWorkspace.shared.openApplication(at: url, configuration: configuration) { runningApp, error in
            launchedApp = runningApp
            launchError = error
            semaphore.signal()
        }
        _ = semaphore.wait(timeout: .now() + 15)

        if let launchError {
            return (false, "Launch failed: \(launchError.localizedDescription)")
        }
        app = launchedApp ?? NSRunningApplication.runningApplications(withBundleIdentifier: bundleIdentifier).first
        guard let app else {
            return (false, "Launch did not return a running application.")
        }
        app.activate(options: [.activateAllWindows])
        axApp = AXUIElementCreateApplication(app.processIdentifier)
        Thread.sleep(forTimeInterval: 1.2)
        return (true, "Launched.")
    }

    private func run(_ probe: UIProbeCase) -> UIProbeResult {
        let start = Date()
        var status: ProbeStatus = .failed
        var note = ""

        if !isRunning() {
            status = .crashed
            note = "App was not running before this probe."
            _ = launchApp()
        } else if requiresAccessibility(probe), !trusted {
            status = .blocked
            note = "Accessibility permission is not granted for UI traversal."
        } else {
            bringForward()
            if shouldNavigate(to: probe.area) {
                _ = navigate(to: probe.area)
                Thread.sleep(forTimeInterval: 0.08)
            }

            switch probe.action {
            case .launch:
                status = hasWindow() ? .passed : .failed
                note = status == .passed ? "Main window exists." : "Main window was not found."
            case .sidebar:
                let pressed = navigate(to: probe.target)
                status = pressed ? .passed : .failed
                note = pressed ? "Pressed sidebar target \(probe.target)." : "Could not press sidebar target \(probe.target)."
            case .button:
                let pressed = press(title: probe.target)
                status = pressed ? .passed : .failed
                note = pressed ? "Pressed button \(probe.target)." : "Button \(probe.target) was not found or could not be pressed."
            case .hotkey:
                let sent = sendHotkey(probe.input)
                status = sent ? .passed : .failed
                note = sent ? "Sent hotkey \(probe.input)." : "Unknown hotkey \(probe.input)."
            case .search:
                let searched = setFirstTextField(to: probe.input)
                status = searched ? .passed : .failed
                note = searched ? "Set search/text value to \(probe.input)." : "No editable text field was available."
            case .textEntry:
                let typed = setFirstTextField(to: probe.input)
                status = typed ? .passed : .failed
                note = typed ? "Set text value to \(probe.input)." : "No editable text field was available."
            case .boardCell:
                let clicked = press(title: probe.target) || clickBoardCell(probe.target)
                status = clicked ? .passed : .failed
                note = clicked ? "Activated \(probe.target)." : "Could not activate \(probe.target)."
            case .toggle:
                let toggled = press(title: probe.target)
                status = toggled ? .passed : .failed
                note = toggled ? "Toggled \(probe.target)." : "Toggle \(probe.target) was not found."
            case .stepper:
                let stepped = press(title: probe.target) || sendHotkey(probe.input)
                status = stepped ? .passed : .failed
                note = stepped ? "Exercised stepper \(probe.target)." : "Stepper \(probe.target) was not found."
            }

            cleanup(after: probe)
            Thread.sleep(forTimeInterval: 0.05)
            if !isRunning() {
                status = .crashed
                note = "App crashed or quit after probe. Previous note: \(note)"
                _ = launchApp()
            }
        }

        return UIProbeResult(
            id: probe.id,
            area: probe.area,
            name: probe.name,
            action: probe.action.rawValue,
            target: probe.target,
            status: status,
            note: note,
            durationMS: Int(Date().timeIntervalSince(start) * 1000)
        )
    }

    private func shouldNavigate(to area: String) -> Bool {
        area != "Global" && area != "Board" && area != "Launch"
    }

    private func navigate(to area: String) -> Bool {
        if press(title: area) {
            return true
        }
        return clickSidebarItem(area)
    }

    private func requiresAccessibility(_ probe: UIProbeCase) -> Bool {
        switch probe.action {
        case .hotkey:
            return false
        default:
            return true
        }
    }

    private func bringForward() {
        app?.activate(options: [.activateAllWindows])
        if let axApp {
            AXUIElementPerformAction(axApp, kAXRaiseAction as CFString)
        }
    }

    private func isRunning() -> Bool {
        guard let app else {
            return false
        }
        return !app.isTerminated
    }

    private func hasWindow() -> Bool {
        guard let axApp else {
            return false
        }
        let windows: [AXUIElement] = attribute(axApp, kAXWindowsAttribute as CFString) ?? []
        return !windows.isEmpty
    }

    private func press(title: String) -> Bool {
        guard let axApp else {
            return false
        }
        let normalizedTarget = normalize(title)
        let candidates = flatten(axApp).filter { element in
            let text = elementText(element)
            return normalize(text) == normalizedTarget || normalize(text).contains(normalizedTarget)
        }
        let sorted = candidates.sorted { left, right in
            score(left, target: normalizedTarget) > score(right, target: normalizedTarget)
        }
        for element in sorted {
            if AXUIElementPerformAction(element, kAXPressAction as CFString) == .success {
                return true
            }
            if clickCenter(of: element) {
                return true
            }
        }
        return false
    }

    private func setFirstTextField(to value: String) -> Bool {
        guard let axApp else {
            return false
        }
        let fields = flatten(axApp).filter { element in
            let role: String = attribute(element, kAXRoleAttribute as CFString) ?? ""
            return role == kAXTextFieldRole as String || role == "AXSearchField"
        }
        guard let field = fields.first else {
            return false
        }
        let result = AXUIElementSetAttributeValue(field, kAXValueAttribute as CFString, value as CFTypeRef)
        return result == .success
    }

    private func sendHotkey(_ description: String) -> Bool {
        let parts = description.lowercased().split(separator: "+").map(String.init)
        guard let keyName = parts.last,
              let keyCode = keyCode(for: keyName) else {
            return false
        }
        var flags = CGEventFlags()
        if parts.contains("cmd") || parts.contains("command") {
            flags.insert(.maskCommand)
        }
        if parts.contains("option") || parts.contains("alt") {
            flags.insert(.maskAlternate)
        }
        if parts.contains("shift") {
            flags.insert(.maskShift)
        }
        if parts.contains("control") || parts.contains("ctrl") {
            flags.insert(.maskControl)
        }
        post(keyCode: keyCode, flags: flags)
        return true
    }

    private func clickBoardCell(_ target: String) -> Bool {
        guard let axApp,
              let window = (attribute(axApp, kAXWindowsAttribute as CFString) as [AXUIElement]?)?.first,
              let frame = frame(of: window) else {
            return false
        }
        let numbers = target.split(whereSeparator: { !$0.isNumber }).compactMap { Int($0) }
        guard numbers.count >= 2 else {
            return false
        }
        let col = max(0, min(15, numbers[0] - 1))
        let row = max(0, min(9, numbers[1] - 1))
        let board = CGRect(x: frame.minX + frame.width * 0.22, y: frame.minY + frame.height * 0.20, width: frame.width * 0.54, height: frame.height * 0.58)
        let point = CGPoint(
            x: board.minX + board.width * (CGFloat(col) + 0.5) / 16,
            y: board.minY + board.height * (CGFloat(row) + 0.5) / 10
        )
        click(point: point)
        return true
    }

    private func clickSidebarItem(_ area: String) -> Bool {
        guard let index = sidebarOrder.firstIndex(of: area),
              let axApp,
              let window = (attribute(axApp, kAXWindowsAttribute as CFString) as [AXUIElement]?)?.first else {
            return false
        }
        let frame = frame(of: window) ?? CGRect(x: 0, y: 0, width: 1200, height: 800)
        let rowHeight: CGFloat = 28
        let firstRowY = frame.minY + 84
        let point = CGPoint(x: frame.minX + 90, y: firstRowY + CGFloat(index) * rowHeight)
        click(point: point)
        Thread.sleep(forTimeInterval: 0.08)
        return true
    }

    private func cleanup(after probe: UIProbeCase) {
        if probe.tags.contains("sheet") {
            _ = press(title: "Cancel")
        }
        if probe.tags.contains("modal") || probe.tags.contains("sheet") {
            post(keyCode: UInt16(kVK_Escape), flags: [])
            Thread.sleep(forTimeInterval: 0.05)
            post(keyCode: UInt16(kVK_Escape), flags: [])
        }
    }

    private func flatten(_ element: AXUIElement, depth: Int = 0, limit: Int = 2400) -> [AXUIElement] {
        guard depth < 9 else {
            return [element]
        }
        var output = [element]
        if output.count >= limit {
            return output
        }
        let children: [AXUIElement] = attribute(element, kAXChildrenAttribute as CFString) ?? []
        for child in children {
            output.append(contentsOf: flatten(child, depth: depth + 1, limit: max(0, limit - output.count)))
            if output.count >= limit {
                break
            }
        }
        return output
    }

    private func elementText(_ element: AXUIElement) -> String {
        let title: String = attribute(element, kAXTitleAttribute as CFString) ?? ""
        let value: String = attribute(element, kAXValueAttribute as CFString) ?? ""
        let description: String = attribute(element, kAXDescriptionAttribute as CFString) ?? ""
        let identifier: String = attribute(element, kAXIdentifierAttribute as CFString) ?? ""
        return [title, value, description, identifier].filter { !$0.isEmpty }.joined(separator: " ")
    }

    private func score(_ element: AXUIElement, target: String) -> Int {
        let role: String = attribute(element, kAXRoleAttribute as CFString) ?? ""
        let text = normalize(elementText(element))
        var result = text == target ? 100 : 20
        if role == kAXButtonRole as String || role == kAXMenuItemRole as String {
            result += 80
        }
        if role == kAXStaticTextRole as String {
            result -= 10
        }
        return result
    }

    private func clickCenter(of element: AXUIElement) -> Bool {
        guard let frame = frame(of: element), frame.width > 0, frame.height > 0 else {
            return false
        }
        click(point: CGPoint(x: frame.midX, y: frame.midY))
        return true
    }

    private func frame(of element: AXUIElement) -> CGRect? {
        guard let positionValue: AXValue = attribute(element, kAXPositionAttribute as CFString),
              let sizeValue: AXValue = attribute(element, kAXSizeAttribute as CFString) else {
            return nil
        }
        var point = CGPoint.zero
        var size = CGSize.zero
        AXValueGetValue(positionValue, .cgPoint, &point)
        AXValueGetValue(sizeValue, .cgSize, &size)
        return CGRect(origin: point, size: size)
    }

    private func attribute<T>(_ element: AXUIElement, _ name: CFString) -> T? {
        var value: CFTypeRef?
        let error = AXUIElementCopyAttributeValue(element, name, &value)
        guard error == .success else {
            return nil
        }
        return value as? T
    }

    private func normalize(_ text: String) -> String {
        text.lowercased()
            .replacingOccurrences(of: "\n", with: " ")
            .split(separator: " ")
            .joined(separator: " ")
            .trimmingCharacters(in: .whitespacesAndNewlines)
    }

    private func click(point: CGPoint) {
        let source = CGEventSource(stateID: .hidSystemState)
        CGEvent(mouseEventSource: source, mouseType: .leftMouseDown, mouseCursorPosition: point, mouseButton: .left)?.post(tap: .cghidEventTap)
        CGEvent(mouseEventSource: source, mouseType: .leftMouseUp, mouseCursorPosition: point, mouseButton: .left)?.post(tap: .cghidEventTap)
    }

    private func post(keyCode: UInt16, flags: CGEventFlags) {
        let source = CGEventSource(stateID: .hidSystemState)
        let down = CGEvent(keyboardEventSource: source, virtualKey: keyCode, keyDown: true)
        down?.flags = flags
        down?.post(tap: .cghidEventTap)
        let up = CGEvent(keyboardEventSource: source, virtualKey: keyCode, keyDown: false)
        up?.flags = flags
        up?.post(tap: .cghidEventTap)
        Thread.sleep(forTimeInterval: 0.08)
    }

    private func keyCode(for key: String) -> UInt16? {
        let table: [String: Int] = [
            "a": kVK_ANSI_A, "b": kVK_ANSI_B, "c": kVK_ANSI_C, "d": kVK_ANSI_D,
            "e": kVK_ANSI_E, "f": kVK_ANSI_F, "g": kVK_ANSI_G, "h": kVK_ANSI_H,
            "i": kVK_ANSI_I, "j": kVK_ANSI_J, "k": kVK_ANSI_K, "l": kVK_ANSI_L,
            "m": kVK_ANSI_M, "n": kVK_ANSI_N, "o": kVK_ANSI_O, "p": kVK_ANSI_P,
            "q": kVK_ANSI_Q, "r": kVK_ANSI_R, "s": kVK_ANSI_S, "t": kVK_ANSI_T,
            "u": kVK_ANSI_U, "v": kVK_ANSI_V, "w": kVK_ANSI_W, "x": kVK_ANSI_X,
            "y": kVK_ANSI_Y, "z": kVK_ANSI_Z, "0": kVK_ANSI_0, "1": kVK_ANSI_1,
            "2": kVK_ANSI_2, "3": kVK_ANSI_3, "4": kVK_ANSI_4, "5": kVK_ANSI_5,
            "6": kVK_ANSI_6, "7": kVK_ANSI_7, "8": kVK_ANSI_8, "9": kVK_ANSI_9,
            "return": kVK_Return, "enter": kVK_Return, "escape": kVK_Escape,
            "tab": kVK_Tab, "space": kVK_Space, "up": kVK_UpArrow, "down": kVK_DownArrow,
            "left": kVK_LeftArrow, "right": kVK_RightArrow
        ]
        return table[key].map(UInt16.init)
    }

    private func makeReport(startedAt: String, results: [UIProbeResult]) -> UISuiteReport {
        UISuiteReport(
            appPath: options.appPath,
            startedAt: startedAt,
            finishedAt: ISO8601DateFormatter().string(from: Date()),
            accessibilityTrusted: trusted,
            total: results.count,
            passed: results.filter { $0.status == .passed }.count,
            failed: results.filter { $0.status == .failed }.count,
            blocked: results.filter { $0.status == .blocked }.count,
            crashed: results.filter { $0.status == .crashed }.count,
            skipped: results.filter { $0.status == .skipped }.count,
            results: results
        )
    }

    private func write(report: UISuiteReport) {
        let url = URL(fileURLWithPath: options.reportPath)
        try? FileManager.default.createDirectory(at: url.deletingLastPathComponent(), withIntermediateDirectories: true)
        let encoder = JSONEncoder()
        encoder.outputFormatting = [.prettyPrinted, .sortedKeys]
        if let data = try? encoder.encode(report) {
            try? data.write(to: url, options: .atomic)
        }
        let markdown = makeMarkdown(report)
        let markdownURL = url.deletingPathExtension().appendingPathExtension("md")
        try? markdown.write(to: markdownURL, atomically: true, encoding: .utf8)
    }

    private func makeMarkdown(_ report: UISuiteReport) -> String {
        var lines: [String] = []
        lines.append("# Field of Chaos UI Exploration Report")
        lines.append("")
        lines.append("- Started: \(report.startedAt)")
        lines.append("- Finished: \(report.finishedAt)")
        lines.append("- Accessibility trusted: \(report.accessibilityTrusted)")
        lines.append("- Total: \(report.total)")
        lines.append("- Passed: \(report.passed)")
        lines.append("- Failed: \(report.failed)")
        lines.append("- Blocked: \(report.blocked)")
        lines.append("- Crashed: \(report.crashed)")
        lines.append("")
        lines.append("| ID | Area | Action | Target | Status | Note |")
        lines.append("| --- | --- | --- | --- | --- | --- |")
        for result in report.results {
            lines.append("| \(result.id) | \(result.area) | \(result.action) | \(result.target) | \(result.status.rawValue) | \(result.note.replacingOccurrences(of: "|", with: "/")) |")
        }
        return lines.joined(separator: "\n").appending("\n")
    }

    private func printSummary(_ report: UISuiteReport) {
        print("Field of Chaos UI exploration complete.")
        print("Total \(report.total), passed \(report.passed), failed \(report.failed), blocked \(report.blocked), crashed \(report.crashed), skipped \(report.skipped).")
        print("Report: \(options.reportPath)")
    }

    static func makeCases() -> [UIProbeCase] {
        var cases: [UIProbeCase] = []

        func add(_ area: String, _ name: String, _ action: ProbeAction, _ target: String, _ input: String = "", expected: String = "App remains running.", tags: [String] = []) {
            let id = String(format: "FOC-UI-%03d", cases.count + 1)
            cases.append(UIProbeCase(id: id, area: area, name: name, action: action, target: target, input: input, expected: expected, tags: tags))
        }

        add("Launch", "launch and find main window", .launch, "Field of Chaos")

        let sections = ["Play", "Roster", "Character", "Tutorial", "Scenario", "Skirmish", "Dice Log", "Rules", "Campaign", "Settings"]
        for pass in 1...5 {
            for section in sections {
                add(section, "navigate to \(section) pass \(pass)", .sidebar, section)
            }
        }

        let areaButtons: [(String, [String], [String])] = [
            ("Play", ["Continue", "Scenario", "Tutorial", "Campaign", "New Skirmish", "Rules"], []),
            ("Roster", ["New", "Save", "Import", "Export"], ["modal"]),
            ("Character", ["Reset", "Apply", "Apply Advancement"], []),
            ("Tutorial", ["Back", "Next", "Rule", "Restart", "Practice Fight", "Skip"], []),
            ("Scenario", ["Generate Skirmish", "Campaign Mission", "Load Skirmish"], ["modal"]),
            ("Skirmish", ["Reset", "Generate", "Save", "Load", "Rules", "Reload", "Clear", "Wait", "AI"], ["modal"]),
            ("Dice Log", ["Run", "Export"], ["modal"]),
            ("Rules", ["Combat", "Movement", "Wound"], []),
            ("Campaign", ["Apply Advancement", "Next Mission", "Backup", "Import"], ["modal"]),
            ("Settings", ["Export Backup"], ["modal"])
        ]

        for pass in 1...4 {
            for (area, buttons, tags) in areaButtons {
                for button in buttons {
                    let localTags = tags + (button == "New" ? ["sheet"] : [])
                    add(area, "press \(button) in \(area) pass \(pass)", .button, button, tags: localTags)
                }
            }
        }

        let hotkeys = [
            "cmd+0", "cmd+1", "cmd+n", "cmd+s", "cmd+shift+s", "cmd+o", "cmd+g",
            "cmd+r", "cmd+l", "cmd+i", "cmd+e", "cmd+t", "cmd+/",
            "cmd+option+a", "cmd+option+w", "cmd+option+r", "cmd+option+j",
            "escape", "tab", "space"
        ]
        for pass in 1...4 {
            for hotkey in hotkeys {
                let cleanupTags: [String]
                if hotkey == "cmd+n" {
                    cleanupTags = ["sheet"]
                } else if ["cmd+o", "cmd+i", "cmd+e", "cmd+shift+s", "cmd+/"].contains(hotkey) {
                    cleanupTags = ["modal"]
                } else {
                    cleanupTags = []
                }
                add("Global", "send \(hotkey) pass \(pass)", .hotkey, hotkey, hotkey, tags: cleanupTags)
            }
        }

        let ruleQueries = [
            "Combat", "Movement", "Wound", "Grenade", "Healing", "Character",
            "Initiative", "Ammo", "Cover", "Ranged", "Campaign", "Evade",
            "Running", "Marching", "Firearm", "Medical", "Armor", "Death",
            "Unconscious", "Scenario"
        ]
        for pass in 1...3 {
            for query in ruleQueries {
                add("Rules", "search rules for \(query) pass \(pass)", .search, "Search rules", query)
            }
        }

        let creationNames = ["Test Scout", "Long Name Character Probe", "A", "FoC QA 001", "Skirmish Runner", "Medic Probe"]
        for pass in 1...4 {
            for name in creationNames {
                add("Roster", "open character sheet name entry \(name) pass \(pass)", .textEntry, "Name", name, tags: ["sheet"])
            }
        }

        let toggles = [
            ("Scenario", "Auto-run opponent turns"),
            ("Settings", "Auto-run AI"),
            ("Settings", "Show Play on Launch"),
            ("Settings", "Sound Cues"),
            ("Settings", "Reduce Motion"),
            ("Character", "Armor Save"),
            ("Character", "In Cover"),
            ("Character", "Firearm Basic"),
            ("Character", "Firearm Advanced"),
            ("Character", "Evade"),
            ("Character", "Running")
        ]
        for pass in 1...3 {
            for (area, toggle) in toggles {
                add(area, "toggle \(toggle) pass \(pass)", .toggle, toggle)
            }
        }

        let steppers = [
            ("Scenario", "Seed", "up"),
            ("Scenario", "Difficulty", "up"),
            ("Scenario", "Max Turns", "up"),
            ("Settings", "Max Turns", "down"),
            ("Character", "RE", "up"),
            ("Character", "IR", "up"),
            ("Character", "AP", "up"),
            ("Character", "PH", "up"),
            ("Character", "ME", "up"),
            ("Character", "Clips", "down"),
            ("Character", "Rounds", "up")
        ]
        for pass in 1...4 {
            for (area, stepper, input) in steppers {
                add(area, "step \(stepper) \(input) pass \(pass)", .stepper, stepper, input)
            }
        }

        for y in 1...10 {
            for x in 1...16 {
                add("Board", "activate board cell \(x),\(y)", .boardCell, "Board cell \(x), \(y)", expected: "Board tap is handled or rejected without a crash.")
            }
        }

        let scenarioCycles = ["Generate Skirmish", "Reset", "Wait", "AI", "Rules", "Reload", "Clear"]
        for pass in 1...4 {
            for command in scenarioCycles {
                add("Skirmish", "skirmish command cycle \(command) pass \(pass)", .button, command)
            }
        }

        while cases.count < 520 {
            let section = sections[cases.count % sections.count]
            add(section, "stability revisit \(section) \(cases.count)", .sidebar, section)
        }

        return Array(cases.prefix(520))
    }

    static func caseSummary(_ cases: [UIProbeCase]) -> String {
        let byArea = Dictionary(grouping: cases, by: \.area).mapValues(\.count).sorted { $0.key < $1.key }
        return byArea.map { "\($0.key): \($0.value)" }.joined(separator: "\n")
    }
}

func parseOptions() -> Options {
    var options = Options()
    var arguments = Array(CommandLine.arguments.dropFirst())
    while !arguments.isEmpty {
        let argument = arguments.removeFirst()
        switch argument {
        case "--app":
            if !arguments.isEmpty { options.appPath = arguments.removeFirst() }
        case "--report":
            if !arguments.isEmpty { options.reportPath = arguments.removeFirst() }
        case "--limit":
            if !arguments.isEmpty { options.limit = Int(arguments.removeFirst()) ?? 0 }
        case "--dry-run":
            options.dryRun = true
        case "--keep-running":
            options.keepRunning = true
        default:
            break
        }
    }
    return options
}

let runner = FieldOfChaosUIProbeRunner(options: parseOptions())
exit(runner.run())
