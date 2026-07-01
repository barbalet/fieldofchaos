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

static void test_animal_rules(void) {
    int expected_radius;
    int expected_direction;

    expect_true(focgold_animal_head_shot_releases_gas(),
                "Animal head shot should release gas");
    expect_int(focgold_foe_attack_modifier_dice(focgold_foe_animal),
               1,
               "Animal should add 1 attack die");

    focgold_seed(501u);
    expected_radius = focgold_roll_d6() + 6;
    focgold_seed(501u);
    expect_int(focgold_animal_gas_initial_radius_inches(),
               expected_radius,
               "Animal gas cloud should use d6 + 6 inch radius");
    expect_int(focgold_animal_gas_shrink_per_turn_inches(),
               2,
               "Animal gas cloud should shrink 2 inches per turn");
    expect_int(focgold_animal_gas_drift_inches_per_turn(),
               2,
               "Animal gas cloud should drift 2 inches per turn");

    focgold_seed(502u);
    expected_direction = focgold_roll_d6();
    focgold_seed(502u);
    expect_int(focgold_animal_gas_drift_direction(),
               expected_direction,
               "Animal gas drift should use d6 direction");
}

static void test_hunter_and_soldier_rules(void) {
    bool saved = true;
    bool expected_saved;

    expect_int(focgold_foe_movement_modifier_inches(focgold_foe_hunter),
               1,
               "Hunter should add 1 inch movement");
    expect_int(focgold_foe_attack_modifier_dice(focgold_foe_hunter),
               0,
               "Hunter should not use the Animal attack bonus");

    expect_int(focgold_soldier_armor_save_threshold(),
               5,
               "Soldier armor save should succeed on 5-6");
    expect_true(focgold_damage_counts_as_shot_for_armor(
                    focgold_damage_bullet,
                    false),
                "bullet wounds should count as shot wounds");
    expect_true(focgold_damage_counts_as_shot_for_armor(
                    focgold_damage_shotgun,
                    false),
                "Shotgun wounds should count as shot wounds");
    expect_true(!focgold_damage_counts_as_shot_for_armor(
                    focgold_damage_grenade,
                    false),
                "Grenade wounds should not count as shot wounds by default");
    expect_true(focgold_damage_counts_as_shot_for_armor(
                    focgold_damage_grenade,
                    true),
                "Grenade wounds can count as shot wounds by referee ruling");
    expect_true(!focgold_damage_counts_as_shot_for_armor(
                    focgold_damage_melee,
                    false),
                "melee wounds should not count as shot wounds");

    focgold_seed(503u);
    expected_saved = focgold_roll_d6() >= focgold_soldier_armor_save_threshold();
    focgold_seed(503u);
    expect_true(focgold_resolve_soldier_armor_save(focgold_damage_shotgun,
                                                   false,
                                                   &saved),
                "Soldier armor save should resolve for Shotgun wounds");
    expect_true(saved == expected_saved,
                "Soldier armor save should roll d6 against 5-6");

    saved = true;
    expect_true(focgold_resolve_soldier_armor_save(focgold_damage_grenade,
                                                   false,
                                                   &saved),
                "Soldier armor save should resolve for Grenade wounds");
    expect_true(!saved,
                "Grenade wounds should not receive armor save by default");
    expect_true(!focgold_resolve_soldier_armor_save(focgold_damage_bullet,
                                                    false,
                                                    0),
                "missing armor save output should fail");
}

int main(void) {
    test_animal_rules();
    test_hunter_and_soldier_rules();

    if (failures != 0) {
        fprintf(stderr, "%d failure(s)\n", failures);
        return 1;
    }

    printf("foe tests passed\n");
    return 0;
}
