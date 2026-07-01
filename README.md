# fieldofchaos

C implementation of the Field of Chaos Gold rules subset.

## Rules Source

The source of truth is `pdf/foc-just-the-rules-gold.pdf`.

Gold keeps the 2018 core rules and adds the agreed Shotgun and Grenade rules adapted from the newer rules set. The retained PDFs below are reference-only:

- `pdf/foc-just-the-rules-2018.pdf`
- `pdf/foc-just-the-rules-2024.pdf`
- `pdf/field-of-chaos-rpg-070524.pdf`
- `pdf/foc_rpg_051018.pdf`

## Layout

- `src/include/fieldofchaosgold.h`: public C API
- `src/lib/fieldofchaosgold.c`: rules implementation
- `src/lib/fieldofchaosgold_internal.h`: internal dice helpers
- `src/cli/fieldofchaosgold.c`: command-line examples and rule tables
- `src/tests/`: focused C test programs
- `PLAN.md`: completed 40-cycle rewrite record

## Build

Run commands from `src/`:

```sh
cd src
make
make test
make clean
```

`make lib` builds `build/libfieldofchaosgold.a`. `make` builds both the static library and `build/fieldofchaosgold`.

## CLI Examples

```sh
cd src
./build/fieldofchaosgold tables
./build/fieldofchaosgold attack
./build/fieldofchaosgold blast
./build/fieldofchaosgold heal
```

The CLI prints deterministic examples for rule-table review, ranged attacks, Shotgun/Grenade blast handling, and healing.
