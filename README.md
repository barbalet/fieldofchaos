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
- `--quiet`
