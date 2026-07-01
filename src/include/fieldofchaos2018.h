#ifndef foc2018_fieldofchaos2018_h
#define foc2018_fieldofchaos2018_h

#include <stdbool.h>
#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

enum {
    foc2018_character_name_capacity = 64,
    foc2018_standard_clip_rounds = 10
};

typedef enum foc2018_stat_id {
    foc2018_stat_regular_intelligence = 0,
    foc2018_stat_irregular_intelligence,
    foc2018_stat_appearance,
    foc2018_stat_physical_health,
    foc2018_stat_mental_health,
    foc2018_stat_count
} foc2018_stat_id;

typedef struct foc2018_stats {
    int regular_intelligence;
    int irregular_intelligence;
    int appearance;
    int physical_health;
    int mental_health;
} foc2018_stats;

typedef enum foc2018_skill_id {
    foc2018_skill_firearm_basic = 0,
    foc2018_skill_firearm_advanced,
    foc2018_skill_close_combat,
    foc2018_skill_improvised_weapon_basic,
    foc2018_skill_clandestine_weapon_basic,
    foc2018_skill_carpentry,
    foc2018_skill_manufacture_clandestine_weapon,
    foc2018_skill_chemistry_basic,
    foc2018_skill_explosive_engineering,
    foc2018_skill_biology_basic,
    foc2018_skill_chemistry_advanced,
    foc2018_skill_chemistry_pharmaceutical,
    foc2018_skill_first_aid,
    foc2018_skill_paramedic,
    foc2018_skill_evade,
    foc2018_skill_marching,
    foc2018_skill_running,
    foc2018_skill_count
} foc2018_skill_id;

typedef struct foc2018_skills {
    int firearm_basic;
    int firearm_advanced;
    int close_combat;
    int improvised_weapon_basic;
    int clandestine_weapon_basic;
    int carpentry;
    int manufacture_clandestine_weapon;
    int chemistry_basic;
    int explosive_engineering;
    int biology_basic;
    int chemistry_advanced;
    int chemistry_pharmaceutical;
    int first_aid;
    int paramedic;
    int evade;
    int marching;
    int running;
} foc2018_skills;

typedef enum foc2018_weapon {
    foc2018_weapon_sniper_rifle = 0,
    foc2018_weapon_rifle,
    foc2018_weapon_carbine,
    foc2018_weapon_automatic,
    foc2018_weapon_submg,
    foc2018_weapon_count
} foc2018_weapon;

typedef enum foc2018_range_band {
    foc2018_range_close = 0,
    foc2018_range_standard,
    foc2018_range_long
} foc2018_range_band;

typedef enum foc2018_movement_pace {
    foc2018_movement_very_slow = 0,
    foc2018_movement_slow,
    foc2018_movement_standard,
    foc2018_movement_fast
} foc2018_movement_pace;

typedef enum foc2018_cover {
    foc2018_cover_none = 0,
    foc2018_cover_light,
    foc2018_cover_heavy
} foc2018_cover;

typedef enum foc2018_target_movement {
    foc2018_target_movement_none = 0,
    foc2018_target_movement_standard,
    foc2018_target_movement_fast
} foc2018_target_movement;

typedef enum foc2018_hit_location {
    foc2018_hit_left_leg = 0,
    foc2018_hit_right_leg,
    foc2018_hit_chest,
    foc2018_hit_abdomen,
    foc2018_hit_left_arm,
    foc2018_hit_right_arm,
    foc2018_hit_head
} foc2018_hit_location;

typedef struct foc2018_wounds {
    int head;
    int chest;
    int abdomen;
    int left_arm;
    int right_arm;
    int left_leg;
    int right_leg;
} foc2018_wounds;

typedef enum foc2018_melee_attack {
    foc2018_melee_blow = 0,
    foc2018_melee_weapon
} foc2018_melee_attack;

typedef enum foc2018_healing_method {
    foc2018_healing_no_skill = 0,
    foc2018_healing_bandage,
    foc2018_healing_first_aid,
    foc2018_healing_paramedic
} foc2018_healing_method;

typedef struct foc2018_character {
    char name[foc2018_character_name_capacity];
    foc2018_stats stats;
    foc2018_skills skills;
    foc2018_weapon weapon;
    foc2018_wounds maximum_wounds;
    foc2018_wounds current_wounds;
    int clips;
    int rounds_in_clip;
    bool has_bandage;
} foc2018_character;

typedef struct foc2018_stat_generation_choice {
    int rolled_dice[3];
    size_t first_inverted_die;
    size_t second_inverted_die;
    size_t plus_two_value;
    size_t stat_value_indices[5];
} foc2018_stat_generation_choice;

typedef struct foc2018_attack_modifiers {
    foc2018_cover cover;
    foc2018_target_movement target_movement;
    bool target_has_evade;
} foc2018_attack_modifiers;

typedef struct foc2018_attack_result {
    bool can_attack;
    bool hit;
    bool head_shot;
    bool jammed;
    bool head_shot_allowed;
    int dice_count;
    int dice_total;
    int skill_dice;
    int modifier_dice;
    int required_to_hit;
    int required_head_shot;
    int shots_fired;
    int rounds_spent;
    foc2018_hit_location location;
    int wounds_inflicted;
} foc2018_attack_result;

typedef struct foc2018_melee_result {
    bool hit;
    int dice_count;
    int dice_total;
    int required_to_hit;
    foc2018_hit_location location;
    int wounds_inflicted;
} foc2018_melee_result;

typedef struct foc2018_healing_result {
    bool recovered;
    int dice_count;
    int highest_die;
    int required_to_recover;
    foc2018_hit_location location;
    int wounds_recovered;
} foc2018_healing_result;

void foc2018_seed(unsigned int seed);
int foc2018_roll_d6(void);

int foc2018_stat_total(const foc2018_stats *stats);
bool foc2018_validate_initial_stats(const foc2018_stats *stats);
bool foc2018_validate_long_term_stats(const foc2018_stats *stats);
bool foc2018_generate_stats(const foc2018_stat_generation_choice *choice,
                            foc2018_stats *stats);

const char *foc2018_stat_name(foc2018_stat_id stat);
bool foc2018_validate_skills(const foc2018_skills *skills);
int foc2018_skill_value(const foc2018_skills *skills, foc2018_skill_id skill);
const char *foc2018_skill_name(foc2018_skill_id skill);
foc2018_stat_id foc2018_skill_stat(foc2018_skill_id skill);
const foc2018_skill_id *foc2018_skill_prerequisites(foc2018_skill_id skill,
                                                    size_t *count);
bool foc2018_skill_improves_healing(foc2018_skill_id skill);

const char *foc2018_weapon_name(foc2018_weapon weapon);
bool foc2018_parse_weapon(const char *name, foc2018_weapon *weapon);
int foc2018_weapon_range_inches(foc2018_weapon weapon,
                                foc2018_range_band range);
int foc2018_weapon_shots_per_turn(foc2018_weapon weapon,
                                  bool has_firearm_advanced);
foc2018_movement_pace foc2018_weapon_movement_rule(foc2018_weapon weapon);
bool foc2018_weapon_can_head_shot(foc2018_weapon weapon);

const char *foc2018_range_name(foc2018_range_band range);
int foc2018_ranged_base_dice(foc2018_range_band range);
int foc2018_ranged_hit_threshold(foc2018_range_band range);
int foc2018_ranged_head_shot_threshold(foc2018_range_band range);
int foc2018_ranged_modifier_dice(const foc2018_attack_modifiers *modifiers);

int foc2018_movement_inches(foc2018_movement_pace pace,
                            const foc2018_skills *skills,
                            const foc2018_wounds *wounds);

void foc2018_default_wounds(foc2018_wounds *wounds);
int foc2018_total_wounds(const foc2018_wounds *wounds);
bool foc2018_is_unconscious(const foc2018_wounds *wounds);
bool foc2018_is_grave_condition(const foc2018_wounds *wounds);
bool foc2018_is_dead(const foc2018_wounds *wounds);
bool foc2018_can_firearm(const foc2018_wounds *wounds);
foc2018_hit_location foc2018_roll_bullet_hit_location(void);
const char *foc2018_hit_location_name(foc2018_hit_location location);

bool foc2018_resolve_ranged_attack(const foc2018_character *attacker,
                                   const foc2018_character *target,
                                   foc2018_range_band range,
                                   const foc2018_attack_modifiers *modifiers,
                                   bool called_head_shot,
                                   foc2018_attack_result *result);
bool foc2018_resolve_melee(const foc2018_character *attacker,
                           foc2018_melee_attack attack,
                           foc2018_melee_result *result);
bool foc2018_resolve_healing(const foc2018_character *healer,
                             const foc2018_character *target,
                             foc2018_healing_method method,
                             foc2018_healing_result *result);

#ifdef __cplusplus
}
#endif

#endif
