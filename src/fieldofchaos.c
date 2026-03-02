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
#define DEFAULT_MAX_ROUNDS 1000

typedef enum {
    RANGE_CLOSE = 0,
    RANGE_STANDARD = 1,
    RANGE_LONG = 2
} RangeBand;

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

typedef enum {
    LOC_HEAD = 0,
    LOC_BODY,
    LOC_LEFT_ARM,
    LOC_RIGHT_ARM,
    LOC_LEFT_LEG,
    LOC_RIGHT_LEG
} HitLocation;

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
    Wounds max_wounds;
    Wounds wounds;
    Skills skills;
    Loadout loadout;
    int stunned_turns;
    bool jammed;
    bool unconscious;
    bool dead;
} Character;

typedef struct {
    RangeBand range;
    int max_rounds;
    bool verbose;
} DuelConfig;

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
            "  --quiet\n");
}

static int roll_d6(void) {
    return (rand() % 6) + 1;
}

static int roll_nd6(int n) {
    int sum = 0;
    int i;
    for (i = 0; i < n; i++) {
        sum += roll_d6();
    }
    return sum;
}

static int roll_nd6_with_one_flag(int n, bool *saw_one) {
    int sum = 0;
    int i;
    *saw_one = false;
    for (i = 0; i < n; i++) {
        int r = roll_d6();
        if (r == 1) {
            *saw_one = true;
        }
        sum += r;
    }
    return sum;
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

static RangeBand parse_range(const char *s) {
    if (equals_ignore_case(s, "close")) {
        return RANGE_CLOSE;
    }
    if (equals_ignore_case(s, "standard")) {
        return RANGE_STANDARD;
    }
    return RANGE_LONG;
}

static const char *range_to_string(RangeBand band) {
    switch (band) {
        case RANGE_CLOSE:
            return "close";
        case RANGE_STANDARD:
            return "standard";
        case RANGE_LONG:
        default:
            return "long";
    }
}

static const char *location_to_string(HitLocation location) {
    switch (location) {
        case LOC_HEAD:
            return "head";
        case LOC_BODY:
            return "body";
        case LOC_LEFT_ARM:
            return "left_arm";
        case LOC_RIGHT_ARM:
            return "right_arm";
        case LOC_LEFT_LEG:
            return "left_leg";
        case LOC_RIGHT_LEG:
            return "right_leg";
        default:
            return "unknown";
    }
}

static void default_max_wounds(Wounds *w) {
    w->head = 1;
    w->body = 4;
    w->left_arm = 1;
    w->right_arm = 1;
    w->left_leg = 2;
    w->right_leg = 2;
}

static int stat_total(const Stats *s) {
    return s->re + s->ir + s->ap + s->ph + s->me;
}

static int compare_int_desc(const void *a, const void *b) {
    const int ia = *(const int *)a;
    const int ib = *(const int *)b;
    return (ib - ia);
}

static void generate_stats(Stats *stats) {
    int d[3] = {roll_d6(), roll_d6(), roll_d6()};
    int index_a = 0;
    int index_b = 1;
    int i;

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

    {
        int values[5];
        int max_index = 0;

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
}

static bool validate_stats(const Stats *stats, char *error, size_t error_size) {
    const int min_value = 0;
    const int max_value = 10;
    int total = stat_total(stats);

    if (stats->re < min_value || stats->re > max_value ||
        stats->ir < min_value || stats->ir > max_value ||
        stats->ap < min_value || stats->ap > max_value ||
        stats->ph < min_value || stats->ph > max_value ||
        stats->me < min_value || stats->me > max_value) {
        snprintf(error, error_size, "stats must be between 0 and 10");
        return false;
    }

    if (total > 40) {
        snprintf(error, error_size, "combined stats cannot exceed 40 (current=%d)", total);
        return false;
    }

    return true;
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

    character->skills.firearm_basic = true;
    character->skills.firearm_advanced = false;
    character->skills.firearm_sniper = false;
    character->skills.evade = false;
    character->skills.marching = false;
    character->skills.running = false;
    character->skills.close_combat = false;

    character->loadout.weapon = WEAPON_RIFLE;
    character->loadout.clips = DEFAULT_CLIPS;
    character->loadout.rounds_in_clip = DEFAULT_CLIP_SIZE;
    character->loadout.armor_save = false;
    character->loadout.in_cover = false;
    character->loadout.medical = MEDICAL_NONE;

    default_max_wounds(&character->max_wounds);
    character->wounds = character->max_wounds;

    character->stunned_turns = 0;
    character->jammed = false;
    character->unconscious = false;
    character->dead = false;
}

static int character_total_wounds(const Character *character) {
    return character->wounds.head +
           character->wounds.body +
           character->wounds.left_arm +
           character->wounds.right_arm +
           character->wounds.left_leg +
           character->wounds.right_leg;
}

static void clamp_wounds(Character *character) {
    if (character->wounds.head < 0) character->wounds.head = 0;
    if (character->wounds.body < 0) character->wounds.body = 0;
    if (character->wounds.left_arm < 0) character->wounds.left_arm = 0;
    if (character->wounds.right_arm < 0) character->wounds.right_arm = 0;
    if (character->wounds.left_leg < 0) character->wounds.left_leg = 0;
    if (character->wounds.right_leg < 0) character->wounds.right_leg = 0;

    if (character->wounds.head > character->max_wounds.head) character->wounds.head = character->max_wounds.head;
    if (character->wounds.body > character->max_wounds.body) character->wounds.body = character->max_wounds.body;
    if (character->wounds.left_arm > character->max_wounds.left_arm) character->wounds.left_arm = character->max_wounds.left_arm;
    if (character->wounds.right_arm > character->max_wounds.right_arm) character->wounds.right_arm = character->max_wounds.right_arm;
    if (character->wounds.left_leg > character->max_wounds.left_leg) character->wounds.left_leg = character->max_wounds.left_leg;
    if (character->wounds.right_leg > character->max_wounds.right_leg) character->wounds.right_leg = character->max_wounds.right_leg;
}

static void refresh_character_state(Character *character) {
    clamp_wounds(character);

    if (character_total_wounds(character) <= 0) {
        character->dead = true;
        character->unconscious = true;
        return;
    }

    character->dead = false;

    if (character->wounds.head <= 0 || character->wounds.body <= 0) {
        character->unconscious = true;
    }
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

static char *read_text_file(const char *path) {
    FILE *file = fopen(path, "rb");
    char *buffer;
    long size;

    if (file == NULL) {
        fprintf(stderr, "failed to open '%s': %s\n", path, strerror(errno));
        return NULL;
    }

    if (fseek(file, 0, SEEK_END) != 0) {
        fclose(file);
        return NULL;
    }

    size = ftell(file);
    if (size < 0) {
        fclose(file);
        return NULL;
    }

    if (fseek(file, 0, SEEK_SET) != 0) {
        fclose(file);
        return NULL;
    }

    buffer = (char *)malloc((size_t)size + 1);
    if (buffer == NULL) {
        fclose(file);
        return NULL;
    }

    if (fread(buffer, 1, (size_t)size, file) != (size_t)size) {
        free(buffer);
        fclose(file);
        return NULL;
    }

    buffer[size] = '\0';
    fclose(file);
    return buffer;
}

static const char *find_json_value(const char *json, const char *key) {
    char pattern[96];
    const char *key_ptr;
    const char *colon;

    snprintf(pattern, sizeof(pattern), "\"%s\"", key);
    key_ptr = strstr(json, pattern);
    if (key_ptr == NULL) {
        return NULL;
    }

    colon = strchr(key_ptr, ':');
    if (colon == NULL) {
        return NULL;
    }

    colon++;
    while (*colon && isspace((unsigned char)*colon)) {
        colon++;
    }
    return colon;
}

static bool json_get_int(const char *json, const char *key, int *out) {
    const char *value = find_json_value(json, key);
    char *endptr;
    long parsed;

    if (value == NULL) {
        return false;
    }

    errno = 0;
    parsed = strtol(value, &endptr, 10);
    if (errno != 0 || endptr == value) {
        return false;
    }

    *out = (int)parsed;
    return true;
}

static bool json_get_bool(const char *json, const char *key, bool *out) {
    const char *value = find_json_value(json, key);
    if (value == NULL) {
        return false;
    }

    if (strncmp(value, "true", 4) == 0 || *value == '1') {
        *out = true;
        return true;
    }

    if (strncmp(value, "false", 5) == 0 || *value == '0') {
        *out = false;
        return true;
    }

    return false;
}

static bool json_get_string(const char *json, const char *key, char *out, size_t out_size) {
    const char *value = find_json_value(json, key);
    size_t i = 0;

    if (value == NULL || *value != '"') {
        return false;
    }

    value++;
    while (*value && *value != '"') {
        if (*value == '\\' && value[1]) {
            value++;
        }
        if (i + 1 < out_size) {
            out[i++] = *value;
        }
        value++;
    }

    if (*value != '"') {
        return false;
    }

    out[i] = '\0';
    return true;
}

static bool load_character_json(const char *path, Character *character) {
    char *json = read_text_file(path);
    char weapon_name[32];
    char medical_name[32];

    if (json == NULL) {
        return false;
    }

    init_default_character(character, "Loaded Character");
    default_max_wounds(&character->max_wounds);

    (void)json_get_string(json, "name", character->name, sizeof(character->name));

    if (!json_get_int(json, "RE", &character->stats.re) ||
        !json_get_int(json, "IR", &character->stats.ir) ||
        !json_get_int(json, "AP", &character->stats.ap) ||
        !json_get_int(json, "PH", &character->stats.ph) ||
        !json_get_int(json, "ME", &character->stats.me)) {
        fprintf(stderr, "invalid character file '%s': missing required stats\n", path);
        free(json);
        return false;
    }

    (void)json_get_bool(json, "firearm_basic", &character->skills.firearm_basic);
    (void)json_get_bool(json, "firearm_advanced", &character->skills.firearm_advanced);
    (void)json_get_bool(json, "firearm_sniper", &character->skills.firearm_sniper);
    (void)json_get_bool(json, "evade", &character->skills.evade);
    (void)json_get_bool(json, "marching", &character->skills.marching);
    (void)json_get_bool(json, "running", &character->skills.running);
    (void)json_get_bool(json, "close_combat", &character->skills.close_combat);

    if (json_get_string(json, "weapon", weapon_name, sizeof(weapon_name))) {
        character->loadout.weapon = parse_weapon(weapon_name);
    }
    (void)json_get_int(json, "clips", &character->loadout.clips);
    (void)json_get_int(json, "rounds_in_clip", &character->loadout.rounds_in_clip);
    (void)json_get_bool(json, "armor_save", &character->loadout.armor_save);
    (void)json_get_bool(json, "in_cover", &character->loadout.in_cover);
    if (json_get_string(json, "medical", medical_name, sizeof(medical_name))) {
        character->loadout.medical = parse_medical(medical_name);
    }

    (void)json_get_int(json, "head", &character->wounds.head);
    (void)json_get_int(json, "body", &character->wounds.body);
    (void)json_get_int(json, "left_arm", &character->wounds.left_arm);
    (void)json_get_int(json, "right_arm", &character->wounds.right_arm);
    (void)json_get_int(json, "left_leg", &character->wounds.left_leg);
    (void)json_get_int(json, "right_leg", &character->wounds.right_leg);

    free(json);

    refresh_character_state(character);
    return true;
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

static HitLocation roll_bullet_hit_location(void) {
    int roll = roll_nd6(2);
    switch (roll) {
        case 4:
            return LOC_LEFT_ARM;
        case 6:
            return LOC_LEFT_LEG;
        case 8:
            return LOC_RIGHT_LEG;
        case 10:
            return LOC_RIGHT_ARM;
        case 12:
            return LOC_HEAD;
        default:
            return LOC_BODY;
    }
}

static int get_firearm_bonus(const Character *attacker) {
    if (attacker->loadout.weapon == WEAPON_SNIPER_RIFLE) {
        if (attacker->skills.firearm_sniper) {
            return 2;
        }
        if (attacker->skills.firearm_advanced) {
            return 1;
        }
        return 0;
    }

    if (attacker->skills.firearm_advanced) {
        return 2;
    }
    if (attacker->skills.firearm_basic) {
        return 1;
    }
    return 0;
}

static int range_base_dice(RangeBand range) {
    if (range == RANGE_CLOSE) {
        return 1;
    }
    if (range == RANGE_STANDARD) {
        return 2;
    }
    return 3;
}

static int range_threshold(RangeBand range) {
    if (range == RANGE_CLOSE) {
        return 4;
    }
    if (range == RANGE_STANDARD) {
        return 8;
    }
    return 14;
}

static int shotgun_damage_for_range(RangeBand range) {
    if (range == RANGE_CLOSE) {
        return roll_nd6(2);
    }
    if (range == RANGE_STANDARD) {
        return roll_d6();
    }
    return (roll_d6() + 1) / 2;
}

static int shots_per_turn(const Character *character) {
    switch (character->loadout.weapon) {
        case WEAPON_SNIPER_RIFLE:
            return 1;
        case WEAPON_RIFLE:
            return character->skills.firearm_advanced ? 2 : 1;
        case WEAPON_SUBMG:
            return 4;
        case WEAPON_SHOTGUN:
            return 1;
        case WEAPON_NONE:
        default:
            return 0;
    }
}

static bool can_make_melee_attack(const Character *character) {
    if (character->dead || character->unconscious) {
        return false;
    }
    if (character->wounds.left_arm <= 0 && character->wounds.right_arm <= 0) {
        return false;
    }
    return true;
}

static bool can_use_firearm(const Character *character, RangeBand range) {
    if (character->dead || character->unconscious) {
        return false;
    }
    if (character->wounds.left_arm <= 0 || character->wounds.right_arm <= 0) {
        return false;
    }
    if (character->loadout.weapon == WEAPON_NONE) {
        return false;
    }
    if (character->loadout.weapon == WEAPON_SUBMG && range == RANGE_LONG) {
        return false;
    }
    return true;
}

static int *wound_pool_for_location(Character *character, HitLocation location) {
    switch (location) {
        case LOC_HEAD:
            return &character->wounds.head;
        case LOC_BODY:
            return &character->wounds.body;
        case LOC_LEFT_ARM:
            return &character->wounds.left_arm;
        case LOC_RIGHT_ARM:
            return &character->wounds.right_arm;
        case LOC_LEFT_LEG:
            return &character->wounds.left_leg;
        case LOC_RIGHT_LEG:
            return &character->wounds.right_leg;
        default:
            return NULL;
    }
}

static bool apply_single_wound(Character *target, HitLocation location, bool verbose) {
    bool armor_saved = false;
    int *pool;

    if (target->loadout.armor_save) {
        int armor_roll = roll_d6();
        if (armor_roll >= 5) {
            armor_saved = true;
            if (verbose) {
                printf("    %s armor save succeeds (%d).\n", target->name, armor_roll);
            }
        }
    }

    if (armor_saved) {
        return false;
    }

    pool = wound_pool_for_location(target, location);
    if (pool == NULL || *pool <= 0) {
        return false;
    }

    (*pool)--;
    refresh_character_state(target);
    return true;
}

static void apply_chest_wind_check(Character *target, bool verbose) {
    int roll;
    if (target->dead) {
        return;
    }

    roll = roll_nd6(2);
    if (roll > target->stats.ph) {
        int turns = roll - target->stats.ph;
        if (turns > target->stunned_turns) {
            target->stunned_turns = turns;
        }
        if (verbose) {
            printf("    %s is winded for %d turn(s).\n", target->name, turns);
        }
    }
}

static void apply_grave_condition(Character *character, bool verbose) {
    int remaining;

    if (character->dead || character->unconscious) {
        return;
    }

    remaining = character_total_wounds(character);
    if (remaining < 6) {
        int roll = roll_nd6(2);
        if (roll > character->stats.ph) {
            character->unconscious = true;
            if (verbose) {
                printf("  %s falls unconscious from grave condition (2d6=%d > PH=%d).\n",
                       character->name,
                       roll,
                       character->stats.ph);
            }
        }
    }
}

static void perform_melee_attack(Character *attacker, Character *target, bool verbose) {
    int bonus;
    int total;
    bool inflicted;

    if (!can_make_melee_attack(attacker)) {
        if (verbose) {
            printf("  %s cannot make a melee attack this turn.\n", attacker->name);
        }
        return;
    }

    bonus = attacker->skills.close_combat ? 1 : 0;
    total = roll_d6() + bonus;

    if (total < 2) {
        if (verbose) {
            printf("  %s misses melee attack (roll=%d).\n", attacker->name, total);
        }
        return;
    }

    inflicted = apply_single_wound(target, LOC_BODY, verbose);
    if (verbose) {
        if (inflicted) {
            printf("  %s lands melee hit on %s body.\n", attacker->name, target->name);
        } else {
            printf("  %s lands melee hit but causes no wound.\n", attacker->name);
        }
    }

    if (inflicted) {
        apply_chest_wind_check(target, verbose);
    }
}

static void perform_ranged_shot(Character *attacker, Character *target, const DuelConfig *config, int shot_index) {
    int base_dice;
    int threshold;
    int penalty = 0;
    int dice_count;
    int bonus;
    int sum;
    int total;
    bool saw_one = false;
    bool hit;

    if (target->loadout.in_cover) {
        penalty++;
    }
    if (target->skills.evade) {
        penalty++;
    }
    if (target->skills.running) {
        penalty++;
    }

    base_dice = range_base_dice(config->range);
    threshold = range_threshold(config->range);
    dice_count = base_dice - penalty;
    bonus = get_firearm_bonus(attacker);

    if (dice_count < 1) {
        if (config->verbose) {
            printf("  %s shot %d auto-misses (all dice canceled by modifiers).\n",
                   attacker->name,
                   shot_index);
        }
        return;
    }

    sum = roll_nd6_with_one_flag(dice_count, &saw_one);
    total = sum + bonus;
    hit = (total >= threshold);

    if (attacker->loadout.weapon == WEAPON_SUBMG && saw_one) {
        attacker->jammed = true;
    }

    if (config->verbose) {
        printf("  %s shot %d roll=%d bonus=%d total=%d target=%d -> %s\n",
               attacker->name,
               shot_index,
               sum,
               bonus,
               total,
               threshold,
               hit ? "HIT" : "MISS");
        if (attacker->loadout.weapon == WEAPON_SUBMG && saw_one) {
            printf("    %s weapon jams on rolled 1.\n", attacker->name);
        }
    }

    if (!hit) {
        return;
    }

    if (attacker->loadout.weapon == WEAPON_SHOTGUN) {
        int wounds = shotgun_damage_for_range(config->range);
        int i;
        if (config->verbose) {
            printf("    Shotgun blast inflicts %d wound attempt(s).\n", wounds);
        }
        for (i = 0; i < wounds; i++) {
            HitLocation loc = roll_bullet_hit_location();
            bool inflicted = apply_single_wound(target, loc, config->verbose);
            if (config->verbose) {
                printf("    -> %s %s\n",
                       location_to_string(loc),
                       inflicted ? "wounded" : "no effect");
            }
            if (inflicted && loc == LOC_BODY) {
                apply_chest_wind_check(target, config->verbose);
            }
            if (target->dead) {
                break;
            }
        }
    } else {
        HitLocation loc = roll_bullet_hit_location();
        bool inflicted = apply_single_wound(target, loc, config->verbose);
        if (config->verbose) {
            printf("    Hit location: %s -> %s\n",
                   location_to_string(loc),
                   inflicted ? "wound" : "no effect");
        }
        if (inflicted && loc == LOC_BODY) {
            apply_chest_wind_check(target, config->verbose);
        }
    }
}

static void take_turn(Character *attacker, Character *target, const DuelConfig *config) {
    int shot_count;
    int shot;

    if (attacker->dead) {
        return;
    }

    if (attacker->unconscious) {
        if (config->verbose) {
            printf("  %s is unconscious and cannot act.\n", attacker->name);
        }
        return;
    }

    if (attacker->stunned_turns > 0) {
        if (config->verbose) {
            printf("  %s is winded and skips turn (%d turn(s) left).\n",
                   attacker->name,
                   attacker->stunned_turns);
        }
        attacker->stunned_turns--;
        return;
    }

    if (!can_use_firearm(attacker, config->range)) {
        perform_melee_attack(attacker, target, config->verbose);
        return;
    }

    if (attacker->jammed) {
        attacker->jammed = false;
        if (config->verbose) {
            printf("  %s spends turn clearing weapon jam.\n", attacker->name);
        }
        return;
    }

    if (attacker->loadout.rounds_in_clip <= 0) {
        if (attacker->loadout.clips > 0) {
            attacker->loadout.clips--;
            attacker->loadout.rounds_in_clip = DEFAULT_CLIP_SIZE;
            if (config->verbose) {
                printf("  %s reloads (clips remaining: %d).\n",
                       attacker->name,
                       attacker->loadout.clips);
            }
            return;
        }

        if (config->verbose) {
            printf("  %s is out of ammunition and switches to melee.\n", attacker->name);
        }
        perform_melee_attack(attacker, target, config->verbose);
        return;
    }

    shot_count = shots_per_turn(attacker);
    if (shot_count <= 0) {
        perform_melee_attack(attacker, target, config->verbose);
        return;
    }

    for (shot = 1; shot <= shot_count; shot++) {
        if (target->dead) {
            return;
        }

        if (attacker->loadout.rounds_in_clip <= 0) {
            if (attacker->loadout.clips > 0) {
                attacker->loadout.clips--;
                attacker->loadout.rounds_in_clip = DEFAULT_CLIP_SIZE;
                if (config->verbose) {
                    printf("  %s runs dry and reloads mid-turn.\n", attacker->name);
                }
                return;
            }

            if (config->verbose) {
                printf("  %s runs out of ammunition.\n", attacker->name);
            }
            return;
        }

        attacker->loadout.rounds_in_clip--;
        perform_ranged_shot(attacker, target, config, shot);

        if (attacker->jammed) {
            return;
        }
    }
}

static int tie_breaker_score(const Character *character) {
    return character_total_wounds(character) * 10 + character->stats.ph + character->stats.me;
}

static void print_character_summary(const Character *character) {
    printf("%s: RE=%d IR=%d AP=%d PH=%d ME=%d, weapon=%s, wounds=%d\n",
           character->name,
           character->stats.re,
           character->stats.ir,
           character->stats.ap,
           character->stats.ph,
           character->stats.me,
           weapon_to_string(character->loadout.weapon),
           character_total_wounds(character));
}

static int run_duel(Character *a, Character *b, const DuelConfig *config) {
    int round;

    for (round = 1; round <= config->max_rounds; round++) {
        int initiative_a;
        int initiative_b;
        Character *first = a;
        Character *second = b;

        if (a->dead || b->dead) {
            break;
        }

        if (config->verbose) {
            printf("\n=== ROUND %d (%s range) ===\n", round, range_to_string(config->range));
        }

        apply_grave_condition(a, config->verbose);
        apply_grave_condition(b, config->verbose);

        initiative_a = roll_nd6(2) + a->stats.me;
        initiative_b = roll_nd6(2) + b->stats.me;

        if (initiative_b > initiative_a) {
            first = b;
            second = a;
        }

        if (config->verbose) {
            printf("Initiative: %s=%d, %s=%d -> %s acts first\n",
                   a->name,
                   initiative_a,
                   b->name,
                   initiative_b,
                   first->name);
        }

        take_turn(first, second, config);
        if (!second->dead) {
            take_turn(second, first, config);
        }

        if (config->verbose) {
            print_character_summary(a);
            print_character_summary(b);
        }

        if (a->dead || b->dead) {
            break;
        }
    }

    if (a->dead && !b->dead) {
        return 2;
    }
    if (b->dead && !a->dead) {
        return 1;
    }

    {
        int score_a = tie_breaker_score(a);
        int score_b = tie_breaker_score(b);
        if (score_a > score_b) {
            return 1;
        }
        if (score_b > score_a) {
            return 2;
        }
        return (roll_d6() <= 3) ? 1 : 2;
    }
}

static bool parse_seed_option(int *index, int argc, char **argv, unsigned int *seed_out) {
    int parsed;
    if (*index + 1 >= argc) {
        return false;
    }
    if (!parse_int_value(argv[*index + 1], &parsed)) {
        return false;
    }
    *seed_out = (unsigned int)parsed;
    *index += 1;
    return true;
}

static int command_create(int argc, char **argv) {
    const char *output_path;
    const char *name;
    Character character;
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

    srand(seed);
    init_default_character(&character, name);

    for (i = 4; i < argc; i++) {
        if (strcmp(argv[i], "--seed") == 0) {
            if (!parse_seed_option(&i, argc, argv, &seed)) {
                fprintf(stderr, "invalid --seed value\n");
                return 1;
            }
            explicit_seed = true;
            srand(seed);
            init_default_character(&character, name);
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
            fprintf(stderr, "unknown option for create: %s\n", argv[i]);
            return 1;
        }
    }

    if (character.loadout.clips < 0 || character.loadout.rounds_in_clip < 0) {
        fprintf(stderr, "clips and rounds must be non-negative\n");
        return 1;
    }

    if (!validate_stats(&character.stats, error, sizeof(error))) {
        fprintf(stderr, "invalid character stats: %s\n", error);
        return 1;
    }

    refresh_character_state(&character);

    if (!save_character_json(output_path, &character)) {
        return 1;
    }

    printf("created character JSON: %s\n", output_path);
    print_character_summary(&character);
    if (explicit_seed) {
        printf("seed used: %u\n", seed);
    }

    return 0;
}

static int command_duel(int argc, char **argv) {
    const char *path_a;
    const char *path_b;
    Character a;
    Character b;
    DuelConfig config;
    unsigned int seed = (unsigned int)time(NULL);
    int i;
    int winner;

    if (argc < 4) {
        usage();
        return 1;
    }

    path_a = argv[2];
    path_b = argv[3];

    config.range = RANGE_CLOSE;
    config.max_rounds = DEFAULT_MAX_ROUNDS;
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
            config.range = parse_range(argv[++i]);
        } else if (strcmp(argv[i], "--max-rounds") == 0) {
            if (i + 1 >= argc || !parse_int_value(argv[++i], &config.max_rounds) || config.max_rounds <= 0) {
                fprintf(stderr, "invalid --max-rounds value\n");
                return 1;
            }
        } else if (strcmp(argv[i], "--quiet") == 0) {
            config.verbose = false;
        } else {
            fprintf(stderr, "unknown option for duel: %s\n", argv[i]);
            return 1;
        }
    }

    srand(seed);

    if (!load_character_json(path_a, &a)) {
        return 1;
    }
    if (!load_character_json(path_b, &b)) {
        return 1;
    }

    printf("duel seed: %u\n", seed);
    print_character_summary(&a);
    print_character_summary(&b);

    winner = run_duel(&a, &b, &config);

    printf("\n=== RESULT ===\n");
    printf("%s final wounds: %d\n", a.name, character_total_wounds(&a));
    printf("%s final wounds: %d\n", b.name, character_total_wounds(&b));

    if (winner == 1) {
        printf("Winner: %s\n", a.name);
    } else {
        printf("Winner: %s\n", b.name);
    }

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
