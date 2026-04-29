# Cycles 61-80 Execution

This log records the sixth and seventh ten-cycle blocks after the initial playable release candidate.

| Cycle | Result |
| --- | --- |
| 61 | Added a playable Mac acceptance checklist for the remaining completion work. |
| 62 | Hardened skirmish selection, dead/unconscious turn handling, and no-active-actor draw resolution. |
| 63 | Added active actor display and action availability hints with disabled reasons. |
| 64 | Added draw outcomes and objective-based timeout resolution for center and breakthrough scenarios. |
| 65 | Added a dedicated Scenario screen for roster choice, seed, difficulty, max turns, AI, generate, campaign mission, and load. |
| 66 | Improved AI movement fallback to prefer cover on standard/hard difficulty while preserving easy hesitancy. |
| 67 | Added action previews for move, attack, reload, clear, recover, and wait availability. |
| 68 | Added dice log summary counts and latest-result context above the event table. |
| 69 | Added Codable skirmish save/load snapshots and autosave for active skirmishes. |
| 70 | Ran C and Swift build QA after core play changes. |
| 71 | Documented campaign structure, mission flow, rewards, injuries, recovery, backup, and autosave. |
| 72 | Added persistent campaign autosave load/save under Application Support. |
| 73 | Added campaign roster records with missions, XP earned, injuries, recovery, and status. |
| 74 | Added injury consequences and recovery ticks after each mission. |
| 75 | Added advancement choices: PH, ME, Firearm Advanced, Evade, and Running. |
| 76 | Added mission rewards/consequences to mission history and campaign roster. |
| 77 | Added campaign-driven next mission generation from current record progress. |
| 78 | Expanded campaign summary/history UI with roster state and mission metadata. |
| 79 | Added campaign backup import alongside export. |
| 80 | Ran C tests, Swift build, and Xcode app build verification. |

## Verification

- `cd src && make test`
- `swift build`
- `xcodebuild -project FieldOfChaos.xcodeproj -scheme FieldOfChaos -configuration Debug build`

## Notes

The next block, cycles 81-100, should focus on Mac game feel: app icon, visual polish, animations, sound, preferences, keyboard polish, accessibility pass, onboarding, automated smoke scripts, release configuration, signing/notarization prep, and final release-candidate playthrough.
