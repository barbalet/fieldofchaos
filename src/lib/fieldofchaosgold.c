#include "fieldofchaosgold.h"
#include "fieldofchaosgold_internal.h"

#include <string.h>

static unsigned int focgold_rng_state = 1u;

static unsigned int focgold_next_random(void) {
    focgold_rng_state = focgold_rng_state * 1664525u + 1013904223u;
    return focgold_rng_state;
}

void focgold_seed(unsigned int seed) {
    focgold_rng_state = seed;
}

int focgold_roll_d6(void) {
    return (int)(((focgold_next_random() >> 16) % 6u) + 1u);
}

int focgold_roll_d6_sum(int dice_count) {
    int total = 0;
    int index;

    if (dice_count <= 0) {
        return 0;
    }

    for (index = 0; index < dice_count; index++) {
        total += focgold_roll_d6();
    }

    return total;
}

int focgold_roll_d6_highest(int dice_count) {
    int highest = 0;
    int index;

    if (dice_count <= 0) {
        return 0;
    }

    for (index = 0; index < dice_count; index++) {
        int roll = focgold_roll_d6();
        if (roll > highest) {
            highest = roll;
        }
    }

    return highest;
}

int focgold_stat_total(const focgold_stats *stats) {
    if (stats == 0) {
        return 0;
    }

    return stats->regular_intelligence +
           stats->irregular_intelligence +
           stats->appearance +
           stats->physical_health +
           stats->mental_health;
}

static bool focgold_stats_have_valid_values(const focgold_stats *stats) {
    if (stats == 0) {
        return false;
    }

    return stats->regular_intelligence >= 0 && stats->regular_intelligence <= 10 &&
           stats->irregular_intelligence >= 0 && stats->irregular_intelligence <= 10 &&
           stats->appearance >= 0 && stats->appearance <= 10 &&
           stats->physical_health >= 0 && stats->physical_health <= 10 &&
           stats->mental_health >= 0 && stats->mental_health <= 10;
}

bool focgold_validate_initial_stats(const focgold_stats *stats) {
    return focgold_stats_have_valid_values(stats) &&
           focgold_stat_total(stats) <= 30;
}

bool focgold_validate_long_term_stats(const focgold_stats *stats) {
    return focgold_stats_have_valid_values(stats) &&
           focgold_stat_total(stats) <= 40;
}

static bool focgold_is_permutation_of_five(const size_t values[5]) {
    bool seen[5] = {false, false, false, false, false};
    size_t index;

    for (index = 0; index < 5; index++) {
        if (values[index] >= 5 || seen[values[index]]) {
            return false;
        }
        seen[values[index]] = true;
    }

    return true;
}

static bool focgold_dice_are_valid(const int dice[3]) {
    size_t index;

    for (index = 0; index < 3; index++) {
        if (dice[index] < 1 || dice[index] > 6) {
            return false;
        }
    }

    return true;
}

bool focgold_generate_stats(const focgold_stat_generation_choice *choice,
                            focgold_stats *stats) {
    int values[5];
    size_t index;

    if (choice == 0 || stats == 0) {
        return false;
    }

    if (!focgold_dice_are_valid(choice->rolled_dice) ||
        choice->first_inverted_die >= 3 ||
        choice->second_inverted_die >= 3 ||
        choice->first_inverted_die == choice->second_inverted_die ||
        choice->plus_two_value >= 5 ||
        !focgold_is_permutation_of_five(choice->stat_value_indices)) {
        return false;
    }

    values[0] = choice->rolled_dice[0];
    values[1] = choice->rolled_dice[1];
    values[2] = choice->rolled_dice[2];
    values[3] = 7 - choice->rolled_dice[choice->first_inverted_die];
    values[4] = 7 - choice->rolled_dice[choice->second_inverted_die];

    for (index = 0; index < 5; index++) {
        values[index] += index == choice->plus_two_value ? 2 : 1;
    }

    stats->regular_intelligence = values[choice->stat_value_indices[focgold_stat_regular_intelligence]];
    stats->irregular_intelligence = values[choice->stat_value_indices[focgold_stat_irregular_intelligence]];
    stats->appearance = values[choice->stat_value_indices[focgold_stat_appearance]];
    stats->physical_health = values[choice->stat_value_indices[focgold_stat_physical_health]];
    stats->mental_health = values[choice->stat_value_indices[focgold_stat_mental_health]];

    return focgold_validate_initial_stats(stats);
}

const char *focgold_stat_name(focgold_stat_id stat) {
    static const char *const names[focgold_stat_count] = {
        "Regular Intelligence",
        "Irregular Intelligence",
        "Appearance",
        "Physical Health",
        "Mental Health"
    };

    if (stat < 0 || stat >= focgold_stat_count) {
        return 0;
    }

    return names[stat];
}

int focgold_skill_value(const focgold_skills *skills, focgold_skill_id skill) {
    if (skills == 0) {
        return 0;
    }

    switch (skill) {
    case focgold_skill_firearm_basic:
        return skills->firearm_basic;
    case focgold_skill_firearm_advanced:
        return skills->firearm_advanced;
    case focgold_skill_close_combat:
        return skills->close_combat;
    case focgold_skill_improvised_weapon_basic:
        return skills->improvised_weapon_basic;
    case focgold_skill_clandestine_weapon_basic:
        return skills->clandestine_weapon_basic;
    case focgold_skill_carpentry:
        return skills->carpentry;
    case focgold_skill_manufacture_clandestine_weapon:
        return skills->manufacture_clandestine_weapon;
    case focgold_skill_chemistry_basic:
        return skills->chemistry_basic;
    case focgold_skill_explosive_engineering:
        return skills->explosive_engineering;
    case focgold_skill_biology_basic:
        return skills->biology_basic;
    case focgold_skill_chemistry_advanced:
        return skills->chemistry_advanced;
    case focgold_skill_chemistry_pharmaceutical:
        return skills->chemistry_pharmaceutical;
    case focgold_skill_first_aid:
        return skills->first_aid;
    case focgold_skill_paramedic:
        return skills->paramedic;
    case focgold_skill_evade:
        return skills->evade;
    case focgold_skill_marching:
        return skills->marching;
    case focgold_skill_running:
        return skills->running;
    case focgold_skill_count:
        break;
    }

    return 0;
}

bool focgold_validate_skills(const focgold_skills *skills) {
    int skill;

    if (skills == 0) {
        return false;
    }

    for (skill = 0; skill < focgold_skill_count; skill++) {
        int value = focgold_skill_value(skills, (focgold_skill_id)skill);
        if (value < 0 || value > 100) {
            return false;
        }
    }

    return true;
}

const char *focgold_skill_name(focgold_skill_id skill) {
    static const char *const names[focgold_skill_count] = {
        "Firearm Use, Basic",
        "Firearm Use, Advanced",
        "Close Combat",
        "Improvised Weapon Use, Basic",
        "Clandestine Weapon Use, Basic",
        "Carpentry",
        "Manufacture Clandestine Weapon",
        "Chemistry, Basic",
        "Explosive Engineering",
        "Biology, Basic",
        "Chemistry, Advanced",
        "Chemistry, Pharmaceutical",
        "First Aid",
        "Paramedic",
        "Evade",
        "Marching",
        "Running"
    };

    if (skill < 0 || skill >= focgold_skill_count) {
        return 0;
    }

    return names[skill];
}

focgold_stat_id focgold_skill_stat(focgold_skill_id skill) {
    static const focgold_stat_id stats[focgold_skill_count] = {
        focgold_stat_physical_health,
        focgold_stat_mental_health,
        focgold_stat_physical_health,
        focgold_stat_irregular_intelligence,
        focgold_stat_irregular_intelligence,
        focgold_stat_physical_health,
        focgold_stat_irregular_intelligence,
        focgold_stat_regular_intelligence,
        focgold_stat_irregular_intelligence,
        focgold_stat_regular_intelligence,
        focgold_stat_regular_intelligence,
        focgold_stat_regular_intelligence,
        focgold_stat_regular_intelligence,
        focgold_stat_regular_intelligence,
        focgold_stat_physical_health,
        focgold_stat_physical_health,
        focgold_stat_physical_health
    };

    if (skill < 0 || skill >= focgold_skill_count) {
        return focgold_stat_count;
    }

    return stats[skill];
}

const focgold_skill_id *focgold_skill_prerequisites(focgold_skill_id skill,
                                                    size_t *count) {
    static const focgold_skill_id firearm_advanced[] = {
        focgold_skill_firearm_basic
    };
    static const focgold_skill_id manufacture_clandestine_weapon[] = {
        focgold_skill_carpentry
    };
    static const focgold_skill_id explosive_engineering[] = {
        focgold_skill_chemistry_basic
    };
    static const focgold_skill_id chemistry_advanced[] = {
        focgold_skill_biology_basic,
        focgold_skill_chemistry_basic
    };
    static const focgold_skill_id chemistry_pharmaceutical[] = {
        focgold_skill_chemistry_advanced
    };
    static const focgold_skill_id paramedic[] = {
        focgold_skill_first_aid
    };

    if (count != 0) {
        *count = 0;
    }

    switch (skill) {
    case focgold_skill_firearm_advanced:
        if (count != 0) {
            *count = 1;
        }
        return firearm_advanced;
    case focgold_skill_manufacture_clandestine_weapon:
        if (count != 0) {
            *count = 1;
        }
        return manufacture_clandestine_weapon;
    case focgold_skill_explosive_engineering:
        if (count != 0) {
            *count = 1;
        }
        return explosive_engineering;
    case focgold_skill_chemistry_advanced:
        if (count != 0) {
            *count = 2;
        }
        return chemistry_advanced;
    case focgold_skill_chemistry_pharmaceutical:
        if (count != 0) {
            *count = 1;
        }
        return chemistry_pharmaceutical;
    case focgold_skill_paramedic:
        if (count != 0) {
            *count = 1;
        }
        return paramedic;
    case focgold_skill_firearm_basic:
    case focgold_skill_close_combat:
    case focgold_skill_improvised_weapon_basic:
    case focgold_skill_clandestine_weapon_basic:
    case focgold_skill_carpentry:
    case focgold_skill_chemistry_basic:
    case focgold_skill_biology_basic:
    case focgold_skill_first_aid:
    case focgold_skill_evade:
    case focgold_skill_marching:
    case focgold_skill_running:
    case focgold_skill_count:
        break;
    }

    return 0;
}

bool focgold_skill_improves_healing(focgold_skill_id skill) {
    return skill == focgold_skill_first_aid ||
           skill == focgold_skill_paramedic;
}

const char *focgold_weapon_name(focgold_weapon weapon) {
    static const char *const names[focgold_weapon_count] = {
        "Sniper Rifle",
        "Rifle",
        "Carbine",
        "Automatic",
        "SubMG",
        "Shotgun"
    };

    if (weapon < 0 || weapon >= focgold_weapon_count) {
        return 0;
    }

    return names[weapon];
}

bool focgold_parse_weapon(const char *name, focgold_weapon *weapon) {
    int index;

    if (name == 0 || weapon == 0) {
        return false;
    }

    for (index = 0; index < focgold_weapon_count; index++) {
        const focgold_weapon current = (focgold_weapon)index;
        if (strcmp(name, focgold_weapon_name(current)) == 0) {
            *weapon = current;
            return true;
        }
    }

    return false;
}

bool focgold_weapon_uses_firearm_basic(focgold_weapon weapon) {
    return weapon >= 0 && weapon < focgold_weapon_count;
}

int focgold_weapon_range_inches(focgold_weapon weapon,
                                focgold_range_band range) {
    static const int ranges[focgold_weapon_count][3] = {
        {24, 36, 48},
        {18, 27, 36},
        {12, 18, 24},
        {8, 12, 16},
        {6, 9, 12},
        {0, 0, 0}
    };

    if (weapon < 0 || weapon >= focgold_weapon_count ||
        range < 0 || range > focgold_range_long) {
        return 0;
    }

    return ranges[weapon][range];
}

int focgold_shotgun_total_length_inches(void) {
    return 15;
}

int focgold_shotgun_band_max_inches(focgold_shotgun_band band) {
    static const int maximums[focgold_shotgun_band_count] = {
        5,
        10,
        15
    };

    if (band < 0 || band >= focgold_shotgun_band_count) {
        return 0;
    }

    return maximums[band];
}

int focgold_shotgun_band_width_inches(focgold_shotgun_band band) {
    static const int widths[focgold_shotgun_band_count] = {
        1,
        2,
        3
    };

    if (band < 0 || band >= focgold_shotgun_band_count) {
        return 0;
    }

    return widths[band];
}

focgold_damage_roll focgold_shotgun_band_wounds(focgold_shotgun_band band) {
    static const focgold_damage_roll rolls[focgold_shotgun_band_count] = {
        {2, 6},
        {1, 6},
        {1, 3}
    };
    const focgold_damage_roll none = {0, 0};

    if (band < 0 || band >= focgold_shotgun_band_count) {
        return none;
    }

    return rolls[band];
}

int focgold_grenade_throw_range_inches(void) {
    return 12;
}

int focgold_grenade_radius_inches(focgold_grenade_band band,
                                  bool inside_building) {
    static const int open_radii[focgold_grenade_band_count] = {
        1,
        3,
        4
    };
    static const int building_radii[focgold_grenade_band_count] = {
        3,
        6,
        9
    };

    if (band < 0 || band >= focgold_grenade_band_count) {
        return 0;
    }

    return inside_building ? building_radii[band] : open_radii[band];
}

focgold_damage_roll focgold_grenade_band_wounds(focgold_grenade_band band) {
    static const focgold_damage_roll rolls[focgold_grenade_band_count] = {
        {1, 6},
        {1, 3},
        {1, 2}
    };
    const focgold_damage_roll none = {0, 0};

    if (band < 0 || band >= focgold_grenade_band_count) {
        return none;
    }

    return rolls[band];
}

bool focgold_grenade_uses_skill(void) {
    return false;
}

bool focgold_grenade_skill_modifies_reliability(focgold_skill_id skill) {
    (void)skill;
    return false;
}

bool focgold_resolve_grenade_reliability(focgold_grenade_reliability_result *result) {
    int total;

    if (result == 0) {
        return false;
    }

    total = focgold_roll_d6_sum(focgold_grenade_reliability_dice);
    result->dice_total = total;
    result->dud = total <= focgold_grenade_dud_threshold;

    return true;
}

const char *focgold_range_name(focgold_range_band range) {
    static const char *const names[] = {
        "Close",
        "Standard",
        "Long"
    };

    if (range < 0 || range > focgold_range_long) {
        return 0;
    }

    return names[range];
}

int focgold_ranged_base_dice(focgold_range_band range) {
    static const int dice[] = {
        3,
        2,
        1
    };

    if (range < 0 || range > focgold_range_long) {
        return 0;
    }

    return dice[range];
}

int focgold_ranged_hit_threshold(focgold_range_band range) {
    static const int thresholds[] = {
        4,
        10,
        16
    };

    if (range < 0 || range > focgold_range_long) {
        return 0;
    }

    return thresholds[range];
}

int focgold_ranged_head_shot_threshold(focgold_range_band range) {
    static const int thresholds[] = {
        6,
        12,
        18
    };

    if (range < 0 || range > focgold_range_long) {
        return 0;
    }

    return thresholds[range];
}

int focgold_ranged_modifier_dice(const focgold_attack_modifiers *modifiers) {
    focgold_cover cover;
    int modifier = 0;

    if (modifiers == 0) {
        return 0;
    }

    cover = modifiers->cover;
    if (modifiers->line_crosses_grenade_debris &&
        cover == focgold_cover_none) {
        cover = focgold_cover_light;
    }

    switch (cover) {
    case focgold_cover_light:
        modifier -= 1;
        break;
    case focgold_cover_heavy:
        modifier -= 2;
        break;
    case focgold_cover_none:
        break;
    }

    switch (modifiers->target_movement) {
    case focgold_target_movement_standard:
        modifier -= 1;
        break;
    case focgold_target_movement_fast:
        modifier -= 2;
        break;
    case focgold_target_movement_none:
        break;
    }

    if (modifiers->target_has_evade) {
        modifier -= 1;
    }

    return modifier;
}

int focgold_weapon_shots_per_turn(focgold_weapon weapon,
                                  bool has_firearm_advanced) {
    switch (weapon) {
    case focgold_weapon_rifle:
    case focgold_weapon_carbine:
        return has_firearm_advanced ? 2 : 1;
    case focgold_weapon_automatic:
        return 2;
    case focgold_weapon_submg:
        return 3;
    case focgold_weapon_sniper_rifle:
    case focgold_weapon_shotgun:
        return 1;
    case focgold_weapon_count:
        break;
    }

    return 0;
}

focgold_movement_pace focgold_weapon_movement_rule(focgold_weapon weapon) {
    (void)weapon;
    return focgold_movement_standard;
}

bool focgold_weapon_can_head_shot(focgold_weapon weapon) {
    return weapon == focgold_weapon_sniper_rifle ||
           weapon == focgold_weapon_rifle ||
           weapon == focgold_weapon_carbine;
}

int focgold_weapon_jam_dice_count(focgold_weapon weapon) {
    switch (weapon) {
    case focgold_weapon_automatic:
        return 2;
    case focgold_weapon_submg:
        return 1;
    case focgold_weapon_sniper_rifle:
    case focgold_weapon_rifle:
    case focgold_weapon_carbine:
    case focgold_weapon_shotgun:
    case focgold_weapon_count:
        break;
    }

    return 0;
}

int focgold_weapon_jam_threshold(focgold_weapon weapon) {
    switch (weapon) {
    case focgold_weapon_automatic:
    case focgold_weapon_submg:
        return 1;
    case focgold_weapon_sniper_rifle:
    case focgold_weapon_rifle:
    case focgold_weapon_carbine:
    case focgold_weapon_shotgun:
    case focgold_weapon_count:
        break;
    }

    return 0;
}

bool focgold_resolve_weapon_jam(focgold_weapon weapon, bool *jammed) {
    int dice_count;
    int threshold;

    if (jammed == 0) {
        return false;
    }

    dice_count = focgold_weapon_jam_dice_count(weapon);
    threshold = focgold_weapon_jam_threshold(weapon);
    if (dice_count == 0 || threshold == 0) {
        *jammed = false;
        return true;
    }

    *jammed = focgold_roll_d6_sum(dice_count) <= threshold;
    return true;
}

int focgold_reload_turn_count(void) {
    return focgold_reload_turns;
}

int focgold_clear_jam_turn_count(void) {
    return focgold_clear_jam_turns;
}

int focgold_called_head_shot_required_setup_moves(void) {
    return focgold_called_head_shot_setup_moves;
}

bool focgold_called_head_shot_ready(int moves_declared_before_firing) {
    return moves_declared_before_firing >= focgold_called_head_shot_setup_moves;
}

bool focgold_called_head_shot_miss_is_total_miss(void) {
    return true;
}

static int focgold_skill_enabled(int skill_value) {
    return skill_value > 0;
}

static int focgold_disabled_leg_count(const focgold_wounds *wounds) {
    int disabled = 0;

    if (wounds == 0) {
        return 0;
    }

    if (wounds->left_leg <= 0) {
        disabled++;
    }
    if (wounds->right_leg <= 0) {
        disabled++;
    }

    return disabled;
}

int focgold_movement_inches(focgold_movement_pace pace,
                            const focgold_skills *skills,
                            const focgold_wounds *wounds) {
    int movement;
    int disabled_legs;

    switch (pace) {
    case focgold_movement_very_slow:
        movement = 2;
        if (skills != 0 && focgold_skill_enabled(skills->evade)) {
            movement += 1;
        }
        break;
    case focgold_movement_slow:
        movement = 3;
        if (skills != 0 && focgold_skill_enabled(skills->marching)) {
            movement += 1;
        }
        break;
    case focgold_movement_standard:
        movement = 5;
        if (skills != 0 && focgold_skill_enabled(skills->marching)) {
            movement += 1;
        }
        break;
    case focgold_movement_fast:
        movement = 8;
        if (skills != 0 && focgold_skill_enabled(skills->running)) {
            movement += 2;
        }
        break;
    default:
        return 0;
    }

    disabled_legs = focgold_disabled_leg_count(wounds);
    if (disabled_legs >= 2) {
        return 0;
    }
    if (disabled_legs == 1) {
        movement /= 2;
    }

    return movement;
}

int focgold_movement_after_one_grenade_inches(focgold_movement_pace pace,
                                              const focgold_skills *skills,
                                              const focgold_wounds *wounds) {
    return focgold_movement_inches(pace, skills, wounds) / 2;
}

bool focgold_can_throw_two_grenades(bool stationary) {
    return stationary;
}

focgold_turn_permissions focgold_two_grenade_turn_permissions(void) {
    focgold_turn_permissions permissions = {
        false,
        false,
        false,
        false,
        false,
        false,
        false
    };

    return permissions;
}
