#include "fieldofchaos_engine.h"

#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include <time.h>

static void usage(void) {
    fprintf(stderr,
            "Field of Chaos C Utility\n"
            "Usage:\n"
            "  fieldofchaos create <output.json> <name> [options]\n"
            "  fieldofchaos duel <character_a.json> <character_b.json> [options]\n"
            "\n"
            "Create options:\n"
            "  --seed <int>\n"
            "  --weapon <none|sniper|rifle|submg|shotgun>\n"
            "  --re <0-10> --ir <0-10> --ap <0-10> --ph <0-10> --me <0-10>\n"
            "  --clips <int> --rounds <int>\n"
            "  --firearm-basic --firearm-advanced --firearm-sniper\n"
            "  --evade --marching --running --close-combat\n"
            "  --armor-save --in-cover\n"
            "  --medical <none|first_aid|paramedic|pharma>\n"
            "\n"
            "Duel options:\n"
            "  --seed <int>\n"
            "  --range <close|standard|long>\n"
            "  --max-rounds <int>\n"
            "  --event-log <path>   Write JSON-lines structured events\n"
            "  --quiet\n");
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

static void event_log_sink(const FocEvent *event, void *context) {
    FILE *file = (FILE *)context;
    foc_write_event_jsonl(file, event);
}

static int command_create(int argc, char **argv) {
    const char *output_path;
    const char *name;
    FocCharacter character;
    unsigned int seed = (unsigned int)time(NULL);
    bool explicit_seed = false;
    int i;
    char error[128];

    if (argc < 4) {
        usage();
        return 1;
    }

    output_path = argv[2];
    name = argv[3];

    for (i = 4; i < argc; i++) {
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

    for (i = 4; i < argc; i++) {
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
            fprintf(stderr, "unknown option for create: %s\n", argv[i]);
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

static int command_duel(int argc, char **argv) {
    const char *path_a;
    const char *path_b;
    const char *event_log_path = NULL;
    FocCharacter a;
    FocCharacter b;
    FocDuelConfig config;
    unsigned int seed = (unsigned int)time(NULL);
    int i;
    int winner;
    char error[160];
    FILE *event_log = NULL;

    if (argc < 4) {
        usage();
        return 1;
    }

    path_a = argv[2];
    path_b = argv[3];

    config.range = FOC_RANGE_CLOSE;
    config.max_rounds = FOC_DEFAULT_MAX_ROUNDS;
    config.verbose = true;

    for (i = 4; i < argc; i++) {
        if (strcmp(argv[i], "--seed") == 0) {
            if (!parse_seed_option(&i, argc, argv, &seed)) {
                fprintf(stderr, "invalid --seed value\n");
                return 1;
            }
        } else if (strcmp(argv[i], "--range") == 0) {
            if (i + 1 >= argc) {
                fprintf(stderr, "--range requires a value\n");
                return 1;
            }
            config.range = foc_parse_range(argv[++i]);
        } else if (strcmp(argv[i], "--max-rounds") == 0) {
            if (i + 1 >= argc || !foc_parse_int_value(argv[++i], &config.max_rounds) || config.max_rounds <= 0) {
                fprintf(stderr, "invalid --max-rounds value\n");
                return 1;
            }
        } else if (strcmp(argv[i], "--event-log") == 0) {
            if (i + 1 >= argc) {
                fprintf(stderr, "--event-log requires a value\n");
                return 1;
            }
            event_log_path = argv[++i];
        } else if (strcmp(argv[i], "--quiet") == 0) {
            config.verbose = false;
        } else {
            fprintf(stderr, "unknown option for duel: %s\n", argv[i]);
            return 1;
        }
    }

    foc_seed(seed);

    if (!foc_load_character_json(path_a, &a, error, sizeof(error))) {
        fprintf(stderr, "invalid character file '%s': %s\n", path_a, error);
        return 1;
    }
    if (!foc_load_character_json(path_b, &b, error, sizeof(error))) {
        fprintf(stderr, "invalid character file '%s': %s\n", path_b, error);
        return 1;
    }

    if (event_log_path != NULL) {
        event_log = fopen(event_log_path, "w");
        if (event_log == NULL) {
            fprintf(stderr, "failed to open event log: %s\n", event_log_path);
            return 1;
        }
    }

    printf("duel seed: %u\n", seed);
    foc_print_character_summary(&a);
    foc_print_character_summary(&b);

    winner = foc_run_duel(&a, &b, &config, event_log == NULL ? NULL : event_log_sink, event_log);

    if (event_log != NULL) {
        fclose(event_log);
        printf("event log: %s\n", event_log_path);
    }

    printf("\n=== RESULT ===\n");
    printf("%s final wounds: %d\n", a.name, foc_character_total_wounds(&a));
    printf("%s final wounds: %d\n", b.name, foc_character_total_wounds(&b));
    printf("Winner: %s\n", winner == 1 ? a.name : b.name);

    return 0;
}

int main(int argc, char **argv) {
    if (argc < 2) {
        usage();
        return 1;
    }

    if (strcmp(argv[1], "create") == 0) {
        return command_create(argc, argv);
    }

    if (strcmp(argv[1], "duel") == 0) {
        return command_duel(argc, argv);
    }

    usage();
    return 1;
}
