#include "fieldofchaos_engine.h"

#include <ctype.h>
#include <errno.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define FOC_ENGINE_VERSION "0.1.0-cycle10"

typedef struct {
    FocEventSink sink;
    void *context;
    int next_event_id;
    unsigned int seed;
} EventEmitter;

static unsigned int g_current_seed = 0;

const char *foc_engine_version(void) {
    return FOC_ENGINE_VERSION;
}

void foc_seed(unsigned int seed) {
    g_current_seed = seed;
    srand(seed);
}

unsigned int foc_current_seed(void) {
    return g_current_seed;
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
        int roll = roll_d6();
        if (roll == 1) {
            *saw_one = true;
        }
        sum += roll;
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

static void copy_name(char *out, const char *value) {
    if (value == NULL) {
        out[0] = '\0';
        return;
    }
    strncpy(out, value, FOC_MAX_NAME_LEN - 1);
    out[FOC_MAX_NAME_LEN - 1] = '\0';
}

bool foc_parse_int_value(const char *s, int *out) {
    char *endptr;
    long value;

    if (s == NULL || out == NULL) {
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

FocWeaponType foc_parse_weapon(const char *s) {
    if (equals_ignore_case(s, "none")) {
        return FOC_WEAPON_NONE;
    }
    if (equals_ignore_case(s, "sniper") || equals_ignore_case(s, "sniper_rifle")) {
        return FOC_WEAPON_SNIPER_RIFLE;
    }
    if (equals_ignore_case(s, "rifle")) {
        return FOC_WEAPON_RIFLE;
    }
    if (equals_ignore_case(s, "submg")) {
        return FOC_WEAPON_SUBMG;
    }
    if (equals_ignore_case(s, "shotgun")) {
        return FOC_WEAPON_SHOTGUN;
    }
    return FOC_WEAPON_NONE;
}

const char *foc_weapon_to_string(FocWeaponType weapon) {
    switch (weapon) {
        case FOC_WEAPON_SNIPER_RIFLE:
            return "sniper";
        case FOC_WEAPON_RIFLE:
            return "rifle";
        case FOC_WEAPON_SUBMG:
            return "submg";
        case FOC_WEAPON_SHOTGUN:
            return "shotgun";
        case FOC_WEAPON_NONE:
        default:
            return "none";
    }
}

FocMedicalLevel foc_parse_medical(const char *s) {
    if (equals_ignore_case(s, "none")) {
        return FOC_MEDICAL_NONE;
    }
    if (equals_ignore_case(s, "first_aid") || equals_ignore_case(s, "first-aid")) {
        return FOC_MEDICAL_FIRST_AID;
    }
    if (equals_ignore_case(s, "paramedic")) {
        return FOC_MEDICAL_PARAMEDIC;
    }
    if (equals_ignore_case(s, "pharma") || equals_ignore_case(s, "pharmaceutical")) {
        return FOC_MEDICAL_PHARMA;
    }
    return FOC_MEDICAL_NONE;
}

const char *foc_medical_to_string(FocMedicalLevel level) {
    switch (level) {
        case FOC_MEDICAL_FIRST_AID:
            return "first_aid";
        case FOC_MEDICAL_PARAMEDIC:
            return "paramedic";
        case FOC_MEDICAL_PHARMA:
            return "pharma";
        case FOC_MEDICAL_NONE:
        default:
            return "none";
    }
}

FocRangeBand foc_parse_range(const char *s) {
    if (equals_ignore_case(s, "close")) {
        return FOC_RANGE_CLOSE;
    }
    if (equals_ignore_case(s, "standard")) {
        return FOC_RANGE_STANDARD;
    }
    return FOC_RANGE_LONG;
}

const char *foc_range_to_string(FocRangeBand band) {
    switch (band) {
        case FOC_RANGE_CLOSE:
            return "close";
        case FOC_RANGE_STANDARD:
            return "standard";
        case FOC_RANGE_LONG:
        default:
            return "long";
    }
}

const char *foc_location_to_string(FocHitLocation location) {
    switch (location) {
        case FOC_LOC_HEAD:
            return "head";
        case FOC_LOC_BODY:
            return "body";
        case FOC_LOC_LEFT_ARM:
            return "left_arm";
        case FOC_LOC_RIGHT_ARM:
            return "right_arm";
        case FOC_LOC_LEFT_LEG:
            return "left_leg";
        case FOC_LOC_RIGHT_LEG:
            return "right_leg";
        default:
            return "unknown";
    }
}

const char *foc_event_type_to_string(FocEventType type) {
    switch (type) {
        case FOC_EVENT_ROUND_START:
            return "round_start";
        case FOC_EVENT_ROLL:
            return "roll";
        case FOC_EVENT_ATTACK:
            return "attack";
        case FOC_EVENT_WOUND:
            return "wound";
        case FOC_EVENT_AMMO:
            return "ammo";
        case FOC_EVENT_STATUS:
            return "status";
        case FOC_EVENT_RESULT:
            return "result";
        default:
            return "unknown";
    }
}

const char *foc_movement_pace_to_string(FocMovementPace pace) {
    switch (pace) {
        case FOC_PACE_SLOW:
            return "slow";
        case FOC_PACE_STANDARD:
            return "standard";
        case FOC_PACE_FAST:
            return "fast";
        default:
            return "unknown";
    }
}

const char *foc_objective_to_string(FocObjectiveType objective) {
    switch (objective) {
        case FOC_OBJECTIVE_ELIMINATION:
            return "elimination";
        case FOC_OBJECTIVE_HOLD_CENTER:
            return "hold_center";
        case FOC_OBJECTIVE_BREAKTHROUGH:
            return "breakthrough";
        default:
            return "unknown";
    }
}

const char *foc_ai_action_to_string(FocAIActionType action) {
    switch (action) {
        case FOC_AI_WAIT:
            return "wait";
        case FOC_AI_MOVE:
            return "move";
        case FOC_AI_ATTACK:
            return "attack";
        case FOC_AI_RELOAD:
            return "reload";
        case FOC_AI_CLEAR_JAM:
            return "clear_jam";
        default:
            return "unknown";
    }
}

void foc_default_max_wounds(FocWounds *wounds) {
    wounds->head = 1;
    wounds->body = 4;
    wounds->left_arm = 1;
    wounds->right_arm = 1;
    wounds->left_leg = 2;
    wounds->right_leg = 2;
}

static int compare_int_desc(const void *a, const void *b) {
    const int ia = *(const int *)a;
    const int ib = *(const int *)b;
    return ib - ia;
}

void foc_generate_stats(FocStats *stats) {
    int d[3] = {roll_d6(), roll_d6(), roll_d6()};
    int values[5];
    int index_a = 0;
    int index_b = 1;
    int max_index = 0;
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

int foc_stat_total(const FocStats *stats) {
    return stats->re + stats->ir + stats->ap + stats->ph + stats->me;
}

bool foc_validate_stats(const FocStats *stats, char *error, size_t error_size) {
    int total;
    if (stats == NULL) {
        snprintf(error, error_size, "stats are missing");
        return false;
    }

    if (stats->re < 0 || stats->re > 10 ||
        stats->ir < 0 || stats->ir > 10 ||
        stats->ap < 0 || stats->ap > 10 ||
        stats->ph < 0 || stats->ph > 10 ||
        stats->me < 0 || stats->me > 10) {
        snprintf(error, error_size, "stats must be between 0 and 10");
        return false;
    }

    total = foc_stat_total(stats);
    if (total > 40) {
        snprintf(error, error_size, "combined stats cannot exceed 40 (current=%d)", total);
        return false;
    }

    return true;
}

void foc_init_default_character(FocCharacter *character, const char *name) {
    memset(character, 0, sizeof(*character));

    character->schema_version = FOC_CHARACTER_SCHEMA_VERSION;
    if (name != NULL) {
        strncpy(character->name, name, FOC_MAX_NAME_LEN - 1);
        character->name[FOC_MAX_NAME_LEN - 1] = '\0';
    } else {
        strncpy(character->name, "Unnamed", FOC_MAX_NAME_LEN - 1);
        character->name[FOC_MAX_NAME_LEN - 1] = '\0';
    }

    foc_generate_stats(&character->stats);

    character->skills.firearm_basic = true;
    character->loadout.weapon = FOC_WEAPON_RIFLE;
    character->loadout.clips = FOC_DEFAULT_CLIPS;
    character->loadout.rounds_in_clip = FOC_DEFAULT_CLIP_SIZE;
    character->loadout.medical = FOC_MEDICAL_NONE;

    foc_default_max_wounds(&character->max_wounds);
    character->wounds = character->max_wounds;
}

int foc_character_total_wounds(const FocCharacter *character) {
    return character->wounds.head +
           character->wounds.body +
           character->wounds.left_arm +
           character->wounds.right_arm +
           character->wounds.left_leg +
           character->wounds.right_leg;
}

static void clamp_wounds(FocCharacter *character) {
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

void foc_refresh_character_state(FocCharacter *character) {
    clamp_wounds(character);

    if (foc_character_total_wounds(character) <= 0) {
        character->dead = true;
        character->unconscious = true;
        return;
    }

    character->dead = false;

    if (character->wounds.head <= 0 || character->wounds.body <= 0) {
        character->unconscious = true;
    } else {
        character->unconscious = false;
    }
}

void foc_make_character_snapshot(const FocCharacter *character, FocCharacterSnapshot *snapshot) {
    memset(snapshot, 0, sizeof(*snapshot));
    if (character == NULL) {
        return;
    }

    copy_name(snapshot->name, character->name);
    snapshot->schema_version = character->schema_version;
    snapshot->stats = character->stats;
    snapshot->wounds = character->wounds;
    snapshot->stat_total = foc_stat_total(&character->stats);
    snapshot->wound_total = foc_character_total_wounds(character);
    snapshot->weapon = character->loadout.weapon;
    snapshot->medical = character->loadout.medical;
    snapshot->clips = character->loadout.clips;
    snapshot->rounds_in_clip = character->loadout.rounds_in_clip;
    snapshot->initiative_modifier = character->stats.me;
    snapshot->firearm_basic = character->skills.firearm_basic;
    snapshot->firearm_advanced = character->skills.firearm_advanced;
    snapshot->firearm_sniper = character->skills.firearm_sniper;
    snapshot->evade = character->skills.evade;
    snapshot->marching = character->skills.marching;
    snapshot->running = character->skills.running;
    snapshot->close_combat = character->skills.close_combat;
    snapshot->armor_save = character->loadout.armor_save;
    snapshot->in_cover = character->loadout.in_cover;
    snapshot->jammed = character->jammed;
    snapshot->unconscious = character->unconscious;
    snapshot->dead = character->dead;
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

bool foc_save_character_json(const char *path, const FocCharacter *character) {
    FILE *file = fopen(path, "w");
    if (file == NULL) {
        fprintf(stderr, "failed to open '%s' for writing: %s\n", path, strerror(errno));
        return false;
    }

    fprintf(file, "{\n");
    fprintf(file, "  \"schema_version\": %d,\n", FOC_CHARACTER_SCHEMA_VERSION);
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
    write_json_string(file, foc_weapon_to_string(character->loadout.weapon));
    fprintf(file, ",\n");
    fprintf(file, "    \"clips\": %d,\n", character->loadout.clips);
    fprintf(file, "    \"rounds_in_clip\": %d,\n", character->loadout.rounds_in_clip);
    fprintf(file, "    \"armor_save\": %s,\n", character->loadout.armor_save ? "true" : "false");
    fprintf(file, "    \"in_cover\": %s,\n", character->loadout.in_cover ? "true" : "false");
    fprintf(file, "    \"medical\": ");
    write_json_string(file, foc_medical_to_string(character->loadout.medical));
    fprintf(file, "\n");
    fprintf(file, "  },\n");

    fprintf(file, "  \"wounds\": {\n");
    fprintf(file, "    \"head\": %d,\n", character->wounds.head);
    fprintf(file, "    \"body\": %d,\n", character->wounds.body);
    fprintf(file, "    \"left_arm\": %d,\n", character->wounds.left_arm);
    fprintf(file, "    \"right_arm\": %d,\n", character->wounds.right_arm);
    fprintf(file, "    \"left_leg\": %d,\n", character->wounds.left_leg);
    fprintf(file, "    \"right_leg\": %d\n", character->wounds.right_leg);
    fprintf(file, "  },\n");

    fprintf(file, "  \"state\": {\n");
    fprintf(file, "    \"stunned_turns\": %d,\n", character->stunned_turns);
    fprintf(file, "    \"jammed\": %s,\n", character->jammed ? "true" : "false");
    fprintf(file, "    \"unconscious\": %s,\n", character->unconscious ? "true" : "false");
    fprintf(file, "    \"dead\": %s\n", character->dead ? "true" : "false");
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

static void set_error(char *error, size_t error_size, const char *message) {
    if (error != NULL && error_size > 0) {
        snprintf(error, error_size, "%s", message);
    }
}

bool foc_load_character_json(const char *path, FocCharacter *character, char *error, size_t error_size) {
    char *json = read_text_file(path);
    char weapon_name[32];
    char medical_name[32];
    char stats_error[128];
    int schema_version = 0;

    if (json == NULL) {
        if (error != NULL && error_size > 0) {
            snprintf(error, error_size, "failed to open '%s': %s", path, strerror(errno));
        }
        return false;
    }

    foc_init_default_character(character, "Loaded Character");
    character->schema_version = 0;

    if (json_get_int(json, "schema_version", &schema_version)) {
        if (schema_version > FOC_CHARACTER_SCHEMA_VERSION) {
            free(json);
            set_error(error, error_size, "unsupported future character schema version");
            return false;
        }
        character->schema_version = schema_version;
    }

    (void)json_get_string(json, "name", character->name, sizeof(character->name));

    if (!json_get_int(json, "RE", &character->stats.re) ||
        !json_get_int(json, "IR", &character->stats.ir) ||
        !json_get_int(json, "AP", &character->stats.ap) ||
        !json_get_int(json, "PH", &character->stats.ph) ||
        !json_get_int(json, "ME", &character->stats.me)) {
        free(json);
        set_error(error, error_size, "missing required stats RE, IR, AP, PH, or ME");
        return false;
    }

    if (!foc_validate_stats(&character->stats, stats_error, sizeof(stats_error))) {
        free(json);
        if (error != NULL && error_size > 0) {
            snprintf(error, error_size, "invalid stats: %s", stats_error);
        }
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
        character->loadout.weapon = foc_parse_weapon(weapon_name);
    }
    (void)json_get_int(json, "clips", &character->loadout.clips);
    (void)json_get_int(json, "rounds_in_clip", &character->loadout.rounds_in_clip);
    (void)json_get_bool(json, "armor_save", &character->loadout.armor_save);
    (void)json_get_bool(json, "in_cover", &character->loadout.in_cover);
    if (json_get_string(json, "medical", medical_name, sizeof(medical_name))) {
        character->loadout.medical = foc_parse_medical(medical_name);
    }

    (void)json_get_int(json, "head", &character->wounds.head);
    (void)json_get_int(json, "body", &character->wounds.body);
    (void)json_get_int(json, "left_arm", &character->wounds.left_arm);
    (void)json_get_int(json, "right_arm", &character->wounds.right_arm);
    (void)json_get_int(json, "left_leg", &character->wounds.left_leg);
    (void)json_get_int(json, "right_leg", &character->wounds.right_leg);
    (void)json_get_int(json, "stunned_turns", &character->stunned_turns);
    (void)json_get_bool(json, "jammed", &character->jammed);
    (void)json_get_bool(json, "unconscious", &character->unconscious);
    (void)json_get_bool(json, "dead", &character->dead);

    free(json);

    if (character->loadout.clips < 0 || character->loadout.rounds_in_clip < 0) {
        set_error(error, error_size, "clips and rounds_in_clip must be non-negative");
        return false;
    }

    foc_refresh_character_state(character);
    return true;
}

void foc_print_character_summary(const FocCharacter *character) {
    printf("%s: RE=%d IR=%d AP=%d PH=%d ME=%d, weapon=%s, wounds=%d\n",
           character->name,
           character->stats.re,
           character->stats.ir,
           character->stats.ap,
           character->stats.ph,
           character->stats.me,
           foc_weapon_to_string(character->loadout.weapon),
           foc_character_total_wounds(character));
}

int foc_weapon_range_yards(FocWeaponType weapon, FocRangeBand band) {
    switch (weapon) {
        case FOC_WEAPON_SNIPER_RIFLE:
            if (band == FOC_RANGE_CLOSE) return 288;
            if (band == FOC_RANGE_STANDARD) return 432;
            return 576;
        case FOC_WEAPON_RIFLE:
            if (band == FOC_RANGE_CLOSE) return 216;
            if (band == FOC_RANGE_STANDARD) return 324;
            return 432;
        case FOC_WEAPON_SUBMG:
            if (band == FOC_RANGE_CLOSE) return 96;
            if (band == FOC_RANGE_STANDARD) return 192;
            return 0;
        case FOC_WEAPON_SHOTGUN:
            if (band == FOC_RANGE_CLOSE) return 10;
            if (band == FOC_RANGE_STANDARD) return 20;
            return 30;
        case FOC_WEAPON_NONE:
        default:
            return 0;
    }
}

bool foc_range_for_distance(FocWeaponType weapon, int distance_yards, FocRangeBand *range_out) {
    if (distance_yards <= foc_weapon_range_yards(weapon, FOC_RANGE_CLOSE)) {
        if (range_out != NULL) {
            *range_out = FOC_RANGE_CLOSE;
        }
        return foc_weapon_range_yards(weapon, FOC_RANGE_CLOSE) > 0;
    }
    if (distance_yards <= foc_weapon_range_yards(weapon, FOC_RANGE_STANDARD)) {
        if (range_out != NULL) {
            *range_out = FOC_RANGE_STANDARD;
        }
        return foc_weapon_range_yards(weapon, FOC_RANGE_STANDARD) > 0;
    }
    if (distance_yards <= foc_weapon_range_yards(weapon, FOC_RANGE_LONG)) {
        if (range_out != NULL) {
            *range_out = FOC_RANGE_LONG;
        }
        return foc_weapon_range_yards(weapon, FOC_RANGE_LONG) > 0;
    }
    if (range_out != NULL) {
        *range_out = FOC_RANGE_LONG;
    }
    return false;
}

int foc_character_movement_yards(const FocCharacter *character, FocMovementPace pace) {
    int yards;

    switch (pace) {
        case FOC_PACE_SLOW:
            yards = 2 + (character->skills.evade ? 1 : 0);
            break;
        case FOC_PACE_STANDARD:
            yards = 4 + (character->skills.marching ? 2 : 0);
            break;
        case FOC_PACE_FAST:
            yards = 7 + (character->skills.running ? 3 : 0);
            break;
        default:
            yards = 0;
            break;
    }

    if (character->dead || character->unconscious) {
        return 0;
    }
    if (character->wounds.left_leg <= 0 && character->wounds.right_leg <= 0) {
        return 0;
    }
    if (character->wounds.left_leg <= 0 || character->wounds.right_leg <= 0) {
        yards = yards / 2;
    }
    return yards < 0 ? 0 : yards;
}

int foc_roll_initiative(const FocCharacter *character) {
    if (character == NULL) {
        return 0;
    }
    return roll_nd6(2) + character->stats.me;
}

static void emit_event(EventEmitter *emitter,
                       FocEventType type,
                       int round,
                       int actor_id,
                       const FocCharacter *actor,
                       int target_id,
                       const FocCharacter *target,
                       const char *summary,
                       int roll,
                       int bonus,
                       int total,
                       int threshold,
                       FocHitLocation location,
                       int amount) {
    FocEvent event;

    if (emitter == NULL || emitter->sink == NULL) {
        return;
    }

    memset(&event, 0, sizeof(event));
    event.event_id = emitter->next_event_id++;
    event.seed = emitter->seed;
    event.round = round;
    event.type = type;
    event.actor_id = actor_id;
    event.target_id = target_id;
    event.roll = roll;
    event.bonus = bonus;
    event.total = total;
    event.threshold = threshold;
    event.location = location;
    event.amount = amount;
    copy_name(event.actor, actor == NULL ? "" : actor->name);
    copy_name(event.target, target == NULL ? "" : target->name);
    if (summary != NULL) {
        strncpy(event.summary, summary, FOC_EVENT_SUMMARY_LEN - 1);
        event.summary[FOC_EVENT_SUMMARY_LEN - 1] = '\0';
    }

    emitter->sink(&event, emitter->context);
}

static FocHitLocation roll_bullet_hit_location(void) {
    int roll = roll_nd6(2);
    switch (roll) {
        case 4:
            return FOC_LOC_LEFT_ARM;
        case 6:
            return FOC_LOC_LEFT_LEG;
        case 8:
            return FOC_LOC_RIGHT_LEG;
        case 10:
            return FOC_LOC_RIGHT_ARM;
        case 12:
            return FOC_LOC_HEAD;
        default:
            return FOC_LOC_BODY;
    }
}

static int get_firearm_bonus(const FocCharacter *attacker) {
    if (attacker->loadout.weapon == FOC_WEAPON_SNIPER_RIFLE) {
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

static int range_base_dice(FocRangeBand range) {
    if (range == FOC_RANGE_CLOSE) {
        return 1;
    }
    if (range == FOC_RANGE_STANDARD) {
        return 2;
    }
    return 3;
}

static int range_threshold(FocRangeBand range) {
    if (range == FOC_RANGE_CLOSE) {
        return 4;
    }
    if (range == FOC_RANGE_STANDARD) {
        return 8;
    }
    return 14;
}

static int shotgun_damage_for_range(FocRangeBand range) {
    if (range == FOC_RANGE_CLOSE) {
        return roll_nd6(2);
    }
    if (range == FOC_RANGE_STANDARD) {
        return roll_d6();
    }
    return (roll_d6() + 1) / 2;
}

static int shots_per_turn(const FocCharacter *character) {
    switch (character->loadout.weapon) {
        case FOC_WEAPON_SNIPER_RIFLE:
            return 1;
        case FOC_WEAPON_RIFLE:
            return character->skills.firearm_advanced ? 2 : 1;
        case FOC_WEAPON_SUBMG:
            return 4;
        case FOC_WEAPON_SHOTGUN:
            return 1;
        case FOC_WEAPON_NONE:
        default:
            return 0;
    }
}

static bool can_make_melee_attack(const FocCharacter *character) {
    if (character->dead || character->unconscious) {
        return false;
    }
    if (character->wounds.left_arm <= 0 && character->wounds.right_arm <= 0) {
        return false;
    }
    return true;
}

static bool can_use_firearm(const FocCharacter *character, FocRangeBand range) {
    if (character->dead || character->unconscious) {
        return false;
    }
    if (character->wounds.left_arm <= 0 || character->wounds.right_arm <= 0) {
        return false;
    }
    if (character->loadout.weapon == FOC_WEAPON_NONE) {
        return false;
    }
    if (character->loadout.weapon == FOC_WEAPON_SUBMG && range == FOC_RANGE_LONG) {
        return false;
    }
    return true;
}

void foc_make_targeting_preview(const FocCharacter *attacker,
                                const FocCharacter *target,
                                int distance_yards,
                                FocTargetingPreview *preview) {
    bool in_range;
    FocRangeBand range;
    int base_dice;
    int penalty = 0;
    int bonus;

    memset(preview, 0, sizeof(*preview));
    preview->distance_yards = distance_yards;
    preview->max_range_yards = foc_weapon_range_yards(attacker->loadout.weapon, FOC_RANGE_LONG);

    in_range = foc_range_for_distance(attacker->loadout.weapon, distance_yards, &range);
    preview->in_range = in_range;
    preview->range = range;

    if (target->loadout.in_cover) {
        penalty++;
    }
    if (target->skills.evade) {
        penalty++;
    }
    if (target->skills.running) {
        penalty++;
    }

    base_dice = range_base_dice(range);
    bonus = get_firearm_bonus(attacker);
    preview->base_dice = base_dice;
    preview->penalties = penalty;
    preview->dice_count = base_dice - penalty;
    preview->bonus = bonus;
    preview->threshold = range_threshold(range);

    preview->can_attack = in_range &&
                          can_use_firearm(attacker, range) &&
                          preview->dice_count > 0 &&
                          !attacker->jammed &&
                          attacker->loadout.rounds_in_clip > 0;

    if (!in_range) {
        snprintf(preview->explanation, sizeof(preview->explanation), "Target is out of range at %d yards.", distance_yards);
    } else if (!can_use_firearm(attacker, range)) {
        snprintf(preview->explanation, sizeof(preview->explanation), "Attacker cannot use a firearm at %s range.", foc_range_to_string(range));
    } else if (attacker->jammed) {
        snprintf(preview->explanation, sizeof(preview->explanation), "Weapon is jammed and must be cleared.");
    } else if (attacker->loadout.rounds_in_clip <= 0) {
        snprintf(preview->explanation, sizeof(preview->explanation), "Weapon is empty and must be reloaded.");
    } else if (preview->dice_count <= 0) {
        snprintf(preview->explanation, sizeof(preview->explanation), "Cover/movement modifiers cancel all dice.");
    } else {
        snprintf(preview->explanation,
                 sizeof(preview->explanation),
                 "%s range: %dd6 - %d penalty + %d bonus vs %d.",
                 foc_range_to_string(range),
                 base_dice,
                 penalty,
                 bonus,
                 preview->threshold);
    }
}

static int *wound_pool_for_location(FocCharacter *character, FocHitLocation location) {
    switch (location) {
        case FOC_LOC_HEAD:
            return &character->wounds.head;
        case FOC_LOC_BODY:
            return &character->wounds.body;
        case FOC_LOC_LEFT_ARM:
            return &character->wounds.left_arm;
        case FOC_LOC_RIGHT_ARM:
            return &character->wounds.right_arm;
        case FOC_LOC_LEFT_LEG:
            return &character->wounds.left_leg;
        case FOC_LOC_RIGHT_LEG:
            return &character->wounds.right_leg;
        default:
            return NULL;
    }
}

static bool apply_single_wound(FocCharacter *target, FocHitLocation location, bool verbose) {
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
    foc_refresh_character_state(target);
    return true;
}

static void apply_chest_wind_check(FocCharacter *target,
                                   bool verbose,
                                   EventEmitter *emitter,
                                   int round,
                                   int target_id) {
    int roll;
    char summary[FOC_EVENT_SUMMARY_LEN];

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
        snprintf(summary, sizeof(summary), "%s is winded for %d turn(s)", target->name, turns);
        emit_event(emitter, FOC_EVENT_STATUS, round, target_id, target, 0, NULL, summary, roll, 0, roll, target->stats.ph, FOC_LOC_BODY, turns);
    }
}

static void apply_grave_condition(FocCharacter *character,
                                  bool verbose,
                                  EventEmitter *emitter,
                                  int round,
                                  int actor_id) {
    int remaining;

    if (character->dead || character->unconscious) {
        return;
    }

    remaining = foc_character_total_wounds(character);
    if (remaining < 6) {
        int roll = roll_nd6(2);
        char summary[FOC_EVENT_SUMMARY_LEN];
        snprintf(summary, sizeof(summary), "%s grave condition check %d vs PH %d", character->name, roll, character->stats.ph);
        emit_event(emitter, FOC_EVENT_ROLL, round, actor_id, character, 0, NULL, summary, roll, 0, roll, character->stats.ph, FOC_LOC_BODY, 0);
        if (roll > character->stats.ph) {
            character->unconscious = true;
            if (verbose) {
                printf("  %s falls unconscious from grave condition (2d6=%d > PH=%d).\n",
                       character->name,
                       roll,
                       character->stats.ph);
            }
            snprintf(summary, sizeof(summary), "%s falls unconscious from grave condition", character->name);
            emit_event(emitter, FOC_EVENT_STATUS, round, actor_id, character, 0, NULL, summary, roll, 0, roll, character->stats.ph, FOC_LOC_BODY, 0);
        }
    }
}

static void perform_melee_attack(FocCharacter *attacker,
                                 int attacker_id,
                                 FocCharacter *target,
                                 int target_id,
                                 bool verbose,
                                 EventEmitter *emitter,
                                 int round) {
    int bonus;
    int total;
    bool inflicted;
    char summary[FOC_EVENT_SUMMARY_LEN];

    if (!can_make_melee_attack(attacker)) {
        if (verbose) {
            printf("  %s cannot make a melee attack this turn.\n", attacker->name);
        }
        snprintf(summary, sizeof(summary), "%s cannot make a melee attack", attacker->name);
        emit_event(emitter, FOC_EVENT_STATUS, round, attacker_id, attacker, target_id, target, summary, 0, 0, 0, 0, FOC_LOC_BODY, 0);
        return;
    }

    bonus = attacker->skills.close_combat ? 1 : 0;
    total = roll_d6() + bonus;
    snprintf(summary, sizeof(summary), "%s melee roll total %d", attacker->name, total);
    emit_event(emitter, FOC_EVENT_ATTACK, round, attacker_id, attacker, target_id, target, summary, total - bonus, bonus, total, 2, FOC_LOC_BODY, 0);

    if (total < 2) {
        if (verbose) {
            printf("  %s misses melee attack (roll=%d).\n", attacker->name, total);
        }
        return;
    }

    inflicted = apply_single_wound(target, FOC_LOC_BODY, verbose);
    if (verbose) {
        if (inflicted) {
            printf("  %s lands melee hit on %s body.\n", attacker->name, target->name);
        } else {
            printf("  %s lands melee hit but causes no wound.\n", attacker->name);
        }
    }

    snprintf(summary, sizeof(summary), "%s melee hit on %s body: %s", attacker->name, target->name, inflicted ? "wound" : "no effect");
    emit_event(emitter, FOC_EVENT_WOUND, round, attacker_id, attacker, target_id, target, summary, 0, 0, 0, 0, FOC_LOC_BODY, inflicted ? 1 : 0);

    if (inflicted) {
        apply_chest_wind_check(target, verbose, emitter, round, target_id);
    }
}

static void perform_ranged_shot(FocCharacter *attacker,
                                int attacker_id,
                                FocCharacter *target,
                                int target_id,
                                const FocDuelConfig *config,
                                int shot_index,
                                EventEmitter *emitter,
                                int round) {
    int base_dice;
    int threshold;
    int penalty = 0;
    int dice_count;
    int bonus;
    int sum;
    int total;
    bool saw_one = false;
    bool hit;
    char summary[FOC_EVENT_SUMMARY_LEN];

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
        snprintf(summary, sizeof(summary), "%s shot %d auto-misses after modifiers", attacker->name, shot_index);
        emit_event(emitter, FOC_EVENT_ATTACK, round, attacker_id, attacker, target_id, target, summary, 0, bonus, bonus, threshold, FOC_LOC_BODY, 0);
        return;
    }

    sum = roll_nd6_with_one_flag(dice_count, &saw_one);
    total = sum + bonus;
    hit = total >= threshold;

    if (attacker->loadout.weapon == FOC_WEAPON_SUBMG && saw_one) {
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
        if (attacker->loadout.weapon == FOC_WEAPON_SUBMG && saw_one) {
            printf("    %s weapon jams on rolled 1.\n", attacker->name);
        }
    }

    snprintf(summary,
             sizeof(summary),
             "%s shot %d at %s: total %d vs %d -> %s",
             attacker->name,
             shot_index,
             target->name,
             total,
             threshold,
             hit ? "hit" : "miss");
    emit_event(emitter, FOC_EVENT_ATTACK, round, attacker_id, attacker, target_id, target, summary, sum, bonus, total, threshold, FOC_LOC_BODY, 0);

    if (attacker->loadout.weapon == FOC_WEAPON_SUBMG && saw_one) {
        snprintf(summary, sizeof(summary), "%s weapon jams on rolled 1", attacker->name);
        emit_event(emitter, FOC_EVENT_STATUS, round, attacker_id, attacker, target_id, target, summary, sum, 0, sum, 0, FOC_LOC_BODY, 0);
    }

    if (!hit) {
        return;
    }

    if (attacker->loadout.weapon == FOC_WEAPON_SHOTGUN) {
        int wounds = shotgun_damage_for_range(config->range);
        int i;
        if (config->verbose) {
            printf("    Shotgun blast inflicts %d wound attempt(s).\n", wounds);
        }
        for (i = 0; i < wounds; i++) {
            FocHitLocation loc = roll_bullet_hit_location();
            bool inflicted = apply_single_wound(target, loc, config->verbose);
            if (config->verbose) {
                printf("    -> %s %s\n",
                       foc_location_to_string(loc),
                       inflicted ? "wounded" : "no effect");
            }
            snprintf(summary,
                     sizeof(summary),
                     "%s shotgun wound attempt on %s %s: %s",
                     attacker->name,
                     target->name,
                     foc_location_to_string(loc),
                     inflicted ? "wound" : "no effect");
            emit_event(emitter, FOC_EVENT_WOUND, round, attacker_id, attacker, target_id, target, summary, 0, 0, 0, 0, loc, inflicted ? 1 : 0);
            if (inflicted && loc == FOC_LOC_BODY) {
                apply_chest_wind_check(target, config->verbose, emitter, round, target_id);
            }
            if (target->dead) {
                break;
            }
        }
    } else {
        FocHitLocation loc = roll_bullet_hit_location();
        bool inflicted = apply_single_wound(target, loc, config->verbose);
        if (config->verbose) {
            printf("    Hit location: %s -> %s\n",
                   foc_location_to_string(loc),
                   inflicted ? "wound" : "no effect");
        }
        snprintf(summary,
                 sizeof(summary),
                 "%s hit %s %s: %s",
                 attacker->name,
                 target->name,
                 foc_location_to_string(loc),
                 inflicted ? "wound" : "no effect");
        emit_event(emitter, FOC_EVENT_WOUND, round, attacker_id, attacker, target_id, target, summary, 0, 0, 0, 0, loc, inflicted ? 1 : 0);
        if (inflicted && loc == FOC_LOC_BODY) {
            apply_chest_wind_check(target, config->verbose, emitter, round, target_id);
        }
    }
}

static void take_turn(FocCharacter *attacker,
                      int attacker_id,
                      FocCharacter *target,
                      int target_id,
                      const FocDuelConfig *config,
                      EventEmitter *emitter,
                      int round) {
    int shot_count;
    int shot;
    char summary[FOC_EVENT_SUMMARY_LEN];

    if (attacker->dead) {
        return;
    }

    if (attacker->unconscious) {
        if (config->verbose) {
            printf("  %s is unconscious and cannot act.\n", attacker->name);
        }
        snprintf(summary, sizeof(summary), "%s is unconscious and cannot act", attacker->name);
        emit_event(emitter, FOC_EVENT_STATUS, round, attacker_id, attacker, target_id, target, summary, 0, 0, 0, 0, FOC_LOC_BODY, 0);
        return;
    }

    if (attacker->stunned_turns > 0) {
        if (config->verbose) {
            printf("  %s is winded and skips turn (%d turn(s) left).\n",
                   attacker->name,
                   attacker->stunned_turns);
        }
        snprintf(summary, sizeof(summary), "%s is winded and skips turn", attacker->name);
        emit_event(emitter, FOC_EVENT_STATUS, round, attacker_id, attacker, target_id, target, summary, 0, 0, 0, 0, FOC_LOC_BODY, attacker->stunned_turns);
        attacker->stunned_turns--;
        return;
    }

    if (!can_use_firearm(attacker, config->range)) {
        perform_melee_attack(attacker, attacker_id, target, target_id, config->verbose, emitter, round);
        return;
    }

    if (attacker->jammed) {
        attacker->jammed = false;
        if (config->verbose) {
            printf("  %s spends turn clearing weapon jam.\n", attacker->name);
        }
        snprintf(summary, sizeof(summary), "%s spends turn clearing weapon jam", attacker->name);
        emit_event(emitter, FOC_EVENT_STATUS, round, attacker_id, attacker, target_id, target, summary, 0, 0, 0, 0, FOC_LOC_BODY, 0);
        return;
    }

    if (attacker->loadout.rounds_in_clip <= 0) {
        if (attacker->loadout.clips > 0) {
            attacker->loadout.clips--;
            attacker->loadout.rounds_in_clip = FOC_DEFAULT_CLIP_SIZE;
            if (config->verbose) {
                printf("  %s reloads (clips remaining: %d).\n",
                       attacker->name,
                       attacker->loadout.clips);
            }
            snprintf(summary, sizeof(summary), "%s reloads", attacker->name);
            emit_event(emitter, FOC_EVENT_AMMO, round, attacker_id, attacker, target_id, target, summary, 0, 0, 0, 0, FOC_LOC_BODY, attacker->loadout.clips);
            return;
        }

        if (config->verbose) {
            printf("  %s is out of ammunition and switches to melee.\n", attacker->name);
        }
        snprintf(summary, sizeof(summary), "%s is out of ammunition", attacker->name);
        emit_event(emitter, FOC_EVENT_AMMO, round, attacker_id, attacker, target_id, target, summary, 0, 0, 0, 0, FOC_LOC_BODY, 0);
        perform_melee_attack(attacker, attacker_id, target, target_id, config->verbose, emitter, round);
        return;
    }

    shot_count = shots_per_turn(attacker);
    if (shot_count <= 0) {
        perform_melee_attack(attacker, attacker_id, target, target_id, config->verbose, emitter, round);
        return;
    }

    for (shot = 1; shot <= shot_count; shot++) {
        if (target->dead) {
            return;
        }

        if (attacker->loadout.rounds_in_clip <= 0) {
            if (attacker->loadout.clips > 0) {
                attacker->loadout.clips--;
                attacker->loadout.rounds_in_clip = FOC_DEFAULT_CLIP_SIZE;
                if (config->verbose) {
                    printf("  %s runs dry and reloads mid-turn.\n", attacker->name);
                }
                snprintf(summary, sizeof(summary), "%s runs dry and reloads mid-turn", attacker->name);
                emit_event(emitter, FOC_EVENT_AMMO, round, attacker_id, attacker, target_id, target, summary, 0, 0, 0, 0, FOC_LOC_BODY, attacker->loadout.clips);
                return;
            }

            if (config->verbose) {
                printf("  %s runs out of ammunition.\n", attacker->name);
            }
            snprintf(summary, sizeof(summary), "%s runs out of ammunition", attacker->name);
            emit_event(emitter, FOC_EVENT_AMMO, round, attacker_id, attacker, target_id, target, summary, 0, 0, 0, 0, FOC_LOC_BODY, 0);
            return;
        }

        attacker->loadout.rounds_in_clip--;
        perform_ranged_shot(attacker, attacker_id, target, target_id, config, shot, emitter, round);

        if (attacker->jammed) {
            return;
        }
    }
}

static int tie_breaker_score(const FocCharacter *character) {
    return foc_character_total_wounds(character) * 10 + character->stats.ph + character->stats.me;
}

int foc_run_duel(FocCharacter *a,
                 FocCharacter *b,
                 const FocDuelConfig *config,
                 FocEventSink event_sink,
                 void *event_context) {
    int round;
    int winner;
    EventEmitter emitter;
    char summary[FOC_EVENT_SUMMARY_LEN];

    emitter.sink = event_sink;
    emitter.context = event_context;
    emitter.next_event_id = 1;
    emitter.seed = g_current_seed;

    for (round = 1; round <= config->max_rounds; round++) {
        int initiative_a;
        int initiative_b;
        FocCharacter *first = a;
        FocCharacter *second = b;
        int first_id = 1;
        int second_id = 2;

        if (a->dead || b->dead) {
            break;
        }

        if (config->verbose) {
            printf("\n=== ROUND %d (%s range) ===\n", round, foc_range_to_string(config->range));
        }
        snprintf(summary, sizeof(summary), "Round %d begins at %s range", round, foc_range_to_string(config->range));
        emit_event(&emitter, FOC_EVENT_ROUND_START, round, 0, NULL, 0, NULL, summary, 0, 0, 0, 0, FOC_LOC_BODY, 0);

        apply_grave_condition(a, config->verbose, &emitter, round, 1);
        apply_grave_condition(b, config->verbose, &emitter, round, 2);

        initiative_a = roll_nd6(2) + a->stats.me;
        initiative_b = roll_nd6(2) + b->stats.me;

        snprintf(summary, sizeof(summary), "%s initiative total %d", a->name, initiative_a);
        emit_event(&emitter, FOC_EVENT_ROLL, round, 1, a, 0, NULL, summary, initiative_a - a->stats.me, a->stats.me, initiative_a, 0, FOC_LOC_BODY, 0);
        snprintf(summary, sizeof(summary), "%s initiative total %d", b->name, initiative_b);
        emit_event(&emitter, FOC_EVENT_ROLL, round, 2, b, 0, NULL, summary, initiative_b - b->stats.me, b->stats.me, initiative_b, 0, FOC_LOC_BODY, 0);

        if (initiative_b > initiative_a) {
            first = b;
            second = a;
            first_id = 2;
            second_id = 1;
        }

        if (config->verbose) {
            printf("Initiative: %s=%d, %s=%d -> %s acts first\n",
                   a->name,
                   initiative_a,
                   b->name,
                   initiative_b,
                   first->name);
        }

        take_turn(first, first_id, second, second_id, config, &emitter, round);
        if (!second->dead) {
            take_turn(second, second_id, first, first_id, config, &emitter, round);
        }

        if (config->verbose) {
            foc_print_character_summary(a);
            foc_print_character_summary(b);
        }

        if (a->dead || b->dead) {
            break;
        }
    }

    if (a->dead && !b->dead) {
        winner = 2;
    } else if (b->dead && !a->dead) {
        winner = 1;
    } else {
        int score_a = tie_breaker_score(a);
        int score_b = tie_breaker_score(b);
        if (score_a > score_b) {
            winner = 1;
        } else if (score_b > score_a) {
            winner = 2;
        } else {
            winner = (roll_d6() <= 3) ? 1 : 2;
        }
    }

    snprintf(summary, sizeof(summary), "Winner: %s", winner == 1 ? a->name : b->name);
    emit_event(&emitter, FOC_EVENT_RESULT, round, winner, winner == 1 ? a : b, winner == 1 ? 2 : 1, winner == 1 ? b : a, summary, 0, 0, 0, 0, FOC_LOC_BODY, winner);
    return winner;
}

void foc_event_buffer_clear(FocEventBuffer *buffer) {
    if (buffer == NULL) {
        return;
    }
    memset(buffer, 0, sizeof(*buffer));
}

int foc_event_buffer_count(const FocEventBuffer *buffer) {
    if (buffer == NULL) {
        return 0;
    }
    return buffer->count;
}

bool foc_event_buffer_truncated(const FocEventBuffer *buffer) {
    if (buffer == NULL) {
        return false;
    }
    return buffer->truncated;
}

const FocEvent *foc_event_buffer_get(const FocEventBuffer *buffer, int index) {
    if (buffer == NULL || index < 0 || index >= buffer->count) {
        return NULL;
    }
    return &buffer->events[index];
}

static void event_buffer_sink(const FocEvent *event, void *context) {
    FocEventBuffer *buffer = (FocEventBuffer *)context;
    if (buffer == NULL) {
        return;
    }
    if (buffer->count >= FOC_EVENT_BUFFER_CAPACITY) {
        buffer->truncated = true;
        return;
    }
    buffer->events[buffer->count++] = *event;
}

int foc_run_duel_to_buffer(FocCharacter *a,
                           FocCharacter *b,
                           const FocDuelConfig *config,
                           FocEventBuffer *buffer) {
    foc_event_buffer_clear(buffer);
    return foc_run_duel(a, b, config, event_buffer_sink, buffer);
}

static void set_action_message(char *message, size_t message_size, const char *text) {
    if (message != NULL && message_size > 0) {
        snprintf(message, message_size, "%s", text);
    }
}

bool foc_board_reload(FocCharacter *character, FocEventBuffer *buffer, char *message, size_t message_size) {
    EventEmitter emitter;
    char summary[FOC_EVENT_SUMMARY_LEN];

    if (character->dead || character->unconscious) {
        set_action_message(message, message_size, "Character cannot reload.");
        return false;
    }
    if (character->loadout.rounds_in_clip >= FOC_DEFAULT_CLIP_SIZE) {
        set_action_message(message, message_size, "Clip is already full.");
        return false;
    }
    if (character->loadout.clips <= 0) {
        set_action_message(message, message_size, "No clips remain.");
        return false;
    }

    character->loadout.clips--;
    character->loadout.rounds_in_clip = FOC_DEFAULT_CLIP_SIZE;
    snprintf(summary, sizeof(summary), "%s reloads", character->name);

    emitter.sink = event_buffer_sink;
    emitter.context = buffer;
    emitter.next_event_id = buffer == NULL ? 1 : buffer->count + 1;
    emitter.seed = g_current_seed;
    emit_event(&emitter, FOC_EVENT_AMMO, 0, 1, character, 0, NULL, summary, 0, 0, 0, 0, FOC_LOC_BODY, character->loadout.clips);

    set_action_message(message, message_size, summary);
    return true;
}

bool foc_board_clear_jam(FocCharacter *character, FocEventBuffer *buffer, char *message, size_t message_size) {
    EventEmitter emitter;
    char summary[FOC_EVENT_SUMMARY_LEN];

    if (character->dead || character->unconscious) {
        set_action_message(message, message_size, "Character cannot clear a jam.");
        return false;
    }
    if (!character->jammed) {
        set_action_message(message, message_size, "Weapon is not jammed.");
        return false;
    }

    character->jammed = false;
    snprintf(summary, sizeof(summary), "%s clears weapon jam", character->name);

    emitter.sink = event_buffer_sink;
    emitter.context = buffer;
    emitter.next_event_id = buffer == NULL ? 1 : buffer->count + 1;
    emitter.seed = g_current_seed;
    emit_event(&emitter, FOC_EVENT_STATUS, 0, 1, character, 0, NULL, summary, 0, 0, 0, 0, FOC_LOC_BODY, 0);

    set_action_message(message, message_size, summary);
    return true;
}

bool foc_board_ranged_attack(FocCharacter *attacker,
                             FocCharacter *target,
                             int distance_yards,
                             FocEventBuffer *buffer,
                             char *message,
                             size_t message_size) {
    FocTargetingPreview preview;
    FocDuelConfig config;
    EventEmitter emitter;

    foc_make_targeting_preview(attacker, target, distance_yards, &preview);
    if (!preview.can_attack) {
        set_action_message(message, message_size, preview.explanation);
        return false;
    }

    attacker->loadout.rounds_in_clip--;

    config.range = preview.range;
    config.max_rounds = 1;
    config.verbose = false;

    emitter.sink = event_buffer_sink;
    emitter.context = buffer;
    emitter.next_event_id = buffer == NULL ? 1 : buffer->count + 1;
    emitter.seed = g_current_seed;
    perform_ranged_shot(attacker, 1, target, 2, &config, 1, &emitter, 0);

    if (message != NULL && message_size > 0) {
        snprintf(message,
                 message_size,
                 "%s attacks %s at %d yards.",
                 attacker->name,
                 target->name,
                 distance_yards);
    }
    return true;
}

bool foc_board_heal(FocCharacter *healer,
                    FocCharacter *target,
                    FocEventBuffer *buffer,
                    char *message,
                    size_t message_size) {
    EventEmitter emitter;
    FocHitLocation location = FOC_LOC_BODY;
    int roll;
    int threshold = 6;
    int recovered = 0;
    char summary[FOC_EVENT_SUMMARY_LEN];

    if (healer->dead || healer->unconscious || target->dead) {
        set_action_message(message, message_size, "Healing is not possible.");
        return false;
    }

    if (healer->loadout.medical == FOC_MEDICAL_FIRST_AID) {
        threshold = 5;
    } else if (healer->loadout.medical == FOC_MEDICAL_PARAMEDIC) {
        threshold = 4;
    } else if (healer->loadout.medical == FOC_MEDICAL_PHARMA) {
        threshold = 3;
    }

    roll = roll_d6();
    if (roll >= threshold) {
        if (target->wounds.body < target->max_wounds.body) {
            target->wounds.body++;
            recovered = 1;
        } else if (target->wounds.left_leg < target->max_wounds.left_leg) {
            target->wounds.left_leg++;
            location = FOC_LOC_LEFT_LEG;
            recovered = 1;
        } else if (target->wounds.right_leg < target->max_wounds.right_leg) {
            target->wounds.right_leg++;
            location = FOC_LOC_RIGHT_LEG;
            recovered = 1;
        } else if (target->wounds.left_arm < target->max_wounds.left_arm) {
            target->wounds.left_arm++;
            location = FOC_LOC_LEFT_ARM;
            recovered = 1;
        } else if (target->wounds.right_arm < target->max_wounds.right_arm) {
            target->wounds.right_arm++;
            location = FOC_LOC_RIGHT_ARM;
            recovered = 1;
        } else if (target->wounds.head < target->max_wounds.head) {
            target->wounds.head++;
            location = FOC_LOC_HEAD;
            recovered = 1;
        }
    }
    foc_refresh_character_state(target);

    snprintf(summary,
             sizeof(summary),
             "%s healing roll %d vs %d: %s",
             healer->name,
             roll,
             threshold,
             recovered ? "recovered 1 wound" : "no recovery");

    emitter.sink = event_buffer_sink;
    emitter.context = buffer;
    emitter.next_event_id = buffer == NULL ? 1 : buffer->count + 1;
    emitter.seed = g_current_seed;
    emit_event(&emitter, FOC_EVENT_STATUS, 0, 1, healer, 2, target, summary, roll, 0, roll, threshold, location, recovered);

    set_action_message(message, message_size, summary);
    return recovered != 0;
}

bool foc_board_grenade_attack(FocCharacter *attacker,
                              FocCharacter *target,
                              int distance_yards,
                              bool enclosed,
                              FocEventBuffer *buffer,
                              char *message,
                              size_t message_size) {
    EventEmitter emitter;
    int reliability;
    int radius = enclosed ? 18 : 9;
    int damage = 0;
    int i;
    char summary[FOC_EVENT_SUMMARY_LEN];

    if (attacker->dead || attacker->unconscious) {
        set_action_message(message, message_size, "Attacker cannot throw a grenade.");
        return false;
    }
    if (distance_yards > 50 || distance_yards > radius) {
        set_action_message(message, message_size, "Target is outside grenade effect.");
        return false;
    }

    reliability = roll_nd6(2);
    if (reliability <= 4) {
        snprintf(summary, sizeof(summary), "%s grenade is a dud (%d).", attacker->name, reliability);
        emitter.sink = event_buffer_sink;
        emitter.context = buffer;
        emitter.next_event_id = buffer == NULL ? 1 : buffer->count + 1;
        emitter.seed = g_current_seed;
        emit_event(&emitter, FOC_EVENT_ATTACK, 0, 1, attacker, 2, target, summary, reliability, 0, reliability, 5, FOC_LOC_BODY, 0);
        set_action_message(message, message_size, summary);
        return false;
    }

    if (distance_yards <= 3 || (enclosed && distance_yards <= 6)) {
        damage = roll_d6();
    } else if (distance_yards <= 6 || (enclosed && distance_yards <= 12)) {
        damage = (roll_d6() + 1) / 2;
    } else {
        damage = (roll_d6() % 2) + 1;
    }

    emitter.sink = event_buffer_sink;
    emitter.context = buffer;
    emitter.next_event_id = buffer == NULL ? 1 : buffer->count + 1;
    emitter.seed = g_current_seed;

    snprintf(summary, sizeof(summary), "%s grenade reliability %d, %d wound attempt(s).", attacker->name, reliability, damage);
    emit_event(&emitter, FOC_EVENT_ATTACK, 0, 1, attacker, 2, target, summary, reliability, 0, reliability, 5, FOC_LOC_BODY, damage);

    for (i = 0; i < damage && !target->dead; i++) {
        FocHitLocation loc = roll_bullet_hit_location();
        bool inflicted = apply_single_wound(target, loc, false);
        snprintf(summary,
                 sizeof(summary),
                 "%s grenade hits %s %s: %s",
                 attacker->name,
                 target->name,
                 foc_location_to_string(loc),
                 inflicted ? "wound" : "no effect");
        emit_event(&emitter, FOC_EVENT_WOUND, 0, 1, attacker, 2, target, summary, 0, 0, 0, 0, loc, inflicted ? 1 : 0);
    }

    snprintf(message,
             message_size,
             "%s throws grenade at %s.",
             attacker->name,
             target->name);
    return true;
}

void foc_generate_scenario(const FocScenarioConfig *config, FocScenario *scenario) {
    unsigned int seed = config == NULL ? 40u : config->seed;
    int width = config == NULL || config->width <= 0 ? 16 : config->width;
    int height = config == NULL || config->height <= 0 ? 10 : config->height;
    int difficulty = config == NULL ? 1 : config->difficulty;
    unsigned int mixed = seed * 1103515245u + 12345u;
    FocObjectiveType objective = (FocObjectiveType)(mixed % 3u);

    memset(scenario, 0, sizeof(*scenario));
    scenario->seed = seed;
    scenario->width = width;
    scenario->height = height;
    scenario->objective = objective;
    scenario->player_x = 2;
    scenario->player_y = height / 2;
    scenario->opponent_x = width - 3;
    scenario->opponent_y = (height / 2) + ((mixed & 1u) ? 1 : -1);
    if (scenario->opponent_y < 1) scenario->opponent_y = 1;
    if (scenario->opponent_y >= height) scenario->opponent_y = height - 2;
    scenario->objective_x = width / 2;
    scenario->objective_y = height / 2;

    switch (objective) {
        case FOC_OBJECTIVE_HOLD_CENTER:
            snprintf(scenario->title, sizeof(scenario->title), "Hold Center %u", seed % 1000u);
            snprintf(scenario->briefing, sizeof(scenario->briefing), "Control the center marker through the skirmish. Difficulty %d.", difficulty);
            break;
        case FOC_OBJECTIVE_BREAKTHROUGH:
            snprintf(scenario->title, sizeof(scenario->title), "Breakthrough %u", seed % 1000u);
            snprintf(scenario->briefing, sizeof(scenario->briefing), "Reach the far edge or eliminate the opposition. Difficulty %d.", difficulty);
            break;
        case FOC_OBJECTIVE_ELIMINATION:
        default:
            snprintf(scenario->title, sizeof(scenario->title), "Contact %u", seed % 1000u);
            snprintf(scenario->briefing, sizeof(scenario->briefing), "Eliminate or incapacitate the opposition. Difficulty %d.", difficulty);
            break;
    }
}

static int sign_int(int value) {
    if (value > 0) return 1;
    if (value < 0) return -1;
    return 0;
}

void foc_choose_ai_action(const FocCharacter *ai,
                          const FocCharacter *enemy,
                          int ai_x,
                          int ai_y,
                          int enemy_x,
                          int enemy_y,
                          int board_width,
                          int board_height,
                          FocAIAction *action) {
    FocTargetingPreview preview;
    int distance = abs(ai_x - enemy_x) + abs(ai_y - enemy_y);
    int yards = distance * 2;
    int move_yards;
    int move_cells;

    memset(action, 0, sizeof(*action));
    action->type = FOC_AI_WAIT;
    action->target_index = 0;
    action->move_x = ai_x;
    action->move_y = ai_y;

    if (ai->dead || ai->unconscious) {
        snprintf(action->explanation, sizeof(action->explanation), "%s cannot act.", ai->name);
        return;
    }
    if (ai->jammed) {
        action->type = FOC_AI_CLEAR_JAM;
        snprintf(action->explanation, sizeof(action->explanation), "%s clears a jam.", ai->name);
        return;
    }
    if (ai->loadout.rounds_in_clip <= 0 && ai->loadout.clips > 0) {
        action->type = FOC_AI_RELOAD;
        snprintf(action->explanation, sizeof(action->explanation), "%s reloads.", ai->name);
        return;
    }

    foc_make_targeting_preview(ai, enemy, yards, &preview);
    if (preview.can_attack) {
        action->type = FOC_AI_ATTACK;
        snprintf(action->explanation, sizeof(action->explanation), "%s attacks at %d yards.", ai->name, yards);
        return;
    }

    move_yards = foc_character_movement_yards(ai, FOC_PACE_STANDARD);
    move_cells = move_yards / 2;
    if (move_cells > 0) {
        action->type = FOC_AI_MOVE;
        action->move_x = ai_x + sign_int(enemy_x - ai_x) * move_cells;
        action->move_y = ai_y + sign_int(enemy_y - ai_y) * (move_cells > 1 ? 1 : 0);
        if (action->move_x < 0) action->move_x = 0;
        if (action->move_y < 0) action->move_y = 0;
        if (action->move_x >= board_width) action->move_x = board_width - 1;
        if (action->move_y >= board_height) action->move_y = board_height - 1;
        snprintf(action->explanation, sizeof(action->explanation), "%s moves toward the nearest enemy.", ai->name);
        return;
    }

    snprintf(action->explanation, sizeof(action->explanation), "%s waits.", ai->name);
}

void foc_campaign_init(FocCampaignRecord *campaign) {
    memset(campaign, 0, sizeof(*campaign));
    snprintf(campaign->last_summary, sizeof(campaign->last_summary), "No missions completed.");
}

void foc_campaign_apply_result(FocCampaignRecord *campaign,
                               bool player_won,
                               const FocCharacter *character,
                               const FocScenario *scenario) {
    int wounds_lost = 0;

    campaign->missions++;
    if (player_won) {
        campaign->wins++;
        campaign->xp += 3;
        campaign->advances_available++;
    } else {
        campaign->losses++;
        campaign->xp += 1;
    }

    if (character != NULL) {
        wounds_lost = character->max_wounds.head + character->max_wounds.body +
                      character->max_wounds.left_arm + character->max_wounds.right_arm +
                      character->max_wounds.left_leg + character->max_wounds.right_leg -
                      foc_character_total_wounds(character);
        if (wounds_lost > 0) {
            campaign->injuries += wounds_lost;
        }
    }

    snprintf(campaign->last_summary,
             sizeof(campaign->last_summary),
             "%s: %s, +%d XP.",
             scenario == NULL ? "Mission" : scenario->title,
             player_won ? "win" : "loss",
             player_won ? 3 : 1);
}

bool foc_campaign_spend_advancement(FocCampaignRecord *campaign, FocCharacter *character, char *message, size_t message_size) {
    if (campaign->advances_available <= 0) {
        set_action_message(message, message_size, "No advancement is available.");
        return false;
    }
    if (foc_stat_total(&character->stats) >= 40) {
        set_action_message(message, message_size, "Stat cap reached.");
        return false;
    }

    character->stats.ph++;
    if (character->stats.ph > 10) {
        character->stats.ph = 10;
        character->stats.me++;
        if (character->stats.me > 10) {
            character->stats.me = 10;
        }
    }
    campaign->advances_available--;
    set_action_message(message, message_size, "Advancement applied: +1 PH or ME fallback.");
    return true;
}

static void write_json_escaped(FILE *file, const char *value) {
    const char *p = value;
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
}

void foc_write_event_jsonl(FILE *file, const FocEvent *event) {
    fprintf(file,
            "{\"event_id\":%d,\"seed\":%u,\"round\":%d,\"type\":\"%s\","
            "\"actor_id\":%d,\"target_id\":%d,\"actor\":\"",
            event->event_id,
            event->seed,
            event->round,
            foc_event_type_to_string(event->type),
            event->actor_id,
            event->target_id);
    write_json_escaped(file, event->actor);
    fprintf(file, "\",\"target\":\"");
    write_json_escaped(file, event->target);
    fprintf(file, "\",\"summary\":\"");
    write_json_escaped(file, event->summary);
    fprintf(file,
            "\",\"roll\":%d,\"bonus\":%d,\"total\":%d,\"threshold\":%d,"
            "\"location\":\"%s\",\"amount\":%d}\n",
            event->roll,
            event->bonus,
            event->total,
            event->threshold,
            foc_location_to_string(event->location),
            event->amount);
}
