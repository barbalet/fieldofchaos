#include "fieldofchaosgold.h"

#include <stdio.h>
#include <string.h>

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

static void expect_string(const char *actual,
                          const char *expected,
                          const char *message) {
    if (actual == 0 || strcmp(actual, expected) != 0) {
        fprintf(stderr, "FAIL: %s\n", message);
        failures++;
    }
}

static void expect_damage(focgold_damage_roll actual,
                          int dice_count,
                          int die_sides,
                          const char *message) {
    if (actual.dice_count != dice_count || actual.die_sides != die_sides) {
        fprintf(stderr, "FAIL: %s: got %dd%d expected %dd%d\n",
                message,
                actual.dice_count,
                actual.die_sides,
                dice_count,
                die_sides);
        failures++;
    }
}

static void test_gold_weapon_names(void) {
    expect_int(focgold_weapon_count, 6,
               "Gold firearm weapon list should contain six weapons");
    expect_string(focgold_weapon_name(focgold_weapon_sniper_rifle),
                  "Sniper Rifle",
                  "sniper rifle should be named");
    expect_string(focgold_weapon_name(focgold_weapon_rifle),
                  "Rifle",
                  "rifle should be named");
    expect_string(focgold_weapon_name(focgold_weapon_carbine),
                  "Carbine",
                  "carbine should be named");
    expect_string(focgold_weapon_name(focgold_weapon_automatic),
                  "Automatic",
                  "automatic should be named");
    expect_string(focgold_weapon_name(focgold_weapon_submg),
                  "SubMG",
                  "submg should be named");
    expect_string(focgold_weapon_name(focgold_weapon_shotgun),
                  "Shotgun",
                  "shotgun should be named");
    expect_true(focgold_weapon_name(focgold_weapon_count) == 0,
                "weapon count should not name a real weapon");
}

static void test_weapon_round_trips(void) {
    int index;

    for (index = 0; index < focgold_weapon_count; index++) {
        focgold_weapon expected = (focgold_weapon)index;
        focgold_weapon actual = focgold_weapon_count;
        const char *name = focgold_weapon_name(expected);

        expect_true(focgold_parse_weapon(name, &actual),
                    "weapon name should parse");
        expect_true(actual == expected,
                    "weapon name should round-trip to original enum");
    }
}

static void test_grenade_is_not_a_standard_weapon(void) {
    focgold_weapon weapon = focgold_weapon_rifle;

    expect_true(!focgold_parse_weapon("Grenade", &weapon),
                "grenade should not parse as a standard firearm weapon");
    expect_true(weapon == focgold_weapon_rifle,
                "failed parse should not change output weapon");
    expect_true(!focgold_parse_weapon(0, &weapon),
                "missing weapon name should fail");
    expect_true(!focgold_parse_weapon("Rifle", 0),
                "missing output weapon should fail");
}

static void test_firearm_ranges(void) {
    expect_int(focgold_weapon_range_inches(focgold_weapon_sniper_rifle,
                                           focgold_range_close),
               24,
               "sniper close range should be 24 inches");
    expect_int(focgold_weapon_range_inches(focgold_weapon_sniper_rifle,
                                           focgold_range_standard),
               36,
               "sniper standard range should be 36 inches");
    expect_int(focgold_weapon_range_inches(focgold_weapon_sniper_rifle,
                                           focgold_range_long),
               48,
               "sniper long range should be 48 inches");

    expect_int(focgold_weapon_range_inches(focgold_weapon_rifle,
                                           focgold_range_close),
               18,
               "rifle close range should be 18 inches");
    expect_int(focgold_weapon_range_inches(focgold_weapon_rifle,
                                           focgold_range_standard),
               27,
               "rifle standard range should be 27 inches");
    expect_int(focgold_weapon_range_inches(focgold_weapon_rifle,
                                           focgold_range_long),
               36,
               "rifle long range should be 36 inches");

    expect_int(focgold_weapon_range_inches(focgold_weapon_carbine,
                                           focgold_range_close),
               12,
               "carbine close range should be 12 inches");
    expect_int(focgold_weapon_range_inches(focgold_weapon_carbine,
                                           focgold_range_standard),
               18,
               "carbine standard range should be 18 inches");
    expect_int(focgold_weapon_range_inches(focgold_weapon_carbine,
                                           focgold_range_long),
               24,
               "carbine long range should be 24 inches");

    expect_int(focgold_weapon_range_inches(focgold_weapon_automatic,
                                           focgold_range_close),
               8,
               "automatic close range should be 8 inches");
    expect_int(focgold_weapon_range_inches(focgold_weapon_automatic,
                                           focgold_range_standard),
               12,
               "automatic standard range should be 12 inches");
    expect_int(focgold_weapon_range_inches(focgold_weapon_automatic,
                                           focgold_range_long),
               16,
               "automatic long range should be 16 inches");

    expect_int(focgold_weapon_range_inches(focgold_weapon_submg,
                                           focgold_range_close),
               6,
               "SubMG close range should be 6 inches");
    expect_int(focgold_weapon_range_inches(focgold_weapon_submg,
                                           focgold_range_standard),
               9,
               "SubMG standard range should be 9 inches");
    expect_int(focgold_weapon_range_inches(focgold_weapon_submg,
                                           focgold_range_long),
               12,
               "SubMG long range should be 12 inches");

    expect_int(focgold_weapon_range_inches(focgold_weapon_shotgun,
                                           focgold_range_long),
               0,
               "shotgun should use cone geometry rather than range bands");
}

static void test_ranged_thresholds(void) {
    expect_int(focgold_ranged_hit_threshold(focgold_range_close),
               4,
               "close hit threshold should be 4");
    expect_int(focgold_ranged_hit_threshold(focgold_range_standard),
               10,
               "standard hit threshold should be 10");
    expect_int(focgold_ranged_hit_threshold(focgold_range_long),
               16,
               "long hit threshold should be 16");
    expect_int(focgold_ranged_head_shot_threshold(focgold_range_close),
               6,
               "close head-shot threshold should be 6");
    expect_int(focgold_ranged_head_shot_threshold(focgold_range_standard),
               12,
               "standard head-shot threshold should be 12");
    expect_int(focgold_ranged_head_shot_threshold(focgold_range_long),
               18,
               "long head-shot threshold should be 18");
}

static void test_shotgun_geometry(void) {
    expect_true(focgold_weapon_uses_firearm_basic(focgold_weapon_shotgun),
                "shotgun should be covered by Firearm Use, Basic");
    expect_int(focgold_shotgun_total_length_inches(),
               15,
               "shotgun cone length should be 15 inches");
    expect_int(focgold_shotgun_band_max_inches(focgold_shotgun_near),
               5,
               "near shotgun band should end at 5 inches");
    expect_int(focgold_shotgun_band_width_inches(focgold_shotgun_near),
               1,
               "near shotgun band should be 1 inch wide");
    expect_damage(focgold_shotgun_band_wounds(focgold_shotgun_near),
                  2,
                  6,
                  "near shotgun band should inflict 2d6 wounds");
    expect_int(focgold_shotgun_band_max_inches(focgold_shotgun_middle),
               10,
               "middle shotgun band should end at 10 inches");
    expect_int(focgold_shotgun_band_width_inches(focgold_shotgun_middle),
               2,
               "middle shotgun band should be 2 inches wide");
    expect_damage(focgold_shotgun_band_wounds(focgold_shotgun_middle),
                  1,
                  6,
                  "middle shotgun band should inflict d6 wounds");
    expect_int(focgold_shotgun_band_max_inches(focgold_shotgun_far),
               15,
               "far shotgun band should end at 15 inches");
    expect_int(focgold_shotgun_band_width_inches(focgold_shotgun_far),
               3,
               "far shotgun band should be 3 inches wide");
    expect_damage(focgold_shotgun_band_wounds(focgold_shotgun_far),
                  1,
                  3,
                  "far shotgun band should inflict d3 wounds");
}

static void test_grenade_geometry_and_reliability(void) {
    focgold_grenade_reliability_result reliability;
    int expected_total;

    expect_int(focgold_grenade_throw_range_inches(),
               12,
               "grenade throw range should be 12 inches");
    expect_int(focgold_grenade_radius_inches(focgold_grenade_inner, false),
               1,
               "inner grenade radius should be 1 inch");
    expect_damage(focgold_grenade_band_wounds(focgold_grenade_inner),
                  1,
                  6,
                  "inner grenade band should inflict d6 wounds");
    expect_int(focgold_grenade_radius_inches(focgold_grenade_middle, false),
               3,
               "middle grenade radius should be 3 inches");
    expect_damage(focgold_grenade_band_wounds(focgold_grenade_middle),
                  1,
                  3,
                  "middle grenade band should inflict d3 wounds");
    expect_int(focgold_grenade_radius_inches(focgold_grenade_outer, false),
               4,
               "outer grenade radius should be 4 inches");
    expect_damage(focgold_grenade_band_wounds(focgold_grenade_outer),
                  1,
                  2,
                  "outer grenade band should inflict d2 wounds");
    expect_int(focgold_grenade_radius_inches(focgold_grenade_inner, true),
               3,
               "building inner grenade radius should be 3 inches");
    expect_int(focgold_grenade_radius_inches(focgold_grenade_middle, true),
               6,
               "building middle grenade radius should be 6 inches");
    expect_int(focgold_grenade_radius_inches(focgold_grenade_outer, true),
               9,
               "building outer grenade radius should be 9 inches");

    expect_true(!focgold_grenade_uses_skill(),
                "grenade should have no skill requirement");
    expect_true(!focgold_grenade_skill_modifies_reliability(
                    focgold_skill_explosive_engineering),
                "Explosive Engineering should not modify grenade reliability");
    expect_true(!focgold_grenade_skill_modifies_reliability(
                    focgold_skill_firearm_basic),
                "Firearm Use should not modify grenade reliability");

    focgold_seed(77u);
    expect_true(focgold_resolve_grenade_reliability(&reliability),
                "grenade reliability should resolve");
    focgold_seed(77u);
    expected_total = focgold_roll_d6() + focgold_roll_d6();
    expect_int(reliability.dice_total,
               expected_total,
               "grenade reliability should roll 2d6");
    expect_true(reliability.dud == (expected_total <= focgold_grenade_dud_threshold),
                "grenade should be a dud on 4 or less");
    expect_true(!focgold_resolve_grenade_reliability(0),
                "missing grenade reliability result should fail");
}

static void test_fire_rates_and_jams(void) {
    bool jammed = true;

    expect_int(focgold_standard_clip_rounds,
               10,
               "standard clip size should be 10 rounds");
    expect_int(focgold_reload_turn_count(),
               1,
               "changing a clip should take 1 turn");
    expect_int(focgold_clear_jam_turn_count(),
               1,
               "clearing a jam should take 1 turn");
    expect_int(focgold_weapon_shots_per_turn(focgold_weapon_rifle, false),
               1,
               "rifle without Firearm Advanced should fire 1 shot");
    expect_int(focgold_weapon_shots_per_turn(focgold_weapon_rifle, true),
               2,
               "rifle with Firearm Advanced should fire 2 shots");
    expect_int(focgold_weapon_shots_per_turn(focgold_weapon_carbine, true),
               2,
               "carbine with Firearm Advanced should fire 2 shots");
    expect_int(focgold_weapon_shots_per_turn(focgold_weapon_shotgun, true),
               1,
               "shotgun should fire 1 cone attack");
    expect_int(focgold_weapon_shots_per_turn(focgold_weapon_automatic, false),
               2,
               "automatic should fire 2 shots");
    expect_int(focgold_weapon_shots_per_turn(focgold_weapon_submg, false),
               3,
               "SubMG should fire 3 shots");

    expect_true(focgold_weapon_can_head_shot(focgold_weapon_rifle),
                "rifle should allow head shots");
    expect_true(!focgold_weapon_can_head_shot(focgold_weapon_automatic),
                "automatic should not allow head shots");
    expect_true(!focgold_weapon_can_head_shot(focgold_weapon_submg),
                "SubMG should not allow head shots");

    expect_int(focgold_weapon_jam_dice_count(focgold_weapon_automatic),
               2,
               "automatic should use 2d6 for jams");
    expect_int(focgold_weapon_jam_threshold(focgold_weapon_automatic),
               1,
               "automatic should jam on 1");
    expect_int(focgold_weapon_jam_dice_count(focgold_weapon_submg),
               1,
               "SubMG should use 1d6 for jams");
    expect_int(focgold_weapon_jam_threshold(focgold_weapon_submg),
               1,
               "SubMG should jam on 1");
    expect_true(focgold_resolve_weapon_jam(focgold_weapon_rifle, &jammed),
                "non-jamming weapon should still resolve jam metadata");
    expect_true(!jammed, "rifle should not jam");
    expect_true(!focgold_resolve_weapon_jam(focgold_weapon_rifle, 0),
                "missing jam output should fail");
}

int main(void) {
    test_gold_weapon_names();
    test_weapon_round_trips();
    test_grenade_is_not_a_standard_weapon();
    test_firearm_ranges();
    test_ranged_thresholds();
    test_shotgun_geometry();
    test_grenade_geometry_and_reliability();
    test_fire_rates_and_jams();

    if (failures != 0) {
        fprintf(stderr, "%d failure(s)\n", failures);
        return 1;
    }

    printf("weapon tests passed\n");
    return 0;
}
