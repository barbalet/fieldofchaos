#include <ctype.h>
#include <errno.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define MAX_NAME_LEN 64
#define DEFAULT_CLIP_SIZE 10
#define DEFAULT_CLIPS 4

typedef enum {
    WEAPON_NONE = 0,
    WEAPON_SNIPER_RIFLE,
    WEAPON_RIFLE,
    WEAPON_SUBMG,
    WEAPON_SHOTGUN
} WeaponType;

typedef enum {
    MEDICAL_NONE = 0,
    MEDICAL_FIRST_AID,
    MEDICAL_PARAMEDIC,
    MEDICAL_PHARMA
} MedicalLevel;

typedef struct {
    int re;
    int ir;
    int ap;
    int ph;
    int me;
} Stats;

typedef struct {
    int head;
    int body;
    int left_arm;
    int right_arm;
    int left_leg;
    int right_leg;
} Wounds;

typedef struct {
    bool firearm_basic;
    bool firearm_advanced;
    bool firearm_sniper;
    bool evade;
    bool marching;
    bool running;
    bool close_combat;
} Skills;

typedef struct {
    WeaponType weapon;
    int clips;
    int rounds_in_clip;
    bool armor_save;
    bool in_cover;
    MedicalLevel medical;
} Loadout;

typedef struct {
    char name[MAX_NAME_LEN];
    Stats stats;
    Wounds wounds;
    Skills skills;
    Loadout loadout;
} Character;

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

static bool equals_ignore_case(const char *a, const char *b) {
    size_t i = 0;
    if (a == NULL || b == NULL) {
        return false;
    }
    while (a[i] && b[i]) {
        if (tolower((unsigned char)a[i]) != tolower((unsigned char)b[i])) {
            return false;
        }
        i++;
    }
    return a[i] == '\0' && b[i] == '\0';
}

static WeaponType parse_weapon(const char *s) {
    if (equals_ignore_case(s, "none")) {
        return WEAPON_NONE;
    }
    if (equals_ignore_case(s, "sniper") || equals_ignore_case(s, "sniper_rifle")) {
        return WEAPON_SNIPER_RIFLE;
    }
    if (equals_ignore_case(s, "rifle")) {
        return WEAPON_RIFLE;
    }
    if (equals_ignore_case(s, "submg")) {
        return WEAPON_SUBMG;
    }
    if (equals_ignore_case(s, "shotgun")) {
        return WEAPON_SHOTGUN;
    }
    return WEAPON_NONE;
}

static const char *weapon_to_string(WeaponType weapon) {
    switch (weapon) {
        case WEAPON_SNIPER_RIFLE:
            return "sniper";
        case WEAPON_RIFLE:
            return "rifle";
        case WEAPON_SUBMG:
            return "submg";
        case WEAPON_SHOTGUN:
            return "shotgun";
        case WEAPON_NONE:
        default:
            return "none";
    }
}

static MedicalLevel parse_medical(const char *s) {
    if (equals_ignore_case(s, "none")) {
        return MEDICAL_NONE;
    }
    if (equals_ignore_case(s, "first_aid") || equals_ignore_case(s, "first-aid")) {
        return MEDICAL_FIRST_AID;
    }
    if (equals_ignore_case(s, "paramedic")) {
        return MEDICAL_PARAMEDIC;
    }
    if (equals_ignore_case(s, "pharma") || equals_ignore_case(s, "pharmaceutical")) {
        return MEDICAL_PHARMA;
    }
    return MEDICAL_NONE;
}

static const char *medical_to_string(MedicalLevel level) {
    switch (level) {
        case MEDICAL_FIRST_AID:
            return "first_aid";
        case MEDICAL_PARAMEDIC:
            return "paramedic";
        case MEDICAL_PHARMA:
            return "pharma";
        case MEDICAL_NONE:
        default:
            return "none";
    }
}

static int roll_d6(void) {
    return (rand() % 6) + 1;
}

static int compare_int_desc(const void *a, const void *b) {
    const int ia = *(const int *)a;
    const int ib = *(const int *)b;
    return ib - ia;
}

static void generate_stats(Stats *stats) {
    int d[3] = {roll_d6(), roll_d6(), roll_d6()};
    int values[5];
    int index_a = 0;
    int index_b = 1;
    int i;
    int max_index = 0;

    for (i = 0; i < 3; i++) {
        if (d[i] < d[index_a]) {
            index_b = index_a;
            index_a = i;
        } else if (i != index_a && d[i] < d[index_b]) {
            index_b = i;
        }
    }

    if (index_a == index_b) {
        index_b = (index_a + 1) % 3;
    }

    values[0] = d[0];
    values[1] = d[1];
    values[2] = d[2];
    values[3] = 7 - d[index_a];
    values[4] = 7 - d[index_b];

    for (i = 1; i < 5; i++) {
        if (values[i] > values[max_index]) {
            max_index = i;
        }
    }

    for (i = 0; i < 5; i++) {
        values[i] += 1;
    }
    values[max_index] += 1;

    qsort(values, 5, sizeof(int), compare_int_desc);

    stats->ph = values[0];
    stats->me = values[1];
    stats->re = values[2];
    stats->ir = values[3];
    stats->ap = values[4];
}

static int stat_total(const Stats *s) {
    return s->re + s->ir + s->ap + s->ph + s->me;
}

static bool validate_stats(const Stats *stats, char *error, size_t error_size) {
    if (stats->re < 0 || stats->re > 10 ||
        stats->ir < 0 || stats->ir > 10 ||
        stats->ap < 0 || stats->ap > 10 ||
        stats->ph < 0 || stats->ph > 10 ||
        stats->me < 0 || stats->me > 10) {
        snprintf(error, error_size, "stats must be between 0 and 10");
        return false;
    }

    if (stat_total(stats) > 40) {
        snprintf(error, error_size, "combined stats cannot exceed 40");
        return false;
    }

    return true;
}

static void default_wounds(Wounds *w) {
    w->head = 1;
    w->body = 4;
    w->left_arm = 1;
    w->right_arm = 1;
    w->left_leg = 2;
    w->right_leg = 2;
}

static void init_default_character(Character *character, const char *name) {
    memset(character, 0, sizeof(*character));

    if (name != NULL) {
        strncpy(character->name, name, MAX_NAME_LEN - 1);
        character->name[MAX_NAME_LEN - 1] = '\0';
    } else {
        strncpy(character->name, "Unnamed", MAX_NAME_LEN - 1);
        character->name[MAX_NAME_LEN - 1] = '\0';
    }

    generate_stats(&character->stats);
    default_wounds(&character->wounds);

    character->skills.firearm_basic = true;
    character->loadout.weapon = WEAPON_RIFLE;
    character->loadout.clips = DEFAULT_CLIPS;
    character->loadout.rounds_in_clip = DEFAULT_CLIP_SIZE;
    character->loadout.medical = MEDICAL_NONE;
}

static bool parse_int_value(const char *s, int *out) {
    char *endptr;
    long value;

    if (s == NULL) {
        return false;
    }

    errno = 0;
    value = strtol(s, &endptr, 10);
    if (errno != 0 || endptr == s || *endptr != '\0') {
        return false;
    }

    *out = (int)value;
    return true;
}

static void write_json_string(FILE *file, const char *value) {
    const char *p = value;
    fputc('"', file);
    while (*p) {
        if (*p == '"' || *p == '\\') {
            fputc('\\', file);
            fputc(*p, file);
        } else if (*p == '\n') {
            fputs("\\n", file);
        } else {
            fputc(*p, file);
        }
        p++;
    }
    fputc('"', file);
}

static bool save_character_json(const char *path, const Character *character) {
    FILE *file = fopen(path, "w");
    if (file == NULL) {
        fprintf(stderr, "failed to open '%s' for writing: %s\n", path, strerror(errno));
        return false;
    }

    fprintf(file, "{\n");
    fprintf(file, "  \"name\": ");
    write_json_string(file, character->name);
    fprintf(file, ",\n");

    fprintf(file, "  \"stats\": {\n");
    fprintf(file, "    \"RE\": %d,\n", character->stats.re);
    fprintf(file, "    \"IR\": %d,\n", character->stats.ir);
    fprintf(file, "    \"AP\": %d,\n", character->stats.ap);
    fprintf(file, "    \"PH\": %d,\n", character->stats.ph);
    fprintf(file, "    \"ME\": %d\n", character->stats.me);
    fprintf(file, "  },\n");

    fprintf(file, "  \"skills\": {\n");
    fprintf(file, "    \"firearm_basic\": %s,\n", character->skills.firearm_basic ? "true" : "false");
    fprintf(file, "    \"firearm_advanced\": %s,\n", character->skills.firearm_advanced ? "true" : "false");
    fprintf(file, "    \"firearm_sniper\": %s,\n", character->skills.firearm_sniper ? "true" : "false");
    fprintf(file, "    \"evade\": %s,\n", character->skills.evade ? "true" : "false");
    fprintf(file, "    \"marching\": %s,\n", character->skills.marching ? "true" : "false");
    fprintf(file, "    \"running\": %s,\n", character->skills.running ? "true" : "false");
    fprintf(file, "    \"close_combat\": %s\n", character->skills.close_combat ? "true" : "false");
    fprintf(file, "  },\n");

    fprintf(file, "  \"loadout\": {\n");
    fprintf(file, "    \"weapon\": ");
    write_json_string(file, weapon_to_string(character->loadout.weapon));
    fprintf(file, ",\n");
    fprintf(file, "    \"clips\": %d,\n", character->loadout.clips);
    fprintf(file, "    \"rounds_in_clip\": %d,\n", character->loadout.rounds_in_clip);
    fprintf(file, "    \"armor_save\": %s,\n", character->loadout.armor_save ? "true" : "false");
    fprintf(file, "    \"in_cover\": %s,\n", character->loadout.in_cover ? "true" : "false");
    fprintf(file, "    \"medical\": ");
    write_json_string(file, medical_to_string(character->loadout.medical));
    fprintf(file, "\n");
    fprintf(file, "  },\n");

    fprintf(file, "  \"wounds\": {\n");
    fprintf(file, "    \"head\": %d,\n", character->wounds.head);
    fprintf(file, "    \"body\": %d,\n", character->wounds.body);
    fprintf(file, "    \"left_arm\": %d,\n", character->wounds.left_arm);
    fprintf(file, "    \"right_arm\": %d,\n", character->wounds.right_arm);
    fprintf(file, "    \"left_leg\": %d,\n", character->wounds.left_leg);
    fprintf(file, "    \"right_leg\": %d\n", character->wounds.right_leg);
    fprintf(file, "  }\n");

    fprintf(file, "}\n");

    if (fclose(file) != 0) {
        fprintf(stderr, "failed to close '%s': %s\n", path, strerror(errno));
        return false;
    }

    return true;
}

int main(int argc, char **argv) {
    const char *output_path;
    const char *name;
    Character character;
    unsigned int seed = (unsigned int)time(NULL);
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
            int parsed;
            if (i + 1 >= argc || !parse_int_value(argv[i + 1], &parsed)) {
                fprintf(stderr, "invalid --seed value\n");
                return 1;
            }
            seed = (unsigned int)parsed;
            i++;
        }
    }

    srand(seed);
    init_default_character(&character, name);

    for (i = 3; i < argc; i++) {
        if (strcmp(argv[i], "--seed") == 0) {
            i++;
        } else if (strcmp(argv[i], "--weapon") == 0) {
            if (i + 1 >= argc) {
                fprintf(stderr, "--weapon requires a value\n");
                return 1;
            }
            character.loadout.weapon = parse_weapon(argv[++i]);
        } else if (strcmp(argv[i], "--re") == 0) {
            if (i + 1 >= argc || !parse_int_value(argv[++i], &character.stats.re)) {
                fprintf(stderr, "invalid --re value\n");
                return 1;
            }
        } else if (strcmp(argv[i], "--ir") == 0) {
            if (i + 1 >= argc || !parse_int_value(argv[++i], &character.stats.ir)) {
                fprintf(stderr, "invalid --ir value\n");
                return 1;
            }
        } else if (strcmp(argv[i], "--ap") == 0) {
            if (i + 1 >= argc || !parse_int_value(argv[++i], &character.stats.ap)) {
                fprintf(stderr, "invalid --ap value\n");
                return 1;
            }
        } else if (strcmp(argv[i], "--ph") == 0) {
            if (i + 1 >= argc || !parse_int_value(argv[++i], &character.stats.ph)) {
                fprintf(stderr, "invalid --ph value\n");
                return 1;
            }
        } else if (strcmp(argv[i], "--me") == 0) {
            if (i + 1 >= argc || !parse_int_value(argv[++i], &character.stats.me)) {
                fprintf(stderr, "invalid --me value\n");
                return 1;
            }
        } else if (strcmp(argv[i], "--clips") == 0) {
            if (i + 1 >= argc || !parse_int_value(argv[++i], &character.loadout.clips)) {
                fprintf(stderr, "invalid --clips value\n");
                return 1;
            }
        } else if (strcmp(argv[i], "--rounds") == 0) {
            if (i + 1 >= argc || !parse_int_value(argv[++i], &character.loadout.rounds_in_clip)) {
                fprintf(stderr, "invalid --rounds value\n");
                return 1;
            }
        } else if (strcmp(argv[i], "--medical") == 0) {
            if (i + 1 >= argc) {
                fprintf(stderr, "--medical requires a value\n");
                return 1;
            }
            character.loadout.medical = parse_medical(argv[++i]);
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

    if (!validate_stats(&character.stats, error, sizeof(error))) {
        fprintf(stderr, "invalid stats: %s\n", error);
        return 1;
    }

    if (!save_character_json(output_path, &character)) {
        return 1;
    }

    printf("Created character JSON: %s\n", output_path);
    printf("Name: %s\n", character.name);
    printf("Stats (RE IR AP PH ME): %d %d %d %d %d\n",
           character.stats.re,
           character.stats.ir,
           character.stats.ap,
           character.stats.ph,
           character.stats.me);

    return 0;
}
