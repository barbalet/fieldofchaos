# fieldofchaos

C implementation of the Field of Chaos Gold rules.

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
- `TRACEABILITY.md`: Gold PDF to C/API/test parity matrix

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
./build/fieldofchaosgold stats
./build/fieldofchaosgold skills
./build/fieldofchaosgold weapons
./build/fieldofchaosgold shotgun
./build/fieldofchaosgold grenade
./build/fieldofchaosgold movement
./build/fieldofchaosgold wounds
./build/fieldofchaosgold locations
./build/fieldofchaosgold melee
./build/fieldofchaosgold healing
./build/fieldofchaosgold foes
./build/fieldofchaosgold tables
./build/fieldofchaosgold attack
./build/fieldofchaosgold blast
./build/fieldofchaosgold heal
./build/fieldofchaosgold armor
./build/fieldofchaosgold all
```

The CLI prints Gold rule tables for stats, skill hierarchy and accounting, weapons, Shotgun, Grenade, movement, wounds, hit locations, melee, healing, and foes. It also prints deterministic examples for ranged attacks, Shotgun area resolution, Grenade area resolution, melee, healing application, and Soldier armor.
