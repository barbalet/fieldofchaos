#include "fieldofchaosgold.h"

#include <stdio.h>

static int failures = 0;

static void expect_true(int condition, const char *message) {
    if (!condition) {
        fprintf(stderr, "FAIL: %s\n", message);
        failures++;
    }
}

static void expect_int(int actual, int expected, const char *message) {
    if (actual != expected) {
        fprintf(stderr, "FAIL: %s: got %d expected %d\n",
                message,
                actual,
                expected);
        failures++;
    }
}

static focgold_character regression_character(void) {
    focgold_character character = {0};

    character.weapon = focgold_weapon_rifle;
    character.skills.firearm_basic = 7;
    focgold_default_wounds(&character.maximum_wounds);
    focgold_default_wounds(&character.current_wounds);
    character.rounds_in_clip = focgold_standard_clip_rounds;

    return character;
}

static void test_no_2024_ranged_or_submg_rules(void) {
    expect_int(focgold_ranged_hit_threshold(focgold_range_close),
               4,
               "Gold should keep inherited close threshold 4");
    expect_int(focgold_ranged_hit_threshold(focgold_range_standard),
               10,
               "Gold should keep inherited standard threshold 10");
    expect_int(focgold_ranged_hit_threshold(focgold_range_long),
               16,
               "Gold should keep inherited long threshold 16");
    expect_int(focgold_weapon_range_inches(focgold_weapon_submg,
                                           focgold_range_close),
               6,
               "Gold should keep inherited SubMG close range");
    expect_int(focgold_weapon_range_inches(focgold_weapon_submg,
                                           focgold_range_standard),
               9,
               "Gold should keep inherited SubMG standard range");
    expect_int(focgold_weapon_range_inches(focgold_weapon_submg,
                                           focgold_range_long),
               12,
               "Gold should keep inherited SubMG long range");
    expect_int(focgold_weapon_shots_per_turn(focgold_weapon_submg, false),
               3,
               "Gold should keep inherited SubMG fire rate");
}

static void test_no_body_only_or_winded_model(void) {
    focgold_wounds wounds;

    expect_int((int)sizeof(focgold_wounds),
               (int)(sizeof(int) * 7u),
               "wounds should be seven explicit body-part pools");

    focgold_default_wounds(&wounds);
    wounds.chest = 1;
    expect_true(!focgold_is_unconscious(&wounds),
                "damaged but nonzero chest should not use a winded rule");
    expect_true(!focgold_is_dead(&wounds),
                "nonzero body-part wounds should not be dead");
}

static void test_no_grave_ph_roll(void) {
    focgold_wounds wounds = {
        1,
        1,
        1,
        0,
        1,
        1,
        0
    };
    int expected_next_roll;
    int actual_next_roll;

    focgold_seed(611u);
    expected_next_roll = focgold_roll_d6();
    focgold_seed(611u);
    expect_true(focgold_is_grave_condition(&wounds),
                "low total wounds should mark grave condition");
    actual_next_roll = focgold_roll_d6();
    expect_int(actual_next_roll,
               expected_next_roll,
               "grave condition check should not consume a PH roll");
}

static void test_no_2024_skill_advancement_or_pharma_healing(void) {
    focgold_character attacker = regression_character();
    focgold_character target = regression_character();
    focgold_attack_modifiers modifiers = {
        focgold_cover_none,
        focgold_target_movement_none,
        false,
        false
    };
    focgold_attack_result result;

    expect_true(!focgold_skill_improves_healing(
                    focgold_skill_chemistry_pharmaceutical),
                "Pharmaceutical Chemistry should not improve Gold healing");

    focgold_seed(612u);
    expect_true(focgold_resolve_ranged_attack(&attacker,
                                              &target,
                                              focgold_range_close,
                                              &modifiers,
                                              false,
                                              &result),
                "attack should resolve for advancement regression check");
    expect_int(attacker.skills.firearm_basic,
               7,
               "using a skill should not mutate use-based advancement state");
}

static void test_grenade_and_yard_regressions(void) {
    focgold_weapon weapon = focgold_weapon_rifle;

    expect_true(!focgold_parse_weapon("Grenade", &weapon),
                "Grenade should not be a standard firearm weapon");
    expect_int(focgold_weapon_range_inches(focgold_weapon_rifle,
                                           focgold_range_close),
               18,
               "Rifle close range should be inches, not yard data");
    expect_int(focgold_grenade_throw_range_inches(),
               12,
               "Grenade throw range should be the Gold inch value");
}

int main(void) {
    test_no_2024_ranged_or_submg_rules();
    test_no_body_only_or_winded_model();
    test_no_grave_ph_roll();
    test_no_2024_skill_advancement_or_pharma_healing();
    test_grenade_and_yard_regressions();

    if (failures != 0) {
        fprintf(stderr, "%d failure(s)\n", failures);
        return 1;
    }

    printf("regression tests passed\n");
    return 0;
}
