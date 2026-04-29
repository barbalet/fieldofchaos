# fieldofchaos

Programmatic exploration of the Field of Chaos tabletop skirmish RPG, with a focus on producing and reviewing a C-language implementation of the rules.

## Rules Sources

- Canonical source PDF: `pdf/field-of-chaos-rpg-070524.pdf`
- Extracted rules text source: `docs/field-of-chaos-rules-source.txt`
- Structured extracted rules: `docs/field-of-chaos-rules-extracted.md`
- Natural-English rules rewrite: `docs/field-of-chaos-rules-natural-english.md`

## Project Aim

- Build a C-language program version of the game rules for validation, review, and debugging.
- Keep source material traceable back to the original PDF while maintaining implementation-friendly Markdown documentation.
- Plan the path from the current C rules prototype to a packaged SwiftUI/Metal/C playable game: `docs/playable-game-60-cycle-plan.md`.
- Track the first ten execution cycles in `docs/cycles-01-10-execution.md`.
- Track cycles 11-20 in `docs/cycles-11-20-execution.md`.
- Track cycles 21-30 in `docs/cycles-21-30-execution.md`.
- Track cycles 31-40 in `docs/cycles-31-40-execution.md`.
- Track cycles 41-50 in `docs/cycles-41-50-execution.md`.
- Track cycles 51-60 in `docs/cycles-51-60-execution.md`.

## Architecture

- Reusable C engine API: `src/engine/include/fieldofchaos_engine.h`
- C engine implementation: `src/engine/fieldofchaos_engine.c`
- CLI wrappers: `src/fieldofchaos.c` and `src/character_create.c`
- SwiftUI app shell: `AppShell/FieldOfChaosAppShell.swift`
- Swift app state and views: `AppShell/GameStore.swift`, `AppShell/AppViews.swift`, and `AppShell/CInterop.swift`
- Skirmish board model and Metal renderer: `AppShell/SkirmishModel.swift`, `AppShell/SkirmishViews.swift`, and `AppShell/BoardRenderer.swift`
- Rule reference UI: `AppShell/RuleReference.swift` and `AppShell/RuleViews.swift`
- Campaign model and UI: `AppShell/CampaignModel.swift` and `AppShell/CampaignViews.swift`
- Tutorial flow: `AppShell/TutorialModel.swift`
- Swift Package manifest: `Package.swift`
- Mac Xcode project: `FieldOfChaos.xcodeproj`

## Xcode

Open the Mac project directly:

```bash
open FieldOfChaos.xcodeproj
```

The project has these schemes/targets:

- `FieldOfChaos`: SwiftUI + Metal macOS app, with the C engine compiled into the app target.
- `fieldofchaos`: C command-line utility for create/duel workflows.
- `character_create`: C character JSON utility.
- `test_engine`: C engine test executable.

The app target bundles `docs/alice.json`, `docs/bob.json`, and the natural-English rule reference so it can run from Xcode without relying on the repository working directory.

CLI verification from Terminal:

```bash
xcodebuild -project FieldOfChaos.xcodeproj -scheme FieldOfChaos -configuration Debug build
xcodebuild -project FieldOfChaos.xcodeproj -scheme fieldofchaos -configuration Debug build
xcodebuild -project FieldOfChaos.xcodeproj -scheme character_create -configuration Debug build
xcodebuild -project FieldOfChaos.xcodeproj -scheme test_engine -configuration Debug build
```

## C Utilities

The project includes two executables in `src/`:

- `fieldofchaos` from `src/fieldofchaos.c`
- `character_create` from `src/character_create.c`

Build both programs:

```bash
cd src
make
```

Or build one program at a time:

```bash
cd src
make fieldofchaos
make character_create
```

### Character creation with `character_create`

Run from `src/`:

```bash
./character_create ../docs/alice.json Alice --weapon rifle --firearm-basic --evade --seed 42
./character_create ../docs/bob.json Bob --weapon submg --firearm-advanced --running --seed 99
```

Usage:

```bash
./character_create <output.json> <name> [options]
```

### Character creation with `fieldofchaos create`

Run from `src/`:

```bash
./fieldofchaos create ../docs/alice.json Alice --weapon rifle --firearm-basic --evade --seed 42
./fieldofchaos create ../docs/bob.json Bob --weapon submg --firearm-advanced --running --seed 99
```

Usage:

```bash
./fieldofchaos create <output.json> <name> [options]
```

### Run duel simulation with `fieldofchaos duel`

After creating two character JSON files, run:

```bash
./fieldofchaos duel ../docs/alice.json ../docs/bob.json --range close --seed 42
```

Usage:

```bash
./fieldofchaos duel <character_a.json> <character_b.json> [options]
```

Common duel options:

- `--range close|standard|long`
- `--seed <int>`
- `--max-rounds <int>`
- `--event-log <path>` writes JSON-lines structured roll/result events
- `--quiet`

Run deterministic C engine tests:

```bash
cd src
make test
```

Build the SwiftUI shell and C engine package:

```bash
swift build
```

Run the SwiftUI shell from the repository root:

```bash
swift run FieldOfChaosAppShell
```

Create a local release candidate app bundle:

```bash
sh scripts/package_release.sh
```
