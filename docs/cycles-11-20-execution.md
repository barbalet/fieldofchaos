# Cycles 11-20 Execution Notes

This log records the second ten cycles from `docs/playable-game-60-cycle-plan.md` as implemented work.

## Cycle Results

| Cycle | Result |
|---:|---|
| 11 | Added `GameStore`, Swift app/session state, C character wrappers, and sample roster loading. |
| 12 | Added roster browser rows with import/source status and selected character summaries. |
| 13 | Added character detail header and read-only snapshot display for stats, loadout, wounds, skills, status, and initiative modifier. |
| 14 | Added editable character draft controls with C-backed stat validation and bounded steppers/toggles. |
| 15 | Added app menu commands, settings shell, app icon placeholder, Metal/runtime status, and local roster folder creation. |
| 16 | Added SwiftUI dice/result log powered by C duel events through an in-memory event buffer. |
| 17 | Added C character snapshot and event buffer APIs for UI consumption after combat state changes. |
| 18 | Added actor/type filters and tabular compact log inspection for full generated duel logs. |
| 19 | Added in-app character creation from the existing seeded C generation rules and local save command. |
| 20 | Added character-sheet alpha with editable stats, skills, loadout, wounds, ammo, and validation messaging. |

## App Surface At Cycle 20

- `Roster`: sample and saved characters, selection, source status, save, and new character entry.
- `Character`: editable character sheet backed by C-compatible data.
- `Dice Log`: generated duel event stream with actor/type filters.
- `Settings`: engine version, Metal device, local roster folder, app identity placeholder, and status.

## New C APIs

- `foc_make_character_snapshot`
- `foc_event_buffer_clear`
- `foc_event_buffer_count`
- `foc_event_buffer_truncated`
- `foc_event_buffer_get`
- `foc_run_duel_to_buffer`

## Build And Test Expectations

- `make test` from `src/` covers deterministic generation, schema load/save, future schema rejection, event emission, snapshots, and buffered duel events.
- `swift build` from the repository root builds the SwiftUI app shell and the C engine target.

## Cycle 20 Boundary

The app can inspect, edit, create, locally save, and log characters, but it does not yet support full import/export conflict workflows. Those are cycle 21-22. The skirmish board begins in cycle 23.

