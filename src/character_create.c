#include "fieldofchaos_engine.h"

#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include <time.h>

static void usage(void) {
    fprintf(stderr,
            "Character JSON Creator\n"
            "Usage:\n"
            "  character_create <output.json> <name> [options]\n"
            "\n"
            "Options:\n"
            "  --seed <int>\n"
            "  --weapon <none|sniper|rifle|submg|shotgun>\n"
            "  --re <0-10> --ir <0-10> --ap <0-10> --ph <0-10> --me <0-10>\n"
            "  --clips <int> --rounds <int>\n"
            "  --firearm-basic --firearm-advanced --firearm-sniper\n"
            "  --evade --marching --running --close-combat\n"
            "  --armor-save --in-cover\n"
            "  --medical <none|first_aid|paramedic|pharma>\n");
}

static bool parse_seed_option(int *index, int argc, char **argv, unsigned int *seed_out) {
    int parsed;
    if (*index + 1 >= argc) {
        return false;
    }
    if (!foc_parse_int_value(argv[*index + 1], &parsed)) {
        return false;
    }
    *seed_out = (unsigned int)parsed;
    *index += 1;
    return true;
}

int main(int argc, char **argv) {
    const char *output_path;
    const char *name;
    FocCharacter character;
    unsigned int seed = (unsigned int)time(NULL);
    bool explicit_seed = false;
    int i;
    char error[128];

    if (argc < 3) {
        usage();
        return 1;
    }

    output_path = argv[1];
    name = argv[2];

    for (i = 3; i < argc; i++) {
        if (strcmp(argv[i], "--seed") == 0) {
            if (!parse_seed_option(&i, argc, argv, &seed)) {
                fprintf(stderr, "invalid --seed value\n");
                return 1;
            }
            explicit_seed = true;
        }
    }

    foc_seed(seed);
    foc_init_default_character(&character, name);

    for (i = 3; i < argc; i++) {
        if (strcmp(argv[i], "--seed") == 0) {
            if (!parse_seed_option(&i, argc, argv, &seed)) {
                fprintf(stderr, "invalid --seed value\n");
                return 1;
            }
        } else if (strcmp(argv[i], "--weapon") == 0) {
            if (i + 1 >= argc) {
                fprintf(stderr, "--weapon requires a value\n");
                return 1;
            }
            character.loadout.weapon = foc_parse_weapon(argv[++i]);
        } else if (strcmp(argv[i], "--re") == 0) {
            if (i + 1 >= argc || !foc_parse_int_value(argv[++i], &character.stats.re)) {
                fprintf(stderr, "invalid --re value\n");
                return 1;
            }
        } else if (strcmp(argv[i], "--ir") == 0) {
            if (i + 1 >= argc || !foc_parse_int_value(argv[++i], &character.stats.ir)) {
                fprintf(stderr, "invalid --ir value\n");
                return 1;
            }
        } else if (strcmp(argv[i], "--ap") == 0) {
            if (i + 1 >= argc || !foc_parse_int_value(argv[++i], &character.stats.ap)) {
                fprintf(stderr, "invalid --ap value\n");
                return 1;
            }
        } else if (strcmp(argv[i], "--ph") == 0) {
            if (i + 1 >= argc || !foc_parse_int_value(argv[++i], &character.stats.ph)) {
                fprintf(stderr, "invalid --ph value\n");
                return 1;
            }
        } else if (strcmp(argv[i], "--me") == 0) {
            if (i + 1 >= argc || !foc_parse_int_value(argv[++i], &character.stats.me)) {
                fprintf(stderr, "invalid --me value\n");
                return 1;
            }
        } else if (strcmp(argv[i], "--clips") == 0) {
            if (i + 1 >= argc || !foc_parse_int_value(argv[++i], &character.loadout.clips)) {
                fprintf(stderr, "invalid --clips value\n");
                return 1;
            }
        } else if (strcmp(argv[i], "--rounds") == 0) {
            if (i + 1 >= argc || !foc_parse_int_value(argv[++i], &character.loadout.rounds_in_clip)) {
                fprintf(stderr, "invalid --rounds value\n");
                return 1;
            }
        } else if (strcmp(argv[i], "--medical") == 0) {
            if (i + 1 >= argc) {
                fprintf(stderr, "--medical requires a value\n");
                return 1;
            }
            character.loadout.medical = foc_parse_medical(argv[++i]);
        } else if (strcmp(argv[i], "--firearm-basic") == 0) {
            character.skills.firearm_basic = true;
        } else if (strcmp(argv[i], "--firearm-advanced") == 0) {
            character.skills.firearm_advanced = true;
        } else if (strcmp(argv[i], "--firearm-sniper") == 0) {
            character.skills.firearm_sniper = true;
        } else if (strcmp(argv[i], "--evade") == 0) {
            character.skills.evade = true;
        } else if (strcmp(argv[i], "--marching") == 0) {
            character.skills.marching = true;
        } else if (strcmp(argv[i], "--running") == 0) {
            character.skills.running = true;
        } else if (strcmp(argv[i], "--close-combat") == 0) {
            character.skills.close_combat = true;
        } else if (strcmp(argv[i], "--armor-save") == 0) {
            character.loadout.armor_save = true;
        } else if (strcmp(argv[i], "--in-cover") == 0) {
            character.loadout.in_cover = true;
        } else {
            fprintf(stderr, "unknown option: %s\n", argv[i]);
            return 1;
        }
    }

    if (character.loadout.clips < 0 || character.loadout.rounds_in_clip < 0) {
        fprintf(stderr, "clips and rounds must be non-negative\n");
        return 1;
    }

    if (!foc_validate_stats(&character.stats, error, sizeof(error))) {
        fprintf(stderr, "invalid character stats: %s\n", error);
        return 1;
    }

    foc_refresh_character_state(&character);

    if (!foc_save_character_json(output_path, &character)) {
        return 1;
    }

    printf("created character JSON: %s\n", output_path);
    foc_print_character_summary(&character);
    if (explicit_seed) {
        printf("seed used: %u\n", seed);
    }

    return 0;
}
