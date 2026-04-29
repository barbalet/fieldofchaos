# Architecture

Field of Chaos uses a small native stack:

- C rules engine for deterministic game rules.
- SwiftUI for application state, sheets, menus, logs, campaign screens, and overlays.
- Metal for the skirmish board renderer.

## C Engine

The C engine is the authority for:

- Character records.
- Character JSON import/export.
- Dice and deterministic seeded rolls.
- Combat calculations.
- Turn and duel state transitions.
- Structured events for logs, replay, tests, and UI explanations.
- Character snapshots for SwiftUI panels and future board state.
- In-memory event buffers for Swift log views.
- Board helper APIs for movement, targeting, initiative, reload, jam clearing, and single ranged attacks.
- Scenario, AI, and campaign helper APIs for generated missions and campaign advancement.
- Expanded board action helpers for healing and grenades.

Public C API lives in `src/engine/include/fieldofchaos_engine.h`.

## CLI Utilities

The CLI utilities remain useful for validation and debugging:

- `fieldofchaos create` creates schema-versioned character JSON.
- `fieldofchaos duel` runs deterministic duel simulations.
- `fieldofchaos duel --event-log <path>` writes JSON-lines events for UI and test fixtures.
- `character_create` remains a compatibility utility for direct character generation.

## SwiftUI Shell

The Swift Package app shell proves the app layer can import and call the C engine. It intentionally stays small through cycle 10. Later cycles should add app state wrappers rather than letting SwiftUI own rules logic.

By cycle 20, the Swift layer includes:

- `GameStore` as the observable app/session model.
- `CharacterSnapshot` and `CombatLogEvent` as Swift wrappers around C data.
- Roster, character sheet, creation, settings, and dice log views.
- Local character saves under Application Support.

By cycle 30, the Swift layer also includes:

- Import/export and conflict handling for character JSON.
- A skirmish board model with actor positions, turn order, action gating, movement previews, and targeting previews.
- A Metal-backed board renderer wrapped in SwiftUI.
- Rule reference indexing and overlay search.

By cycle 40, the Swift layer also includes:

- Deterministic scenario setup controls.
- AI turn execution on the skirmish board.
- Skirmish outcome detection and campaign handoff.
- Campaign mission history and advancement spending.

By cycle 50, the Swift layer also includes:

- Tutorial duel workflow.
- Exportable dice logs and campaign backup.
- Rule links from tutorial steps and log events.
- Terrain cover cells and AI difficulty presets.

By cycle 60, the Swift layer also includes:

- AI fallbacks for invalid moves and blocked actions.
- Tutorial completion, skip, restart, and practice-fight paths.
- Autosaved dice log and campaign backup snapshots under Application Support.
- Release packaging script and local app bundle metadata.

## Metal Board

Metal board work begins after cycle 10. The renderer should consume board snapshots and action previews prepared from the C engine and Swift app model. It should not reimplement dice, hit, wound, or advancement rules.

## Data Flow

```text
SwiftUI screens
  -> Swift app/session model
  -> C engine API
  -> state snapshot + structured events
  -> SwiftUI inspectors/logs and Metal board renderer
```

## Persistence

Character JSON is versioned with `schema_version`. The initial release can keep JSON for characters, logs, and campaign state as long as every schema change has validation or migration behavior.

The app writes local character saves under `Application Support/FieldOfChaos/Roster` and keeps recoverable snapshots at `Application Support/FieldOfChaos/last-log.jsonl` and `Application Support/FieldOfChaos/campaign-autosave.json`.
