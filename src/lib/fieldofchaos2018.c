#include "fieldofchaos2018.h"
#include "fieldofchaos2018_internal.h"

#include <string.h>

static unsigned int foc2018_rng_state = 1u;

static unsigned int foc2018_next_random(void) {
    foc2018_rng_state = foc2018_rng_state * 1664525u + 1013904223u;
    return foc2018_rng_state;
}

void foc2018_seed(unsigned int seed) {
    foc2018_rng_state = seed;
}

int foc2018_roll_d6(void) {
    return (int)(((foc2018_next_random() >> 16) % 6u) + 1u);
}

int foc2018_roll_d6_sum(int dice_count) {
    int total = 0;
    int index;

    if (dice_count <= 0) {
        return 0;
    }

    for (index = 0; index < dice_count; index++) {
        total += foc2018_roll_d6();
    }

    return total;
}

int foc2018_roll_d6_highest(int dice_count) {
    int highest = 0;
    int index;

    if (dice_count <= 0) {
        return 0;
    }

    for (index = 0; index < dice_count; index++) {
        int roll = foc2018_roll_d6();
        if (roll > highest) {
            highest = roll;
        }
    }

    return highest;
}

int foc2018_stat_total(const foc2018_stats *stats) {
    if (stats == 0) {
        return 0;
    }

    return stats->regular_intelligence +
           stats->irregular_intelligence +
           stats->appearance +
           stats->physical_health +
           stats->mental_health;
}

static bool foc2018_stats_have_valid_values(const foc2018_stats *stats) {
    if (stats == 0) {
        return false;
    }

    return stats->regular_intelligence >= 0 && stats->regular_intelligence <= 10 &&
           stats->irregular_intelligence >= 0 && stats->irregular_intelligence <= 10 &&
           stats->appearance >= 0 && stats->appearance <= 10 &&
           stats->physical_health >= 0 && stats->physical_health <= 10 &&
           stats->mental_health >= 0 && stats->mental_health <= 10;
}

bool foc2018_validate_initial_stats(const foc2018_stats *stats) {
    return foc2018_stats_have_valid_values(stats) &&
           foc2018_stat_total(stats) <= 30;
}

bool foc2018_validate_long_term_stats(const foc2018_stats *stats) {
    return foc2018_stats_have_valid_values(stats) &&
           foc2018_stat_total(stats) <= 40;
}

static bool foc2018_is_permutation_of_five(const size_t values[5]) {
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

static bool foc2018_dice_are_valid(const int dice[3]) {
    size_t index;

    for (index = 0; index < 3; index++) {
        if (dice[index] < 1 || dice[index] > 6) {
            return false;
        }
    }

    return true;
}

bool foc2018_generate_stats(const foc2018_stat_generation_choice *choice,
                            foc2018_stats *stats) {
    int values[5];
    size_t index;

    if (choice == 0 || stats == 0) {
        return false;
    }

    if (!foc2018_dice_are_valid(choice->rolled_dice) ||
        choice->first_inverted_die >= 3 ||
        choice->second_inverted_die >= 3 ||
        choice->first_inverted_die == choice->second_inverted_die ||
        choice->plus_two_value >= 5 ||
        !foc2018_is_permutation_of_five(choice->stat_value_indices)) {
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

    stats->regular_intelligence = values[choice->stat_value_indices[foc2018_stat_regular_intelligence]];
    stats->irregular_intelligence = values[choice->stat_value_indices[foc2018_stat_irregular_intelligence]];
    stats->appearance = values[choice->stat_value_indices[foc2018_stat_appearance]];
    stats->physical_health = values[choice->stat_value_indices[foc2018_stat_physical_health]];
    stats->mental_health = values[choice->stat_value_indices[foc2018_stat_mental_health]];

    return foc2018_validate_initial_stats(stats);
}

const char *foc2018_stat_name(foc2018_stat_id stat) {
    static const char *const names[foc2018_stat_count] = {
        "Regular Intelligence",
        "Irregular Intelligence",
        "Appearance",
        "Physical Health",
        "Mental Health"
    };

    if (stat < 0 || stat >= foc2018_stat_count) {
        return 0;
    }

    return names[stat];
}

int foc2018_skill_value(const foc2018_skills *skills, foc2018_skill_id skill) {
    if (skills == 0) {
        return 0;
    }

    switch (skill) {
    case foc2018_skill_firearm_basic:
        return skills->firearm_basic;
    case foc2018_skill_firearm_advanced:
        return skills->firearm_advanced;
    case foc2018_skill_close_combat:
        return skills->close_combat;
    case foc2018_skill_improvised_weapon_basic:
        return skills->improvised_weapon_basic;
    case foc2018_skill_clandestine_weapon_basic:
        return skills->clandestine_weapon_basic;
    case foc2018_skill_carpentry:
        return skills->carpentry;
    case foc2018_skill_manufacture_clandestine_weapon:
        return skills->manufacture_clandestine_weapon;
    case foc2018_skill_chemistry_basic:
        return skills->chemistry_basic;
    case foc2018_skill_explosive_engineering:
        return skills->explosive_engineering;
    case foc2018_skill_biology_basic:
        return skills->biology_basic;
    case foc2018_skill_chemistry_advanced:
        return skills->chemistry_advanced;
    case foc2018_skill_chemistry_pharmaceutical:
        return skills->chemistry_pharmaceutical;
    case foc2018_skill_first_aid:
        return skills->first_aid;
    case foc2018_skill_paramedic:
        return skills->paramedic;
    case foc2018_skill_evade:
        return skills->evade;
    case foc2018_skill_marching:
        return skills->marching;
    case foc2018_skill_running:
        return skills->running;
    case foc2018_skill_count:
        break;
    }

    return 0;
}

bool foc2018_validate_skills(const foc2018_skills *skills) {
    int skill;

    if (skills == 0) {
        return false;
    }

    for (skill = 0; skill < foc2018_skill_count; skill++) {
        int value = foc2018_skill_value(skills, (foc2018_skill_id)skill);
        if (value < 0 || value > 100) {
            return false;
        }
    }

    return true;
}

const char *foc2018_skill_name(foc2018_skill_id skill) {
    static const char *const names[foc2018_skill_count] = {
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

    if (skill < 0 || skill >= foc2018_skill_count) {
        return 0;
    }

    return names[skill];
}

foc2018_stat_id foc2018_skill_stat(foc2018_skill_id skill) {
    static const foc2018_stat_id stats[foc2018_skill_count] = {
        foc2018_stat_physical_health,
        foc2018_stat_mental_health,
        foc2018_stat_physical_health,
        foc2018_stat_irregular_intelligence,
        foc2018_stat_irregular_intelligence,
        foc2018_stat_physical_health,
        foc2018_stat_irregular_intelligence,
        foc2018_stat_regular_intelligence,
        foc2018_stat_irregular_intelligence,
        foc2018_stat_regular_intelligence,
        foc2018_stat_regular_intelligence,
        foc2018_stat_regular_intelligence,
        foc2018_stat_regular_intelligence,
        foc2018_stat_regular_intelligence,
        foc2018_stat_physical_health,
        foc2018_stat_physical_health,
        foc2018_stat_physical_health
    };

    if (skill < 0 || skill >= foc2018_skill_count) {
        return foc2018_stat_count;
    }

    return stats[skill];
}

const foc2018_skill_id *foc2018_skill_prerequisites(foc2018_skill_id skill,
                                                    size_t *count) {
    static const foc2018_skill_id firearm_advanced[] = {
        foc2018_skill_firearm_basic
    };
    static const foc2018_skill_id manufacture_clandestine_weapon[] = {
        foc2018_skill_carpentry
    };
    static const foc2018_skill_id explosive_engineering[] = {
        foc2018_skill_chemistry_basic
    };
    static const foc2018_skill_id chemistry_advanced[] = {
        foc2018_skill_biology_basic,
        foc2018_skill_chemistry_basic
    };
    static const foc2018_skill_id chemistry_pharmaceutical[] = {
        foc2018_skill_chemistry_advanced
    };
    static const foc2018_skill_id paramedic[] = {
        foc2018_skill_first_aid
    };

    if (count != 0) {
        *count = 0;
    }

    switch (skill) {
    case foc2018_skill_firearm_advanced:
        if (count != 0) {
            *count = 1;
        }
        return firearm_advanced;
    case foc2018_skill_manufacture_clandestine_weapon:
        if (count != 0) {
            *count = 1;
        }
        return manufacture_clandestine_weapon;
    case foc2018_skill_explosive_engineering:
        if (count != 0) {
            *count = 1;
        }
        return explosive_engineering;
    case foc2018_skill_chemistry_advanced:
        if (count != 0) {
            *count = 2;
        }
        return chemistry_advanced;
    case foc2018_skill_chemistry_pharmaceutical:
        if (count != 0) {
            *count = 1;
        }
        return chemistry_pharmaceutical;
    case foc2018_skill_paramedic:
        if (count != 0) {
            *count = 1;
        }
        return paramedic;
    case foc2018_skill_firearm_basic:
    case foc2018_skill_close_combat:
    case foc2018_skill_improvised_weapon_basic:
    case foc2018_skill_clandestine_weapon_basic:
    case foc2018_skill_carpentry:
    case foc2018_skill_chemistry_basic:
    case foc2018_skill_biology_basic:
    case foc2018_skill_first_aid:
    case foc2018_skill_evade:
    case foc2018_skill_marching:
    case foc2018_skill_running:
    case foc2018_skill_count:
        break;
    }

    return 0;
}

bool foc2018_skill_improves_healing(foc2018_skill_id skill) {
    return skill == foc2018_skill_first_aid ||
           skill == foc2018_skill_paramedic;
}

const char *foc2018_weapon_name(foc2018_weapon weapon) {
    static const char *const names[foc2018_weapon_count] = {
        "Sniper Rifle",
        "Rifle",
        "Carbine",
        "Automatic",
        "SubMG"
    };

    if (weapon < 0 || weapon >= foc2018_weapon_count) {
        return 0;
    }

    return names[weapon];
}

bool foc2018_parse_weapon(const char *name, foc2018_weapon *weapon) {
    int index;

    if (name == 0 || weapon == 0) {
        return false;
    }

    for (index = 0; index < foc2018_weapon_count; index++) {
        const foc2018_weapon current = (foc2018_weapon)index;
        if (strcmp(name, foc2018_weapon_name(current)) == 0) {
            *weapon = current;
            return true;
        }
    }

    return false;
}
