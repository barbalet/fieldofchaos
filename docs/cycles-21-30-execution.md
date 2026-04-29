# Cycles 21-30 Execution Notes

This log records the third ten cycles from `docs/playable-game-60-cycle-plan.md` as implemented work.

## Cycle Results

| Cycle | Result |
|---:|---|
| 21 | Added import/export services for character JSON, local roster storage, and save/export commands. |
| 22 | Added import conflict handling with replace or duplicate choices. |
| 23 | Added skirmish board data model with grid cells, terrain-ready coordinates, actor positions, movement scale, and turn actors. |
| 24 | Added Metal board renderer through `MTKView`, bundled shader source, fallback runtime shader compilation, and stable responsive board sizing. |
| 25 | Added board selection through a transparent cell interaction layer, selected actor/cell highlighting, and target cell highlighting. |
| 26 | Added movement previews using C movement rules for slow, standard, and fast pace, including leg-wound movement reduction. |
| 27 | Added targeting previews using C range-band and hit-calculation helpers. |
| 28 | Added board actions for move, wait, reload, clear jam, and ranged attack. Reload, jam clearing, and ranged attack route through C action helpers. |
| 29 | Added rule reference overlay and dedicated rules screen with indexed Markdown topics and search. |
| 30 | Added turn controller with initiative, current actor gating, acted-state tracking, end-turn advancement, and blocked-action messages. |

## App Surface At Cycle 30

- `Roster`: create, save, import, export, and conflict handling.
- `Character`: editable character sheet from cycles 11-20.
- `Skirmish`: Metal-rendered tactical board, cell selection, movement preview, targeting preview, turn order, and manual two-character actions.
- `Dice Log`: generated duel logs plus board-action events appended from skirmish actions.
- `Rules`: searchable rule reference loaded from `docs/field-of-chaos-rules-natural-english.md`.
- `Settings`: runtime and local storage status.

## New C APIs

- `foc_weapon_range_yards`
- `foc_range_for_distance`
- `foc_character_movement_yards`
- `foc_roll_initiative`
- `foc_make_targeting_preview`
- `foc_board_reload`
- `foc_board_clear_jam`
- `foc_board_ranged_attack`

## Build And Test Expectations

- `make test` from `src/` covers movement, weapon ranges, targeting preview, initiative, and board action helpers.
- `swift build` from the repository root builds the SwiftUI app shell, Metal renderer resources, and C engine target.

## Cycle 30 Boundary

The board is now a manual two-character skirmish surface, not yet the full cycle-31 board alpha. The next cycle should connect board, character sheets, dice log, and C combat more tightly into a finishable manual skirmish loop with win/loss resolution.

