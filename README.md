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

## C Utility

The project now includes a C utility in `src/`:

- Source: `src/fieldofchaos.c`
- Build file: `src/Makefile`

Build:

```bash
cd src
make
```

### Create character JSON

```bash
./fieldofchaos create ../docs/alice.json Alice --weapon rifle --firearm-basic --evade
./fieldofchaos create ../docs/bob.json Bob --weapon submg --firearm-advanced --running
```

The `create` command writes JSON files with:

- core stats: `RE`, `IR`, `AP`, `PH`, `ME`
- selected skills relevant to combat simulation
- loadout data (`weapon`, `clips`, `rounds_in_clip`, armor/cover flags)
- wound pools per body location

### Run duel simulation

```bash
./fieldofchaos duel ../docs/alice.json ../docs/bob.json --range close --seed 42
```

This runs turn-by-turn combat until one winner is declared.
