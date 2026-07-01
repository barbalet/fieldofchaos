# fieldofchaos

Programmatic exploration of the Field of Chaos tabletop skirmish RPG, with a focus on producing and reviewing a C-language implementation of the rules.

## Rewrite Status

This repository is being rewritten around `pdf/foc-just-the-rules-2018.pdf` as the source of truth. The Swift, Xcode, and macOS app workflow has been retired. Some historical docs and scripts still mention that workflow, but they are retained only until their planned review/cleanup cycles.

## Rules Sources

- Canonical source PDF: `pdf/foc-just-the-rules-2018.pdf`
- Reference-only newer PDF: `pdf/foc-just-the-rules-2024.pdf`
- Reference-only full newer PDF: `pdf/field-of-chaos-rpg-070524.pdf`
- Reference-only full 2018 PDF: `pdf/foc_rpg_051018.pdf`
- Extracted rules text source: `docs/field-of-chaos-rules-source.txt`
- Structured extracted rules: `docs/field-of-chaos-rules-extracted.md`
- Natural-English rules rewrite: `docs/field-of-chaos-rules-natural-english.md`

## Project Aim

- Build a C-language program version of the game rules for validation, review, and debugging.
- Keep source material traceable back to the original PDF while maintaining implementation-friendly Markdown documentation.
- Replace the prior playable SwiftUI/Metal/C app direction with a 2018-focused C static library and C program.
- Track the first ten execution cycles in `docs/cycles-01-10-execution.md`.
- Track cycles 11-20 in `docs/cycles-11-20-execution.md`.
- Track cycles 21-30 in `docs/cycles-21-30-execution.md`.
- Track cycles 31-40 in `docs/cycles-31-40-execution.md`.
- Track cycles 41-50 in `docs/cycles-41-50-execution.md`.
- Track cycles 51-60 in `docs/cycles-51-60-execution.md`.
- Track cycles 61-80 in `docs/cycles-61-80-execution.md`.
- Track cycles 81-100 in `docs/cycles-81-100-execution.md`.

## Architecture

- Reusable C engine API: `src/engine/include/fieldofchaos_engine.h`
- C engine implementation: `src/engine/fieldofchaos_engine.c`
- CLI wrappers: `src/fieldofchaos.c` and `src/character_create.c`
- Historical app/playable docs remain under `docs/` until their review cycle.

## Retired App Workflow

The prior Swift package, Xcode project, app packaging, and UI-test workflows are no longer active project workflows. The README will be rewritten near the end of the 40-cycle plan for the final C-only project shape.

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
