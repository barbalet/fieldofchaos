#ifndef FIELDOFCHAOS_ENGINE_H
#define FIELDOFCHAOS_ENGINE_H

#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>

#define FOC_MAX_NAME_LEN 64
#define FOC_EVENT_SUMMARY_LEN 192
#define FOC_DEFAULT_CLIP_SIZE 10
#define FOC_DEFAULT_CLIPS 4
#define FOC_DEFAULT_MAX_ROUNDS 1000
#define FOC_CHARACTER_SCHEMA_VERSION 1
#define FOC_EVENT_BUFFER_CAPACITY 512
#define FOC_ACTION_MESSAGE_LEN 192
#define FOC_SCENARIO_TEXT_LEN 192

typedef enum {
    FOC_RANGE_CLOSE = 0,
    FOC_RANGE_STANDARD = 1,
    FOC_RANGE_LONG = 2
} FocRangeBand;

typedef enum {
    FOC_WEAPON_NONE = 0,
    FOC_WEAPON_SNIPER_RIFLE,
    FOC_WEAPON_RIFLE,
    FOC_WEAPON_SUBMG,
    FOC_WEAPON_SHOTGUN
} FocWeaponType;

typedef enum {
    FOC_MEDICAL_NONE = 0,
    FOC_MEDICAL_FIRST_AID,
    FOC_MEDICAL_PARAMEDIC,
    FOC_MEDICAL_PHARMA
} FocMedicalLevel;

typedef enum {
    FOC_LOC_HEAD = 0,
    FOC_LOC_BODY,
    FOC_LOC_LEFT_ARM,
    FOC_LOC_RIGHT_ARM,
    FOC_LOC_LEFT_LEG,
    FOC_LOC_RIGHT_LEG
} FocHitLocation;

typedef enum {
    FOC_EVENT_ROUND_START = 0,
    FOC_EVENT_ROLL,
    FOC_EVENT_ATTACK,
    FOC_EVENT_WOUND,
    FOC_EVENT_AMMO,
    FOC_EVENT_STATUS,
    FOC_EVENT_RESULT
} FocEventType;

typedef enum {
    FOC_PACE_SLOW = 0,
    FOC_PACE_STANDARD,
    FOC_PACE_FAST
} FocMovementPace;

typedef enum {
    FOC_OBJECTIVE_ELIMINATION = 0,
    FOC_OBJECTIVE_HOLD_CENTER,
    FOC_OBJECTIVE_BREAKTHROUGH
} FocObjectiveType;

typedef enum {
    FOC_AI_WAIT = 0,
    FOC_AI_MOVE,
    FOC_AI_ATTACK,
    FOC_AI_RELOAD,
    FOC_AI_CLEAR_JAM
} FocAIActionType;

typedef struct {
    int re;
    int ir;
    int ap;
    int ph;
    int me;
} FocStats;

typedef struct {
    int head;
    int body;
    int left_arm;
    int right_arm;
    int left_leg;
    int right_leg;
} FocWounds;

typedef struct {
    bool firearm_basic;
    bool firearm_advanced;
    bool firearm_sniper;
    bool evade;
    bool marching;
    bool running;
    bool close_combat;
} FocSkills;

typedef struct {
    FocWeaponType weapon;
    int clips;
    int rounds_in_clip;
    bool armor_save;
    bool in_cover;
    FocMedicalLevel medical;
} FocLoadout;

typedef struct {
    int schema_version;
    char name[FOC_MAX_NAME_LEN];
    FocStats stats;
    FocWounds max_wounds;
    FocWounds wounds;
    FocSkills skills;
    FocLoadout loadout;
    int stunned_turns;
    bool jammed;
    bool unconscious;
    bool dead;
} FocCharacter;

typedef struct {
    FocRangeBand range;
    int max_rounds;
    bool verbose;
} FocDuelConfig;

typedef struct {
    int event_id;
    unsigned int seed;
    int round;
    FocEventType type;
    int actor_id;
    int target_id;
    char actor[FOC_MAX_NAME_LEN];
    char target[FOC_MAX_NAME_LEN];
    char summary[FOC_EVENT_SUMMARY_LEN];
    int roll;
    int bonus;
    int total;
    int threshold;
    FocHitLocation location;
    int amount;
} FocEvent;

typedef struct {
    char name[FOC_MAX_NAME_LEN];
    int schema_version;
    FocStats stats;
    FocWounds wounds;
    int stat_total;
    int wound_total;
    FocWeaponType weapon;
    FocMedicalLevel medical;
    int clips;
    int rounds_in_clip;
    int initiative_modifier;
    bool firearm_basic;
    bool firearm_advanced;
    bool firearm_sniper;
    bool evade;
    bool marching;
    bool running;
    bool close_combat;
    bool armor_save;
    bool in_cover;
    bool jammed;
    bool unconscious;
    bool dead;
} FocCharacterSnapshot;

typedef struct {
    FocEvent events[FOC_EVENT_BUFFER_CAPACITY];
    int count;
    bool truncated;
} FocEventBuffer;

typedef struct {
    bool in_range;
    bool can_attack;
    FocRangeBand range;
    int distance_yards;
    int max_range_yards;
    int base_dice;
    int penalties;
    int dice_count;
    int bonus;
    int threshold;
    char explanation[FOC_ACTION_MESSAGE_LEN];
} FocTargetingPreview;

typedef struct {
    unsigned int seed;
    int difficulty;
    int width;
    int height;
} FocScenarioConfig;

typedef struct {
    unsigned int seed;
    int width;
    int height;
    FocObjectiveType objective;
    int player_x;
    int player_y;
    int opponent_x;
    int opponent_y;
    int objective_x;
    int objective_y;
    char title[FOC_MAX_NAME_LEN];
    char briefing[FOC_SCENARIO_TEXT_LEN];
} FocScenario;

typedef struct {
    FocAIActionType type;
    int move_x;
    int move_y;
    int target_index;
    char explanation[FOC_ACTION_MESSAGE_LEN];
} FocAIAction;

typedef struct {
    int missions;
    int wins;
    int losses;
    int xp;
    int advances_available;
    int injuries;
    char last_summary[FOC_SCENARIO_TEXT_LEN];
} FocCampaignRecord;

typedef void (*FocEventSink)(const FocEvent *event, void *context);

const char *foc_engine_version(void);
void foc_seed(unsigned int seed);
unsigned int foc_current_seed(void);

bool foc_parse_int_value(const char *s, int *out);
FocWeaponType foc_parse_weapon(const char *s);
const char *foc_weapon_to_string(FocWeaponType weapon);
FocMedicalLevel foc_parse_medical(const char *s);
const char *foc_medical_to_string(FocMedicalLevel level);
FocRangeBand foc_parse_range(const char *s);
const char *foc_range_to_string(FocRangeBand band);
const char *foc_location_to_string(FocHitLocation location);
const char *foc_event_type_to_string(FocEventType type);
const char *foc_movement_pace_to_string(FocMovementPace pace);
const char *foc_objective_to_string(FocObjectiveType objective);
const char *foc_ai_action_to_string(FocAIActionType action);

void foc_default_max_wounds(FocWounds *wounds);
void foc_generate_stats(FocStats *stats);
int foc_stat_total(const FocStats *stats);
bool foc_validate_stats(const FocStats *stats, char *error, size_t error_size);
void foc_init_default_character(FocCharacter *character, const char *name);
int foc_character_total_wounds(const FocCharacter *character);
void foc_refresh_character_state(FocCharacter *character);
void foc_make_character_snapshot(const FocCharacter *character, FocCharacterSnapshot *snapshot);

bool foc_save_character_json(const char *path, const FocCharacter *character);
bool foc_load_character_json(const char *path, FocCharacter *character, char *error, size_t error_size);
void foc_print_character_summary(const FocCharacter *character);

int foc_weapon_range_yards(FocWeaponType weapon, FocRangeBand band);
bool foc_range_for_distance(FocWeaponType weapon, int distance_yards, FocRangeBand *range_out);
int foc_character_movement_yards(const FocCharacter *character, FocMovementPace pace);
int foc_roll_initiative(const FocCharacter *character);
void foc_make_targeting_preview(const FocCharacter *attacker,
                                const FocCharacter *target,
                                int distance_yards,
                                FocTargetingPreview *preview);

int foc_run_duel(FocCharacter *a,
                 FocCharacter *b,
                 const FocDuelConfig *config,
                 FocEventSink event_sink,
                 void *event_context);

void foc_event_buffer_clear(FocEventBuffer *buffer);
int foc_event_buffer_count(const FocEventBuffer *buffer);
bool foc_event_buffer_truncated(const FocEventBuffer *buffer);
const FocEvent *foc_event_buffer_get(const FocEventBuffer *buffer, int index);
int foc_run_duel_to_buffer(FocCharacter *a,
                           FocCharacter *b,
                           const FocDuelConfig *config,
                           FocEventBuffer *buffer);
bool foc_board_reload(FocCharacter *character, FocEventBuffer *buffer, char *message, size_t message_size);
bool foc_board_clear_jam(FocCharacter *character, FocEventBuffer *buffer, char *message, size_t message_size);
bool foc_board_ranged_attack(FocCharacter *attacker,
                             FocCharacter *target,
                             int distance_yards,
                             FocEventBuffer *buffer,
                             char *message,
                             size_t message_size);
bool foc_board_heal(FocCharacter *healer,
                    FocCharacter *target,
                    FocEventBuffer *buffer,
                    char *message,
                    size_t message_size);
bool foc_board_grenade_attack(FocCharacter *attacker,
                              FocCharacter *target,
                              int distance_yards,
                              bool enclosed,
                              FocEventBuffer *buffer,
                              char *message,
                              size_t message_size);
void foc_generate_scenario(const FocScenarioConfig *config, FocScenario *scenario);
void foc_choose_ai_action(const FocCharacter *ai,
                          const FocCharacter *enemy,
                          int ai_x,
                          int ai_y,
                          int enemy_x,
                          int enemy_y,
                          int board_width,
                          int board_height,
                          FocAIAction *action);
void foc_campaign_init(FocCampaignRecord *campaign);
void foc_campaign_apply_result(FocCampaignRecord *campaign,
                               bool player_won,
                               const FocCharacter *character,
                               const FocScenario *scenario);
bool foc_campaign_spend_advancement(FocCampaignRecord *campaign, FocCharacter *character, char *message, size_t message_size);
void foc_write_event_jsonl(FILE *file, const FocEvent *event);

#endif
