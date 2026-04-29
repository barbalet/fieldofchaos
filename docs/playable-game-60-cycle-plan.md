# Field of Chaos Playable Game Plan - 60 Cycles

## Goal

Deliver a packaged, playable Field of Chaos skirmish game after cycle 60 using a C rules engine, SwiftUI application shell, and Metal-rendered tactical board.

Playable at cycle 60 means:

- A player can launch the app, complete a tutorial duel, create or import characters, play skirmish scenarios against an AI opponent, inspect rules, review a dice/result log, advance characters through a campaign loop, save/export character data, and install or run the packaged app.
- The C rules implementation remains the authoritative rules engine.
- SwiftUI owns menus, character sheets, campaign screens, overlays, logs, and onboarding.
- Metal owns the skirmish board rendering, visual state, range previews, movement previews, targeting, and effects.

## Feature Tracks

| ID | Feature | Primary Track | Completion Target |
|---|---|---|---|
| F1 | Playable skirmish board | Metal board, C combat state | Cycle 31 alpha, cycle 49 content complete |
| F2 | Character-sheet UI | SwiftUI forms and inspectors | Cycle 20 alpha, cycle 38 complete |
| F3 | Campaign advancement | C progression model, SwiftUI campaign views | Cycle 40 alpha, cycle 52 complete |
| F4 | AI opponent | C tactics, SwiftUI/Metal integration | Cycle 35 alpha, cycle 51 complete |
| F5 | Scenario generator | C generation service, SwiftUI setup flow | Cycle 33 alpha, cycle 47 complete |
| F6 | Dice/result log | C event stream, SwiftUI log UI | Cycle 18 alpha, cycle 43 complete |
| F7 | Save/export characters | JSON schema, import/export UI | Cycle 22 alpha, cycle 44 complete |
| F8 | Tutorial duel | Scripted scenario, guided UI | Cycle 42 alpha, cycle 55 complete |
| F9 | Rule reference overlay | Indexed rules docs, SwiftUI overlay | Cycle 29 alpha, cycle 46 complete |
| F10 | Packaged app | SwiftUI app, Metal, C bridge, distribution | Cycle 15 shell, cycle 60 release candidate |

## Technical Direction

- Keep the existing C prototype as the source of truth, then split it into reusable modules: `character`, `rules`, `combat`, `scenario`, `campaign`, `ai`, `serialization`, and `event_log`.
- Expose a stable C API for Swift via a module map or Swift Package target.
- Store game state and characters in versioned JSON first. Add stricter schema validation before campaign data depends on it.
- Use deterministic seeds for tests, tutorial replay, scenario generation, and AI regression tests.
- Render the board with Metal through an `MTKView` wrapped in SwiftUI. Use SwiftUI for all non-board interface.
- Build early vertical slices: one character, one board, one AI, one scenario, one saved result. Expand breadth only after the loop is playable.

## Cycle Map

| Cycle | Focus | Deliverable | Acceptance Check |
|---:|---|---|---|
| 1 | Product baseline | Define the target playable loop, core player actions, and cycle 60 acceptance checklist. | A one-page scope can be used to reject nonessential work. |
| 2 | Rules inventory | Compare implemented C rules against extracted rules and tag missing mechanics. | Backlog marks current duel coverage, gaps, and rule risk. |
| 3 | Architecture | Design C engine boundaries, Swift bridge shape, JSON schema versions, and app module layout. | Public interfaces are sketched before code movement begins. |
| 4 | Repo setup | Add app/package skeleton plan, build targets, test folders, and CI expectations. | `make` remains supported and app build path is documented. |
| 5 | C engine extraction | Split character model and serialization from the CLI. | CLI character creation still produces compatible JSON. |
| 6 | C combat extraction | Split duel/combat into reusable state update functions. | Existing duel behavior is covered by deterministic tests. |
| 7 | Event model | Add structured combat events for rolls, hits, wounds, reloads, jams, and state changes. | CLI can emit a machine-readable event stream. |
| 8 | Dice/result log foundation | Add roll IDs, seeds, actor IDs, and human-readable summaries to events. | A duel can be replayed from logs well enough for debugging. |
| 9 | Character schema | Version character JSON and add load/save validation. | Invalid or older character data reports actionable errors. |
| 10 | Swift package shell | Create SwiftUI app target with C engine linked in. | App launches and calls a trivial C engine function. |
| 11 | App state model | Add Swift observable game/session models wrapping C handles or snapshots. | SwiftUI can display loaded sample characters. |
| 12 | Character list UI | Build roster browser with import status and selected character summary. | Alice/Bob sample data display in-app. |
| 13 | Character detail UI | Build read-only character-sheet view for stats, skills, loadout, and wounds. | Sheet mirrors JSON values accurately. |
| 14 | Character editing | Add bounded stat/loadout/skill editing with validation. | Invalid totals and unsupported weapons are blocked in UI. |
| 15 | Packaged app shell | Add app icon placeholder, menu commands, settings shell, and local document folder. | A local debug app build can be run repeatedly. |
| 16 | Log UI alpha | Display C event stream in a SwiftUI dice/result log panel. | Duel events show rolls, totals, target numbers, and outcomes. |
| 17 | Combat snapshot API | Add C snapshot objects for board/UI consumption after each action. | Swift can request current wounds, ammo, status, and initiative. |
| 18 | Dice/result log alpha | Add filtering by actor/event type and compact/expanded log rows. | A full duel can be inspected without terminal output. |
| 19 | Character creation flow | Build guided character creation around the existing generation rules. | Player can create and save a valid character in-app. |
| 20 | Character-sheet alpha | Add editable sheet sections, wound display, ammo/loadout controls, and validation messaging. | Character sheet is usable without leaving the app. |
| 21 | Save/export foundation | Add import/export services for characters and roster storage. | App can persist created characters between launches. |
| 22 | Save/export alpha | Add share/export commands for character JSON and import conflict handling. | Exported character can be reimported and matches original data. |
| 23 | Board data model | Define grid/terrain/actor positions, movement costs, line of sight inputs, and range bands. | A scenario state can place actors on a board. |
| 24 | Metal renderer setup | Wrap `MTKView` in SwiftUI and draw a basic board with stable camera scaling. | Board renders in desktop window without blank frames. |
| 25 | Board selection | Add hover/selection/highlight model for actors and cells. | Player can select an actor and see legal interaction targets. |
| 26 | Movement preview | Implement movement ranges from rules-derived pace and injury modifiers. | Leg wounds alter legal movement preview. |
| 27 | Targeting preview | Show weapon range bands, cover modifiers, and hit chance explanation from C. | Target preview matches engine calculation. |
| 28 | Board actions | Route move, wait, reload, clear jam, and ranged attack commands through C. | A manual two-character skirmish can progress on the board. |
| 29 | Rule overlay alpha | Index rules markdown by topic and add in-app overlay search. | Player can open relevant rule text during combat. |
| 30 | Turn controller | Add initiative, action availability, end-turn flow, and blocked-state messaging. | Game prevents illegal actions and advances turns clearly. |
| 31 | Skirmish board alpha | Connect board, character sheets, log, and C combat into one manual skirmish loop. | A local player can finish a small skirmish manually. |
| 32 | Scenario generator model | Add C structures for objectives, deployment zones, terrain tags, and encounter templates. | Generated scenario data is deterministic by seed. |
| 33 | Scenario generator alpha | Add SwiftUI setup flow for seed, board size, objective, opposition, and difficulty. | Player can generate and start a valid skirmish. |
| 34 | AI decision model | Define AI actions, scoring inputs, threat model, and tactical personality hooks. | AI can evaluate move, shoot, reload, take cover, and wait. |
| 35 | AI opponent alpha | Add first working AI turn loop against the player. | Player can complete a generated skirmish against AI. |
| 36 | AI tactics pass | Improve target priority, range choice, movement toward objectives, and survival behavior. | AI avoids obviously invalid or self-defeating actions. |
| 37 | Board polish | Add camera controls, action affordances, animation timing, and status badges. | Board state is readable at common window sizes. |
| 38 | Character-sheet complete | Add advancement-ready skill percentages, medical/loadout details, and combat condition history. | Sheet supports both pregame and in-combat use. |
| 39 | Campaign data model | Add campaign roster, mission records, injuries, rewards, stat/skill growth, and losses. | Campaign data saves and reloads independently of skirmishes. |
| 40 | Campaign alpha | Add post-battle advancement screen with skill-use checks and wound recovery. | Winning or losing a skirmish changes campaign state. |
| 41 | Tutorial design | Script a small duel teaching selection, movement, shooting, cover, wounds, and logs. | Tutorial steps map to real engine actions. |
| 42 | Tutorial duel alpha | Add guided tutorial scenario with prompts and step gating. | New player can complete a scripted duel without external docs. |
| 43 | Dice/result log complete | Add export log, replay seed display, rule links, and campaign mission summary hooks. | Log supports debugging, learning, and post-game review. |
| 44 | Save/export complete | Add character export, campaign backup, import migration, and overwrite safeguards. | Player can move characters/campaign between installs. |
| 45 | Rule overlay integration | Link log events, action previews, and tutorial steps to relevant rule topics. | Rules appear contextually without leaving the current screen. |
| 46 | Rule overlay complete | Add bookmarks, quick reference tabs, and offline bundled content. | Player can answer common combat questions inside the app. |
| 47 | Scenario generator complete | Add terrain/objective variety, enemy loadouts, deployment balance, and seed sharing. | Generated scenarios are playable and meaningfully varied. |
| 48 | Combat rules expansion | Fill remaining high-impact mechanics: shotgun cone, grenades, healing, animal/hunter/soldier variants as scoped. | Scenario content can use implemented mechanics confidently. |
| 49 | Skirmish board content complete | Add objective tracking, win/loss conditions, cover visuals, wound/status effects, and end screen. | Generated skirmishes resolve to clear outcomes. |
| 50 | AI difficulty | Add difficulty presets and deterministic AI regression tests. | Easy/standard/hard AI differ in measurable behavior. |
| 51 | AI opponent complete | Add AI explanation hooks for log, tutorial-friendly behavior, and edge-case fallbacks. | AI can play all generated scenarios without stalls. |
| 52 | Campaign advancement complete | Add campaign mission chain, roster consequences, advancement choices, and retirement/death handling. | Campaign can run for several missions with persistent consequences. |
| 53 | UX integration pass | Smooth navigation across roster, scenario setup, skirmish, log, campaign, and rules overlay. | No core screen feels stranded or modal-trapped. |
| 54 | Accessibility and input | Add keyboard shortcuts, focus order, scalable panels, color-safe status cues, and controller/mouse basics as feasible. | Core game can be played with keyboard and mouse. |
| 55 | Tutorial duel complete | Add reset/retry, skip, final practice fight, and tutorial completion state. | Tutorial prepares player for generated skirmish. |
| 56 | Persistence hardening | Add crash-safe saves, autosave points, schema migration tests, and corrupted-file recovery. | Bad data does not destroy the roster or campaign. |
| 57 | Performance and Metal QA | Profile board rendering, large logs, scenario generation, and AI turns. | Target hardware keeps board interaction responsive. |
| 58 | Packaging prep | Add release build settings, signing/notarization checklist, bundled docs, sample content, and license credits. | A release candidate artifact can be produced locally. |
| 59 | Release candidate QA | Run full smoke pass: tutorial, create/export/import, generated skirmish, AI, campaign advancement, rules overlay, save/relaunch. | No release-blocking defects remain open. |
| 60 | Packaged playable game | Produce packaged app, tag release candidate, archive test logs, and freeze follow-up backlog. | User can install/run the app and complete the playable loop. |

## Milestones

| Milestone | Cycles | Result |
|---|---:|---|
| Foundation | 1-9 | C engine modularized, schemas versioned, logs structured. |
| App Shell | 10-18 | SwiftUI app calls C, displays characters and event logs. |
| Character and Save Loop | 19-22 | Create, edit, save, export, and import characters in-app. |
| Board Alpha | 23-31 | Manual skirmish board is playable. |
| Scenario and AI Alpha | 32-36 | Generated AI skirmish is playable. |
| Campaign and Tutorial Alpha | 37-42 | Character sheet is complete enough for advancement and tutorial exists. |
| Feature Complete | 43-52 | Logs, export, rules, scenarios, board, AI, and campaign are complete. |
| Release Candidate | 53-60 | UX, persistence, performance, packaging, QA, and packaged app are complete. |

## Cross-Cycle Quality Gates

- Every C engine change has deterministic tests using fixed seeds.
- Every user-facing rule calculation can produce a log event or explanation string.
- Every save format change includes a migration or explicit rejection path.
- Every scenario generator change includes seed-based regression coverage.
- Every Metal board change is visually checked at small, medium, and large window sizes.
- Every milestone ends with a smoke test of the playable loop available at that point.

## Release Candidate Smoke Test

Run this complete path during cycle 59 and again during cycle 60:

1. Launch packaged app.
2. Complete tutorial duel.
3. Create a new character.
4. Export the character JSON.
5. Import the exported character into a clean roster.
6. Generate a scenario from a visible seed.
7. Play a skirmish against AI to win/loss resolution.
8. Open rule reference from at least one action preview and one log event.
9. Review dice/result log and export it.
10. Apply campaign advancement after the skirmish.
11. Quit and relaunch.
12. Confirm roster, campaign state, logs, and settings persisted.

## Risks and Mitigations

| Risk | Impact | Mitigation |
|---|---|---|
| C prototype is CLI-shaped and tightly coupled. | Swift integration slows down. | Extract C modules and snapshots in cycles 5-8 before UI depends on them. |
| Metal rendering consumes too much time. | Board arrives late. | Render simple readable board first, then add effects after cycle 31 alpha. |
| Rules ambiguity blocks implementation. | Mechanics churn late. | Rule overlay and event explanations expose assumptions early. |
| AI tries to solve too much. | Unstable or slow opponent. | Start with scored tactical choices, deterministic tests, and difficulty presets. |
| Campaign scope expands. | Release slips. | Campaign is mission-to-mission advancement, not an open-ended RPG management sim. |
| Packaging/signing appears late. | App cannot ship despite being playable. | Add app shell by cycle 15 and packaging prep by cycle 58. |

