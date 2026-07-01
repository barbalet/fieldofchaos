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
    bool head_shot_success = true;
    bool expected_head_shot_success;
    focgold_skills no_skills = {0};
    focgold_wounds wounds;

    expect_true(focgold_animal_head_shot_releases_gas(),
                "Animal head shot should release gas");
    expect_int(focgold_animal_head_shot_success_threshold(),
               5,
               "Animal head shot confirmation should succeed on 5-6");
    expect_int(focgold_foe_attack_modifier_dice(focgold_foe_animal),
               1,
               "Animal should add 1 attack die");
    expect_int(focgold_animal_gas_attack_required_rolls(),
               2,
               "Animal gas attack should require double rolls");
    expect_true(focgold_animal_gas_attack_halves_movement(),
                "Animal gas attack should halve movement");

    focgold_seed(500u);
    expected_head_shot_success =
        focgold_roll_d6() >= focgold_animal_head_shot_success_threshold();
    focgold_seed(500u);
    expect_true(focgold_resolve_animal_head_shot(true, &head_shot_success),
                "Animal head shot confirmation should resolve");
    expect_true(head_shot_success == expected_head_shot_success,
                "Animal head shot confirmation should roll d6 against 5-6");
    head_shot_success = true;
    expect_true(focgold_resolve_animal_head_shot(false, &head_shot_success),
                "non-head hits should resolve without confirmation");
    expect_true(!head_shot_success,
                "non-head hits should not become Animal head shots");
    expect_true(!focgold_resolve_animal_head_shot(true, 0),
                "missing Animal head shot output should fail");

    focgold_default_wounds(&wounds);
    expect_int(focgold_animal_gas_attack_movement_inches(
                   focgold_movement_standard,
                   &no_skills,
                   &wounds),
               2,
               "Animal gas attack should use half standard movement");

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
    focgold_skills no_skills = {0};
    focgold_wounds wounds;

    expect_int(focgold_foe_movement_modifier_inches(focgold_foe_hunter),
               1,
               "Hunter should add 1 inch movement");
    focgold_default_wounds(&wounds);
    expect_int(focgold_foe_movement_inches(focgold_foe_hunter,
                                           focgold_movement_standard,
                                           &no_skills,
                                           &wounds),
               6,
               "Hunter movement should integrate +1 into movement calculation");
    wounds.left_leg = 0;
    wounds.right_leg = 0;
    expect_int(focgold_foe_movement_inches(focgold_foe_hunter,
                                           focgold_movement_standard,
                                           &no_skills,
                                           &wounds),
               0,
               "Hunter movement should not override disabled movement");
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

static void test_soldier_armor_damage_application(void) {
    focgold_wounds wounds;
    bool saved;
    bool expected_saved;

    focgold_default_wounds(&wounds);
    focgold_seed(504u);
    expected_saved = focgold_roll_d6() >= focgold_soldier_armor_save_threshold();
    focgold_seed(504u);
    expect_true(focgold_apply_damage_with_soldier_armor(&wounds,
                                                        focgold_damage_shotgun,
                                                        false,
                                                        focgold_hit_chest,
                                                        2,
                                                        &saved),
                "Shotgun damage should integrate Soldier armor");
    expect_true(saved == expected_saved,
                "integrated Soldier armor should expose save result");
    expect_int(wounds.chest,
               saved ? 4 : 2,
               "saved Shotgun damage should prevent wounds, failed save should apply");

    focgold_default_wounds(&wounds);
    saved = true;
    expect_true(focgold_apply_damage_with_soldier_armor(&wounds,
                                                        focgold_damage_grenade,
                                                        false,
                                                        focgold_hit_chest,
                                                        2,
                                                        &saved),
                "Grenade damage should apply without armor by default");
    expect_true(!saved,
                "Grenade should not receive armor save by default");
    expect_int(wounds.chest,
               2,
               "Grenade damage should apply when no armor save is available");

    focgold_default_wounds(&wounds);
    focgold_seed(505u);
    expected_saved = focgold_roll_d6() >= focgold_soldier_armor_save_threshold();
    focgold_seed(505u);
    expect_true(focgold_apply_damage_with_soldier_armor(&wounds,
                                                        focgold_damage_grenade,
                                                        true,
                                                        focgold_hit_chest,
                                                        2,
                                                        &saved),
                "referee-counted Grenade damage should integrate Soldier armor");
    expect_true(saved == expected_saved,
                "referee-counted Grenade armor should expose save result");
    expect_int(wounds.chest,
               saved ? 4 : 2,
               "referee-counted Grenade save should control wound application");
    expect_true(!focgold_apply_damage_with_soldier_armor(&wounds,
                                                         focgold_damage_bullet,
                                                         false,
                                                         focgold_hit_chest,
                                                         1,
                                                         0),
                "missing integrated armor output should fail");
}

int main(void) {
    test_animal_rules();
    test_hunter_and_soldier_rules();
    test_soldier_armor_damage_application();

    if (failures != 0) {
        fprintf(stderr, "%d failure(s)\n", failures);
        return 1;
    }

    printf("foe tests passed\n");
    return 0;
}
