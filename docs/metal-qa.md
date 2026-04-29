# Metal QA

Cycle 57 checks keep the board renderer simple and reliable:

- The board has a stable aspect ratio from `columns / rows`.
- `BoardRenderState` carries move, target, selected, objective, cover, and actor cells.
- `BoardRenderer` skips drawing when no Metal device or drawable is available, so SwiftUI can still host the app.
- The renderer consumes state prepared by `SkirmishState`; combat math remains in the C engine.
- Accessibility labels on the tap layer expose board coordinates even though the visible board is Metal.

Manual visual smoke path:

1. Run `swift run FieldOfChaosAppShell`.
2. Open Skirmish.
3. Generate a scenario and confirm objective, cover, actors, movement highlights, and targeting highlights are visible.
4. Move, attack, run AI, and confirm the board keeps its aspect ratio.
5. Open Tutorial and confirm the same board remains interactive inside the split view.
