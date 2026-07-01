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

static void expect_location(focgold_hit_location location,
                            const char *message) {
    expect_true(location >= focgold_hit_left_leg && location <= focgold_hit_head,
                message);
}

static void expect_damage_locations(const focgold_area_damage *damage,
                                    const char *message) {
    int index;

    for (index = 0; index < damage->wounds_inflicted; index++) {
        expect_location(damage->hit_locations[index], message);
    }
}

static void test_shotgun_nearest_band_and_allies(void) {
    focgold_area_target targets[] = {
        {1, false, 7, 0},
        {2, true, 8, 1},
        {3, false, 13, 0},
        {4, false, 4, 2}
    };
    focgold_area_damage results[4];
    size_t affected = 99;

    focgold_seed(501u);
    expect_true(focgold_resolve_shotgun_area(targets,
                                             4,
                                             results,
                                             4,
                                             &affected),
                "shotgun area should resolve");
    expect_int((int)affected,
               2,
               "shotgun should affect only the nearest occupied cone band");
    expect_true(results[0].affected,
                "enemy in nearest occupied band should be affected");
    expect_true(results[1].affected,
                "ally in nearest occupied band should be affected");
    expect_true(results[1].ally,
                "ally flag should be preserved in shotgun results");
    expect_true(!results[2].affected,
                "farther occupied shotgun bands should be ignored");
    expect_true(!results[3].affected,
                "target outside the cone width should be ignored");
    expect_true(results[0].shotgun_band == focgold_shotgun_middle,
                "first affected target should be in middle shotgun band");
    expect_true(results[1].shotgun_band == focgold_shotgun_middle,
                "second affected target should be in middle shotgun band");
    expect_true(results[0].wounds_inflicted >= 1 &&
                    results[0].wounds_inflicted <= 6,
                "middle shotgun band should inflict d6 wounds");
    expect_true(results[1].wounds_inflicted >= 1 &&
                    results[1].wounds_inflicted <= 6,
                "ally should receive middle-band d6 wounds");
    expect_damage_locations(&results[0],
                            "shotgun wounds should use 3d6 hit locations");
    expect_damage_locations(&results[1],
                            "ally shotgun wounds should use 3d6 hit locations");
}

static void test_shotgun_near_band_blocks_farther_bands(void) {
    focgold_area_target targets[] = {
        {10, false, 5, 1},
        {11, false, 6, 0},
        {12, false, 12, 0}
    };
    focgold_area_damage results[3];
    size_t affected = 0;

    focgold_seed(502u);
    expect_true(focgold_resolve_shotgun_area(targets,
                                             3,
                                             results,
                                             3,
                                             &affected),
                "shotgun with near target should resolve");
    expect_int((int)affected,
               1,
               "nearest occupied near band should block middle and far bands");
    expect_true(results[0].affected,
                "near-band shotgun target should be affected");
    expect_true(results[0].shotgun_band == focgold_shotgun_near,
                "near target should report near shotgun band");
    expect_true(results[0].wounds_inflicted >= 2 &&
                    results[0].wounds_inflicted <= 12,
                "near shotgun band should inflict 2d6 wounds");
    expect_true(!results[1].affected,
                "middle band should be ignored when near band is occupied");
    expect_true(!results[2].affected,
                "far band should be ignored when near band is occupied");
}

static void test_grenade_open_area_and_allies(void) {
    focgold_area_target targets[] = {
        {20, false, 1, 0},
        {21, true, 3, 0},
        {22, false, 4, 0},
        {23, false, 5, 0}
    };
    focgold_area_damage results[4];
    size_t affected = 0;

    focgold_seed(601u);
    expect_true(focgold_resolve_grenade_area(targets,
                                             4,
                                             false,
                                             false,
                                             results,
                                             4,
                                             &affected),
                "open grenade area should resolve");
    expect_int((int)affected,
               3,
               "open grenade should affect models inside 1/3/4 inch bands");
    expect_true(results[0].grenade_band == focgold_grenade_inner,
                "one inch target should use inner grenade band");
    expect_true(results[1].grenade_band == focgold_grenade_middle,
                "three inch ally should use middle grenade band");
    expect_true(results[1].ally,
                "ally flag should be preserved in grenade results");
    expect_true(results[2].grenade_band == focgold_grenade_outer,
                "four inch target should use outer grenade band");
    expect_true(!results[3].affected,
                "target outside open grenade radius should be ignored");
    expect_true(results[0].wounds_inflicted >= 1 &&
                    results[0].wounds_inflicted <= 6,
                "inner grenade band should inflict d6 wounds");
    expect_true(results[1].wounds_inflicted >= 1 &&
                    results[1].wounds_inflicted <= 3,
                "middle grenade band should inflict d3 wounds");
    expect_true(results[2].wounds_inflicted >= 1 &&
                    results[2].wounds_inflicted <= 2,
                "outer grenade band should inflict d2 wounds");
    expect_damage_locations(&results[0],
                            "grenade wounds should use 3d6 hit locations");
    expect_damage_locations(&results[1],
                            "ally grenade wounds should use 3d6 hit locations");
    expect_damage_locations(&results[2],
                            "outer grenade wounds should use 3d6 hit locations");
}

static void test_grenade_building_radius_and_duds(void) {
    focgold_area_target targets[] = {
        {30, false, 8, 0},
        {31, false, 10, 0}
    };
    focgold_area_damage results[2];
    size_t affected = 0;
    int expected_next_roll;
    int actual_next_roll;

    focgold_seed(602u);
    expect_true(focgold_resolve_grenade_area(targets,
                                             2,
                                             true,
                                             false,
                                             results,
                                             2,
                                             &affected),
                "building grenade area should resolve");
    expect_int((int)affected,
               1,
               "building grenade should use expanded 3/6/9 inch radii");
    expect_true(results[0].affected,
                "eight inch building target should be affected");
    expect_true(results[0].grenade_band == focgold_grenade_outer,
                "eight inch building target should be in outer band");
    expect_true(!results[1].affected,
                "ten inch building target should be outside blast radius");

    focgold_seed(603u);
    expected_next_roll = focgold_roll_d6();
    focgold_seed(603u);
    expect_true(focgold_resolve_grenade_area(targets,
                                             2,
                                             true,
                                             true,
                                             results,
                                             2,
                                             &affected),
                "dud grenade should still resolve to no damage");
    actual_next_roll = focgold_roll_d6();
    expect_int((int)affected,
               0,
               "dud grenade should affect no targets");
    expect_true(!results[0].affected && !results[1].affected,
                "dud grenade should mark all targets unaffected");
    expect_int(actual_next_roll,
               expected_next_roll,
               "dud grenade should not consume damage or hit-location rolls");
}

static void test_area_input_validation(void) {
    focgold_area_target target = {1, false, 1, 0};
    focgold_area_damage result;
    focgold_shotgun_band shotgun_band = focgold_shotgun_band_count;
    focgold_grenade_band grenade_band = focgold_grenade_band_count;

    expect_true(focgold_shotgun_band_for_target(5,
                                                1,
                                                &shotgun_band),
                "shotgun target at edge of near band should classify");
    expect_true(shotgun_band == focgold_shotgun_near,
                "shotgun near edge should be near band");
    expect_true(!focgold_shotgun_band_for_target(16,
                                                 0,
                                                 &shotgun_band),
                "shotgun target beyond 15 inches should not classify");
    expect_true(focgold_grenade_band_for_distance(9,
                                                  true,
                                                  &grenade_band),
                "building grenade target at 9 inches should classify");
    expect_true(grenade_band == focgold_grenade_outer,
                "building grenade 9 inch target should be outer band");
    expect_true(!focgold_grenade_band_for_distance(10,
                                                   true,
                                                   &grenade_band),
                "building grenade target beyond 9 inches should not classify");
    expect_true(!focgold_resolve_shotgun_area(&target,
                                              1,
                                              &result,
                                              0,
                                              0),
                "shotgun area should reject too-small result buffers");
    expect_true(!focgold_resolve_grenade_area(&target,
                                              1,
                                              false,
                                              false,
                                              &result,
                                              0,
                                              0),
                "grenade area should reject too-small result buffers");
}

int main(void) {
    test_shotgun_nearest_band_and_allies();
    test_shotgun_near_band_blocks_farther_bands();
    test_grenade_open_area_and_allies();
    test_grenade_building_radius_and_duds();
    test_area_input_validation();

    if (failures != 0) {
        fprintf(stderr, "%d failure(s)\n", failures);
        return 1;
    }

    printf("area effect tests passed\n");
    return 0;
}
