# UI Test Suite

`UITests/FieldOfChaosExploratoryUITests.swift` defines a broad exploratory UI suite for the playable Mac build.

The suite intentionally records instability instead of hiding it. Each probe writes `passed`, `failed`, `blocked`, or `crashed` to the report. This makes the suite useful before the interface is fully hardened.

## Coverage

- Launch and main-window checks.
- Sidebar navigation across Play, Roster, Character, Tutorial, Scenario, Skirmish, Dice Log, Rules, Campaign, and Settings.
- Button probes across the major workflow surfaces.
- Menu and keyboard shortcut probes.
- Rule search probes.
- Character text entry and sheet-opening probes.
- Toggle and stepper probes.
- Board-cell probes across a 16 by 10 skirmish board.
- Repeated stability revisits until the suite reaches 520 probes.

## Run

```bash
sh scripts/run_ui_exploration.sh
```

Run a smaller sample:

```bash
FOC_UI_LIMIT=25 sh scripts/run_ui_exploration.sh
```

List the generated suite without launching the app:

```bash
swift UITests/FieldOfChaosExploratoryUITests.swift --dry-run
```

Reports are written to:

- `reports/ui-exploration-latest.json`
- `reports/ui-exploration-latest.md`

macOS Accessibility permission is required for full UI traversal. Without it, the suite records affected probes as `blocked`.
