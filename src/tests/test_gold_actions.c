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

static void test_ranged_modifiers(void) {
    focgold_attack_modifiers modifiers = {
        focgold_cover_none,
        focgold_target_movement_none,
        false,
        false
    };

    expect_int(focgold_ranged_modifier_dice(0),
               0,
               "missing modifiers should apply no modifier");
    expect_int(focgold_ranged_modifier_dice(&modifiers),
               0,
               "empty modifiers should apply no modifier");

    modifiers.cover = focgold_cover_light;
    expect_int(focgold_ranged_modifier_dice(&modifiers),
               -1,
               "light cover should apply -1 die");
    modifiers.cover = focgold_cover_heavy;
    expect_int(focgold_ranged_modifier_dice(&modifiers),
               -2,
               "heavy cover should apply -2 dice");

    modifiers.cover = focgold_cover_none;
    modifiers.target_movement = focgold_target_movement_standard;
    expect_int(focgold_ranged_modifier_dice(&modifiers),
               -1,
               "standard target movement should apply -1 die");
    modifiers.target_movement = focgold_target_movement_fast;
    expect_int(focgold_ranged_modifier_dice(&modifiers),
               -2,
               "fast target movement should apply -2 dice");

    modifiers.target_movement = focgold_target_movement_none;
    modifiers.target_has_evade = true;
    expect_int(focgold_ranged_modifier_dice(&modifiers),
               -1,
               "target Evade should apply -1 die");

    modifiers.target_has_evade = false;
    modifiers.line_crosses_grenade_debris = true;
    expect_int(focgold_ranged_modifier_dice(&modifiers),
               -1,
               "grenade debris should act as light cover");

    modifiers.cover = focgold_cover_heavy;
    expect_int(focgold_ranged_modifier_dice(&modifiers),
               -2,
               "grenade debris should not stack beyond heavy cover");

    modifiers.target_movement = focgold_target_movement_fast;
    modifiers.target_has_evade = true;
    expect_int(focgold_ranged_modifier_dice(&modifiers),
               -5,
               "cover, movement, and Evade modifiers should stack");
}

static void test_called_head_shots(void) {
    expect_int(focgold_called_head_shot_required_setup_moves(),
               2,
               "called head shot should require two setup moves");
    expect_true(!focgold_called_head_shot_ready(0),
                "called head shot should not be ready immediately");
    expect_true(!focgold_called_head_shot_ready(1),
                "called head shot should not be ready after one move");
    expect_true(focgold_called_head_shot_ready(2),
                "called head shot should be ready after two moves");
    expect_true(focgold_called_head_shot_ready(3),
                "called head shot should remain ready after more than two moves");
    expect_true(focgold_called_head_shot_miss_is_total_miss(),
                "missed called head shots should miss altogether");
}

static focgold_wounds mobile_wounds(void) {
    focgold_wounds wounds = {
        1,
        3,
        3,
        1,
        1,
        2,
        2
    };

    return wounds;
}

static void test_movement(void) {
    focgold_skills no_skills = {0};
    focgold_skills skilled = {0};
    focgold_wounds wounds = mobile_wounds();

    expect_int(focgold_movement_inches(focgold_movement_very_slow,
                                       &no_skills,
                                       &wounds),
               2,
               "very slow movement should be 2 inches");
    expect_int(focgold_movement_inches(focgold_movement_slow,
                                       &no_skills,
                                       &wounds),
               3,
               "slow movement should be 3 inches");
    expect_int(focgold_movement_inches(focgold_movement_standard,
                                       &no_skills,
                                       &wounds),
               5,
               "standard movement should be 5 inches");
    expect_int(focgold_movement_inches(focgold_movement_fast,
                                       &no_skills,
                                       &wounds),
               8,
               "fast movement should be 8 inches");

    skilled.evade = 1;
    skilled.marching = 1;
    skilled.running = 1;
    expect_int(focgold_movement_inches(focgold_movement_very_slow,
                                       &skilled,
                                       &wounds),
               3,
               "Evade should add 1 inch to very slow movement");
    expect_int(focgold_movement_inches(focgold_movement_slow,
                                       &skilled,
                                       &wounds),
               4,
               "Marching should add 1 inch to slow movement");
    expect_int(focgold_movement_inches(focgold_movement_standard,
                                       &skilled,
                                       &wounds),
               6,
               "Marching should add 1 inch to standard movement");
    expect_int(focgold_movement_inches(focgold_movement_fast,
                                       &skilled,
                                       &wounds),
               10,
               "Running should add 2 inches to fast movement");

    wounds.left_leg = 0;
    expect_int(focgold_movement_inches(focgold_movement_standard,
                                       &no_skills,
                                       &wounds),
               2,
               "one disabled leg should halve movement, rounded down");
    wounds.right_leg = 0;
    expect_int(focgold_movement_inches(focgold_movement_fast,
                                       &no_skills,
                                       &wounds),
               0,
               "two disabled legs should prevent movement");
}

static void test_weapon_movement_rules(void) {
    expect_true(focgold_weapon_movement_rule(focgold_weapon_sniper_rifle) ==
                    focgold_movement_very_slow,
                "sniper rifle should use Very Slow movement");
    expect_true(focgold_weapon_movement_rule(focgold_weapon_rifle) ==
                    focgold_movement_slow,
                "rifle should use Slow movement");
    expect_true(focgold_weapon_movement_rule(focgold_weapon_carbine) ==
                    focgold_movement_standard,
                "carbine should use Standard movement");
    expect_true(focgold_weapon_movement_rule(focgold_weapon_automatic) ==
                    focgold_movement_standard,
                "automatic should use Standard movement");
    expect_true(focgold_weapon_movement_rule(focgold_weapon_submg) ==
                    focgold_movement_fast,
                "SubMG should use Fast movement");
    expect_true(focgold_weapon_movement_rule(focgold_weapon_shotgun) ==
                    focgold_movement_standard,
                "shotgun should default to Standard movement data");
    expect_true(focgold_weapon_has_referee_movement_rule(focgold_weapon_shotgun),
                "shotgun should preserve the referee-ruling movement note");
    expect_true(!focgold_weapon_has_referee_movement_rule(focgold_weapon_rifle),
                "rifle movement should be table-defined");

    expect_int(focgold_weapon_minimum_range_inches(focgold_weapon_sniper_rifle),
               12,
               "sniper rifle should have a 12 inch minimum range");
    expect_true(!focgold_weapon_can_fire_at_distance(
                    focgold_weapon_sniper_rifle,
                    11),
                "sniper rifle should not fire under 12 inches");
    expect_true(focgold_weapon_can_fire_at_distance(
                    focgold_weapon_sniper_rifle,
                    12),
                "sniper rifle should fire at 12 inches");
    expect_true(focgold_weapon_can_fire_at_distance(focgold_weapon_rifle, 0),
                "rifle should have no minimum range");
    expect_true(!focgold_weapon_can_fire_at_distance(focgold_weapon_rifle, -1),
                "negative target distance should be invalid");
}

static void test_grenade_action_economy(void) {
    focgold_skills no_skills = {0};
    focgold_wounds wounds = mobile_wounds();
    focgold_turn_permissions permissions;

    expect_int(focgold_movement_after_one_grenade_inches(
                   focgold_movement_very_slow,
                   &no_skills,
                   &wounds),
               1,
               "one grenade should halve very slow movement to 1 inch");
    expect_int(focgold_movement_after_one_grenade_inches(
                   focgold_movement_slow,
                   &no_skills,
                   &wounds),
               1,
               "one grenade should halve slow movement to 1 inch");
    expect_int(focgold_movement_after_one_grenade_inches(
                   focgold_movement_standard,
                   &no_skills,
                   &wounds),
               2,
               "one grenade should halve standard movement to 2 inches");
    expect_int(focgold_movement_after_one_grenade_inches(
                   focgold_movement_fast,
                   &no_skills,
                   &wounds),
               4,
               "one grenade should halve fast movement to 4 inches");

    expect_true(!focgold_can_throw_two_grenades(false),
                "moving characters should not throw two grenades");
    expect_true(focgold_can_throw_two_grenades(true),
                "stationary characters should be able to throw two grenades");

    permissions = focgold_two_grenade_turn_permissions();
    expect_true(!permissions.can_move,
                "two-grenade turn should prevent movement");
    expect_true(!permissions.can_firearm_attack,
                "two-grenade turn should prevent firearm attacks");
    expect_true(!permissions.can_melee_attack,
                "two-grenade turn should prevent melee attacks");
    expect_true(!permissions.can_reload,
                "two-grenade turn should prevent reloads");
    expect_true(!permissions.can_clear_jam,
                "two-grenade turn should prevent jam clearing");
    expect_true(!permissions.can_heal,
                "two-grenade turn should prevent healing");
    expect_true(!permissions.can_progress_called_head_shot,
                "two-grenade turn should prevent called-head-shot progress");
}

int main(void) {
    test_ranged_modifiers();
    test_called_head_shots();
    test_movement();
    test_weapon_movement_rules();
    test_grenade_action_economy();

    if (failures != 0) {
        fprintf(stderr, "%d failure(s)\n", failures);
        return 1;
    }

    printf("gold action tests passed\n");
    return 0;
}
