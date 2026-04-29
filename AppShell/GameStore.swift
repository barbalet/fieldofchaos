import FieldOfChaosEngine
import AppKit
import Foundation
import Metal
import UniformTypeIdentifiers

enum AppSection: String, CaseIterable, Identifiable {
    case roster
    case character
    case tutorial
    case skirmish
    case log
    case rules
    case campaign
    case settings

    var id: String { rawValue }

    var label: String {
        switch self {
        case .roster: return "Roster"
        case .character: return "Character"
        case .tutorial: return "Tutorial"
        case .skirmish: return "Skirmish"
        case .log: return "Dice Log"
        case .rules: return "Rules"
        case .campaign: return "Campaign"
        case .settings: return "Settings"
        }
    }

    var symbol: String {
        switch self {
        case .roster: return "person.3"
        case .character: return "person.text.rectangle"
        case .tutorial: return "graduationcap"
        case .skirmish: return "scope"
        case .log: return "list.bullet.rectangle"
        case .rules: return "book"
        case .campaign: return "flag.checkered"
        case .settings: return "gearshape"
        }
    }
}

enum CharacterSource: Equatable {
    case sample(URL?)
    case local(URL)
    case unsaved

    var label: String {
        switch self {
        case .sample: return "Sample"
        case .local: return "Saved"
        case .unsaved: return "Unsaved"
        }
    }
}

struct CharacterRecord: Identifiable {
    let id: UUID
    var character: FocCharacter
    var source: CharacterSource

    var snapshot: CharacterSnapshot {
        CharacterSnapshot(character: character)
    }
}

struct CharacterDraft: Equatable {
    var name = ""
    var re = 0
    var ir = 0
    var ap = 0
    var ph = 0
    var me = 0
    var weapon: WeaponChoice = .rifle
    var medical: MedicalChoice = .none
    var clips = GameConstants.defaultClips
    var roundsInClip = GameConstants.defaultClipSize
    var firearmBasic = true
    var firearmAdvanced = false
    var firearmSniper = false
    var evade = false
    var marching = false
    var running = false
    var closeCombat = false
    var armorSave = false
    var inCover = false
    var head = 1
    var body = 4
    var leftArm = 1
    var rightArm = 1
    var leftLeg = 2
    var rightLeg = 2

    var statTotal: Int { re + ir + ap + ph + me }
    var woundTotal: Int { head + body + leftArm + rightArm + leftLeg + rightLeg }

    init() { }

    init(record: CharacterRecord) {
        let snapshot = record.snapshot
        name = snapshot.name
        re = snapshot.stats["RE"] ?? 0
        ir = snapshot.stats["IR"] ?? 0
        ap = snapshot.stats["AP"] ?? 0
        ph = snapshot.stats["PH"] ?? 0
        me = snapshot.stats["ME"] ?? 0
        weapon = snapshot.weapon
        medical = snapshot.medical
        clips = snapshot.clips
        roundsInClip = snapshot.roundsInClip
        firearmBasic = snapshot.skills.contains("Firearm Basic")
        firearmAdvanced = snapshot.skills.contains("Firearm Advanced")
        firearmSniper = snapshot.skills.contains("Firearm Sniper")
        evade = snapshot.skills.contains("Evade")
        marching = snapshot.skills.contains("Marching")
        running = snapshot.skills.contains("Running")
        closeCombat = snapshot.skills.contains("Close Combat")
        armorSave = snapshot.armorSave
        inCover = snapshot.inCover
        head = snapshot.wounds["Head"] ?? 1
        body = snapshot.wounds["Body"] ?? 4
        leftArm = snapshot.wounds["Left Arm"] ?? 1
        rightArm = snapshot.wounds["Right Arm"] ?? 1
        leftLeg = snapshot.wounds["Left Leg"] ?? 2
        rightLeg = snapshot.wounds["Right Leg"] ?? 2
    }

    func makeCharacter() -> FocCharacter {
        var character = FocCharacter()
        name.withCString { foc_init_default_character(&character, $0) }

        character.stats.re = CInt(re)
        character.stats.ir = CInt(ir)
        character.stats.ap = CInt(ap)
        character.stats.ph = CInt(ph)
        character.stats.me = CInt(me)
        character.skills.firearm_basic = firearmBasic
        character.skills.firearm_advanced = firearmAdvanced
        character.skills.firearm_sniper = firearmSniper
        character.skills.evade = evade
        character.skills.marching = marching
        character.skills.running = running
        character.skills.close_combat = closeCombat
        character.loadout.weapon = weapon.cValue
        character.loadout.medical = medical.cValue
        character.loadout.clips = CInt(clips)
        character.loadout.rounds_in_clip = CInt(roundsInClip)
        character.loadout.armor_save = armorSave
        character.loadout.in_cover = inCover
        character.wounds.head = CInt(head)
        character.wounds.body = CInt(body)
        character.wounds.left_arm = CInt(leftArm)
        character.wounds.right_arm = CInt(rightArm)
        character.wounds.left_leg = CInt(leftLeg)
        character.wounds.right_leg = CInt(rightLeg)
        foc_refresh_character_state(&character)
        return character
    }
}

final class GameStore: ObservableObject {
    @Published var section: AppSection = .roster
    @Published var characters: [CharacterRecord] = []
    @Published var selectedCharacterID: UUID?
    @Published var draft = CharacterDraft()
    @Published var presentCreation = false
    @Published var creationName = "New Character"
    @Published var creationSeed = 20
    @Published var statusMessage = ""
    @Published var logEvents: [CombatLogEvent] = []
    @Published var logActorFilter = "All"
    @Published var logTypeFilter = "All"
    @Published var importConflict: ImportConflict?
    @Published var skirmish = SkirmishState()
    @Published var rules = RuleReferenceStore()
    @Published var showRuleOverlay = false
    @Published var campaign = CampaignState()
    @Published var scenarioSeed = 401
    @Published var scenarioDifficulty = 1
    @Published var aiDifficulty: AIDifficulty = .standard
    @Published var tutorial = TutorialState()

    let engineVersion: String
    let metalDeviceName: String
    let supportDirectory: URL
    let rosterDirectory: URL

    init() {
        engineVersion = String(cString: foc_engine_version())
        metalDeviceName = MTLCreateSystemDefaultDevice()?.name ?? "Metal device unavailable"
        supportDirectory = GameStore.makeSupportDirectory()
        rosterDirectory = GameStore.makeRosterDirectory(in: supportDirectory)
        rules.load()
        loadInitialRoster()
        resetSkirmish()
    }

    var selectedRecord: CharacterRecord? {
        characters.first { $0.id == selectedCharacterID }
    }

    var validationMessage: String? {
        if draft.name.trimmingCharacters(in: .whitespacesAndNewlines).isEmpty {
            return "Name is required."
        }
        if draft.clips < 0 || draft.roundsInClip < 0 {
            return "Ammo values must be non-negative."
        }
        var stats = FocStats()
        stats.re = CInt(draft.re)
        stats.ir = CInt(draft.ir)
        stats.ap = CInt(draft.ap)
        stats.ph = CInt(draft.ph)
        stats.me = CInt(draft.me)
        var error = [CChar](repeating: 0, count: 160)
        let valid = error.withUnsafeMutableBufferPointer { buffer in
            foc_validate_stats(&stats, buffer.baseAddress, buffer.count)
        }
        return valid ? nil : String(cString: error)
    }

    var filteredLogEvents: [CombatLogEvent] {
        logEvents.filter { event in
            let actorMatch = logActorFilter == "All" || event.actor == logActorFilter
            let typeMatch = logTypeFilter == "All" || event.type == logTypeFilter
            return actorMatch && typeMatch
        }
    }

    var logActors: [String] {
        ["All"] + Array(Set(logEvents.map(\.actor).filter { !$0.isEmpty })).sorted()
    }

    var logTypes: [String] {
        ["All"] + Array(Set(logEvents.map(\.type))).sorted()
    }

    func selectCharacter(_ id: UUID?) {
        selectedCharacterID = id
        if let record = selectedRecord {
            draft = CharacterDraft(record: record)
        }
    }

    func applyDraft() {
        guard validationMessage == nil, let selectedCharacterID else {
            return
        }
        guard let index = characters.firstIndex(where: { $0.id == selectedCharacterID }) else {
            return
        }
        characters[index].character = draft.makeCharacter()
        characters[index].source = .unsaved
        statusMessage = "Character updated."
    }

    func resetDraft() {
        if let record = selectedRecord {
            draft = CharacterDraft(record: record)
        }
    }

    func createCharacter() {
        let cleanName = creationName.trimmingCharacters(in: .whitespacesAndNewlines)
        guard !cleanName.isEmpty else {
            statusMessage = "Name is required."
            return
        }

        var character = FocCharacter()
        foc_seed(UInt32(max(0, creationSeed)))
        cleanName.withCString { foc_init_default_character(&character, $0) }
        let record = CharacterRecord(id: UUID(), character: character, source: .unsaved)
        characters.append(record)
        selectCharacter(record.id)
        section = .character
        presentCreation = false
        statusMessage = "Character created."
    }

    func saveSelectedCharacter() {
        guard let selectedCharacterID,
              let index = characters.firstIndex(where: { $0.id == selectedCharacterID }) else {
            statusMessage = "No character selected."
            return
        }

        var character = characters[index].character
        let name = characters[index].snapshot.name
        let fileURL = localURL(for: name, id: characters[index].id)
        let ok = fileURL.path.withCString { path in
            foc_save_character_json(path, &character)
        }

        if ok {
            characters[index].source = .local(fileURL)
            statusMessage = "Saved \(name)."
        } else {
            statusMessage = "Save failed."
        }
    }

    func importCharacter() {
        let panel = NSOpenPanel()
        panel.allowedContentTypes = [.json]
        panel.allowsMultipleSelection = false
        panel.canChooseDirectories = false
        panel.begin { [weak self] response in
            guard response == .OK, let url = panel.url else {
                return
            }
            self?.importCharacter(from: url, strategy: .ask)
        }
    }

    func exportSelectedCharacter() {
        guard let selectedRecord else {
            statusMessage = "No character selected."
            return
        }

        let panel = NSSavePanel()
        panel.allowedContentTypes = [.json]
        panel.nameFieldStringValue = "\(selectedRecord.snapshot.name).json"
        panel.begin { [weak self] response in
            guard response == .OK, let url = panel.url else {
                return
            }
            self?.exportSelectedCharacter(to: url)
        }
    }

    func importCharacter(from url: URL, strategy: ImportStrategy) {
        guard let character = loadCharacter(from: url) else {
            return
        }

        let importedName = CharacterSnapshot(character: character).name
        if let existing = characters.first(where: { $0.snapshot.name.caseInsensitiveCompare(importedName) == .orderedSame }) {
            switch strategy {
            case .ask:
                importConflict = ImportConflict(existingID: existing.id, imported: character, sourceURL: url)
                statusMessage = "Import conflict for \(importedName)."
                return
            case .replace:
                replaceCharacter(existing.id, with: character, source: .unsaved)
                statusMessage = "Replaced \(importedName)."
                return
            case .duplicate:
                addImportedCharacter(character, sourceURL: url, forceDuplicateName: true)
                return
            }
        }

        addImportedCharacter(character, sourceURL: url, forceDuplicateName: false)
    }

    func resolveImportConflict(_ strategy: ImportStrategy) {
        guard let conflict = importConflict else {
            return
        }
        switch strategy {
        case .replace:
            replaceCharacter(conflict.existingID, with: conflict.imported, source: .unsaved)
            statusMessage = "Imported character replaced existing copy."
        case .duplicate:
            addImportedCharacter(conflict.imported, sourceURL: conflict.sourceURL, forceDuplicateName: true)
        case .ask:
            break
        }
        importConflict = nil
    }

    func exportSelectedCharacter(to url: URL) {
        guard let selectedRecord else {
            statusMessage = "No character selected."
            return
        }
        var character = selectedRecord.character
        let ok = url.path.withCString { path in
            foc_save_character_json(path, &character)
        }
        statusMessage = ok ? "Exported \(selectedRecord.snapshot.name)." : "Export failed."
    }

    func runDuelLog() {
        guard characters.count >= 2 else {
            statusMessage = "Two characters are required."
            return
        }

        var a = characters[0].character
        var b = characters[1].character
        var config = FocDuelConfig()
        config.range = "close".withCString { foc_parse_range($0) }
        config.max_rounds = 12
        config.verbose = false
        var buffer = FocEventBuffer()

        foc_seed(42)
        _ = foc_run_duel_to_buffer(&a, &b, &config, &buffer)

        var events: [CombatLogEvent] = []
        let count = Int(foc_event_buffer_count(&buffer))
        for index in 0..<count {
            if let eventPointer = foc_event_buffer_get(&buffer, CInt(index)) {
                events.append(CombatLogEvent(event: eventPointer.pointee))
            }
        }

        logEvents = events
        logActorFilter = "All"
        logTypeFilter = "All"
        statusMessage = foc_event_buffer_truncated(&buffer) ? "Duel log truncated." : "Duel log generated."
    }

    func resetSkirmish() {
        skirmish = SkirmishState()
        skirmish.start(from: Array(characters.prefix(2)))
        statusMessage = "Skirmish reset."
    }

    func generateScenario() {
        skirmish.generateScenario(seed: scenarioSeed, difficulty: scenarioDifficulty, records: Array(characters.prefix(2)))
        statusMessage = skirmish.scenario?.briefing ?? "Scenario generated."
        section = .skirmish
    }

    func appendBoardEvents(_ events: [CombatLogEvent]) {
        guard !events.isEmpty else {
            return
        }
        logEvents.append(contentsOf: events)
        autosaveLogSnapshot()
    }

    func runAITurnIfReady() {
        guard skirmish.currentActor?.side == .opponent else {
            return
        }
        let result = skirmish.runAITurn(difficulty: aiDifficulty)
        appendBoardEvents(result.events)
        statusMessage = result.message
    }

    func startTutorialDuel() {
        tutorial.reset()
        foc_seed(410)
        var trainee = FocCharacter()
        "Trainee".withCString { foc_init_default_character(&trainee, $0) }
        trainee.stats.ph = 8
        trainee.stats.me = 7
        trainee.skills.firearm_basic = true
        trainee.skills.evade = true
        trainee.loadout.weapon = WeaponChoice.rifle.cValue

        foc_seed(411)
        var drone = FocCharacter()
        "Training Drone".withCString { foc_init_default_character(&drone, $0) }
        drone.stats.ph = 5
        drone.stats.me = 4
        drone.loadout.weapon = WeaponChoice.rifle.cValue
        drone.loadout.in_cover = true

        let records = [
            CharacterRecord(id: UUID(), character: trainee, source: .sample(nil)),
            CharacterRecord(id: UUID(), character: drone, source: .sample(nil))
        ]
        skirmish.generateScenario(seed: 410, difficulty: 1, records: records)
        aiDifficulty = .easy
        section = .tutorial
        statusMessage = "Tutorial duel started."
    }

    func startTutorialPracticeFight() {
        tutorial.startPractice()
        foc_seed(510)
        var trainee = FocCharacter()
        "Practice Trainee".withCString { foc_init_default_character(&trainee, $0) }
        trainee.stats.ph = 8
        trainee.stats.me = 7
        trainee.skills.firearm_basic = true
        trainee.skills.running = true
        trainee.loadout.weapon = WeaponChoice.rifle.cValue

        foc_seed(511)
        var opponent = FocCharacter()
        "Practice Rival".withCString { foc_init_default_character(&opponent, $0) }
        opponent.stats.ph = 6
        opponent.stats.me = 6
        opponent.skills.firearm_basic = true
        opponent.loadout.weapon = WeaponChoice.submg.cValue

        let records = [
            CharacterRecord(id: UUID(), character: trainee, source: .sample(nil)),
            CharacterRecord(id: UUID(), character: opponent, source: .sample(nil))
        ]
        skirmish.generateScenario(seed: 510, difficulty: 2, records: records)
        aiDifficulty = .standard
        section = .tutorial
        statusMessage = "Tutorial practice fight started."
    }

    func skipTutorial() {
        tutorial.skip()
        section = .skirmish
        statusMessage = "Tutorial marked complete."
    }

    func focusRule(_ query: String) {
        rules.focus(query)
        showRuleOverlay = true
    }

    func exportLog() {
        let panel = NSSavePanel()
        panel.allowedContentTypes = [.json]
        panel.nameFieldStringValue = "field-of-chaos-log.jsonl"
        panel.begin { [weak self] response in
            guard response == .OK, let url = panel.url else {
                return
            }
            self?.writeLog(to: url)
        }
    }

    func writeLog(to url: URL, announce: Bool = true) {
        let lines = logEvents.compactMap { event -> String? in
            let object: [String: Any] = [
                "event_id": event.id,
                "seed": event.seed,
                "round": event.round,
                "type": event.type,
                "actor": event.actor,
                "target": event.target,
                "summary": event.summary,
                "roll": event.roll,
                "bonus": event.bonus,
                "total": event.total,
                "threshold": event.threshold,
                "location": event.location,
                "amount": event.amount,
                "rule_query": event.ruleQuery
            ]
            guard let data = try? JSONSerialization.data(withJSONObject: object, options: [.sortedKeys]) else {
                return nil
            }
            return String(data: data, encoding: .utf8)
        }
        do {
            try lines.joined(separator: "\n").appending("\n").write(to: url, atomically: true, encoding: .utf8)
            if announce {
                statusMessage = "Log exported."
            }
        } catch {
            if announce {
                statusMessage = "Log export failed."
            }
        }
    }

    func exportCampaignBackup() {
        let panel = NSSavePanel()
        panel.allowedContentTypes = [.json]
        panel.nameFieldStringValue = "field-of-chaos-campaign.json"
        panel.begin { [weak self] response in
            guard response == .OK, let url = panel.url else {
                return
            }
            self?.writeCampaignBackup(to: url)
        }
    }

    func writeCampaignBackup(to url: URL, announce: Bool = true) {
        let summary = campaign.summary
        let object: [String: Any] = [
            "missions": summary.missions,
            "wins": summary.wins,
            "losses": summary.losses,
            "xp": summary.xp,
            "advances_available": summary.advancesAvailable,
            "injuries": summary.injuries,
            "last_summary": summary.lastSummary,
            "history": campaign.missions.map { mission in
                [
                    "title": mission.title,
                    "result": mission.result,
                    "summary": mission.summary
                ]
            }
        ]
        do {
            let data = try JSONSerialization.data(withJSONObject: object, options: [.prettyPrinted, .sortedKeys])
            try data.write(to: url, options: .atomic)
            if announce {
                statusMessage = "Campaign backup exported."
            }
        } catch {
            if announce {
                statusMessage = "Campaign backup failed."
            }
        }
    }

    func completeCampaignMission() {
        guard skirmish.outcome.isFinished else {
            statusMessage = "Skirmish is not complete."
            return
        }
        campaign.apply(skirmish: skirmish)
        autosaveCampaignBackup()
        statusMessage = campaign.summary.lastSummary
        section = .campaign
    }

    func spendCampaignAdvancementOnSelected() {
        guard let selectedCharacterID,
              let index = characters.firstIndex(where: { $0.id == selectedCharacterID }) else {
            statusMessage = "No character selected."
            return
        }
        var character = characters[index].character
        statusMessage = campaign.spendAdvancement(on: &character)
        characters[index].character = character
        characters[index].source = .unsaved
        draft = CharacterDraft(record: characters[index])
        autosaveCampaignBackup()
    }

    private func loadInitialRoster() {
        characters = loadSavedCharacters() + loadSampleCharacters()
        if characters.isEmpty {
            createFallbackRoster()
        }
        selectCharacter(characters.first?.id)
        runDuelLog()
    }

    private func addImportedCharacter(_ character: FocCharacter, sourceURL: URL, forceDuplicateName: Bool) {
        var newCharacter = character
        if forceDuplicateName {
            let original = CharacterSnapshot(character: newCharacter).name
            setCharacterName(&newCharacter, "\(original) Copy")
        }
        let record = CharacterRecord(id: UUID(), character: newCharacter, source: .unsaved)
        characters.append(record)
        selectCharacter(record.id)
        section = .character
        statusMessage = "Imported \(record.snapshot.name)."
    }

    private func replaceCharacter(_ id: UUID, with character: FocCharacter, source: CharacterSource) {
        guard let index = characters.firstIndex(where: { $0.id == id }) else {
            return
        }
        characters[index].character = character
        characters[index].source = source
        selectCharacter(id)
    }

    private func setCharacterName(_ character: inout FocCharacter, _ name: String) {
        var renamed = character
        name.withCString { foc_init_default_character(&renamed, $0) }
        renamed.stats = character.stats
        renamed.max_wounds = character.max_wounds
        renamed.wounds = character.wounds
        renamed.skills = character.skills
        renamed.loadout = character.loadout
        renamed.stunned_turns = character.stunned_turns
        renamed.jammed = character.jammed
        renamed.unconscious = character.unconscious
        renamed.dead = character.dead
        character = renamed
    }

    private func loadSavedCharacters() -> [CharacterRecord] {
        let files = (try? FileManager.default.contentsOfDirectory(at: rosterDirectory, includingPropertiesForKeys: nil)) ?? []
        return files
            .filter { $0.pathExtension == "json" }
            .compactMap { url in
                loadCharacter(from: url).map { CharacterRecord(id: UUID(), character: $0, source: .local(url)) }
            }
    }

    private func loadSampleCharacters() -> [CharacterRecord] {
        guard let docsDirectory = findRepositoryDocsDirectory() else {
            return ["alice", "bob"].compactMap { stem in
                Bundle.main.url(forResource: stem, withExtension: "json").flatMap { url in
                    loadCharacter(from: url).map { CharacterRecord(id: UUID(), character: $0, source: .sample(url)) }
                }
            }
        }
        return ["alice.json", "bob.json"].compactMap { filename in
            let url = docsDirectory.appendingPathComponent(filename)
            return loadCharacter(from: url).map { CharacterRecord(id: UUID(), character: $0, source: .sample(url)) }
        }
    }

    private func createFallbackRoster() {
        foc_seed(42)
        var alice = FocCharacter()
        "Alice".withCString { foc_init_default_character(&alice, $0) }
        alice.skills.evade = true

        foc_seed(99)
        var bob = FocCharacter()
        "Bob".withCString { foc_init_default_character(&bob, $0) }
        bob.loadout.weapon = WeaponChoice.submg.cValue
        bob.skills.firearm_advanced = true
        bob.skills.running = true

        characters = [
            CharacterRecord(id: UUID(), character: alice, source: .sample(nil)),
            CharacterRecord(id: UUID(), character: bob, source: .sample(nil))
        ]
    }

    private func loadCharacter(from url: URL) -> FocCharacter? {
        var character = FocCharacter()
        var error = [CChar](repeating: 0, count: 256)
        let ok = url.path.withCString { path in
            error.withUnsafeMutableBufferPointer { buffer in
                foc_load_character_json(path, &character, buffer.baseAddress, buffer.count)
            }
        }
        if !ok {
            statusMessage = String(cString: error)
        }
        return ok ? character : nil
    }

    private func localURL(for name: String, id: UUID) -> URL {
        let allowed = CharacterSet.alphanumerics.union(CharacterSet(charactersIn: "-_"))
        let sanitized = name.unicodeScalars.map { allowed.contains($0) ? Character($0) : "-" }
        let stem = String(sanitized).trimmingCharacters(in: CharacterSet(charactersIn: "-"))
        let fileStem = stem.isEmpty ? "character" : stem
        return rosterDirectory.appendingPathComponent("\(fileStem)-\(id.uuidString.prefix(8)).json")
    }

    private func findRepositoryDocsDirectory() -> URL? {
        var current = URL(fileURLWithPath: FileManager.default.currentDirectoryPath)
        for _ in 0..<8 {
            let docs = current.appendingPathComponent("docs", isDirectory: true)
            if FileManager.default.fileExists(atPath: docs.appendingPathComponent("alice.json").path) {
                return docs
            }
            current.deleteLastPathComponent()
        }
        return nil
    }

    private func autosaveLogSnapshot() {
        writeLog(to: supportDirectory.appendingPathComponent("last-log.jsonl"), announce: false)
    }

    private func autosaveCampaignBackup() {
        writeCampaignBackup(to: supportDirectory.appendingPathComponent("campaign-autosave.json"), announce: false)
    }

    private static func makeSupportDirectory() -> URL {
        let base = FileManager.default.urls(for: .applicationSupportDirectory, in: .userDomainMask).first
            ?? URL(fileURLWithPath: NSTemporaryDirectory())
        let directory = base.appendingPathComponent("FieldOfChaos", isDirectory: true)
        try? FileManager.default.createDirectory(at: directory, withIntermediateDirectories: true)
        return directory
    }

    private static func makeRosterDirectory(in supportDirectory: URL) -> URL {
        let directory = supportDirectory.appendingPathComponent("Roster", isDirectory: true)
        try? FileManager.default.createDirectory(at: directory, withIntermediateDirectories: true)
        return directory
    }
}

enum ImportStrategy {
    case ask
    case replace
    case duplicate
}

struct ImportConflict: Identifiable {
    let id = UUID()
    let existingID: UUID
    let imported: FocCharacter
    let sourceURL: URL
}
