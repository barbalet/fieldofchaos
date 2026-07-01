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

static void expect_location(focgold_hit_location actual,
                            focgold_hit_location expected,
                            const char *message) {
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

static void test_forced_3d6_locations(void) {
    expect_location(focgold_hit_location_for_3d6_total(3),
                    focgold_hit_left_leg,
                    "3 should hit an odd-side leg");
    expect_location(focgold_hit_location_for_3d6_total(4),
                    focgold_hit_right_leg,
                    "4 should hit an even-side leg");
    expect_location(focgold_hit_location_for_3d6_total(8),
                    focgold_hit_right_leg,
                    "8 should still map to leg");
    expect_location(focgold_hit_location_for_3d6_total(9),
                    focgold_hit_chest,
                    "9 should map to chest");
    expect_location(focgold_hit_location_for_3d6_total(13),
                    focgold_hit_chest,
                    "13 should map to chest");
    expect_location(focgold_hit_location_for_3d6_total(14),
                    focgold_hit_abdomen,
                    "14 should map to abdomen");
    expect_location(focgold_hit_location_for_3d6_total(15),
                    focgold_hit_abdomen,
                    "15 should map to abdomen");
    expect_location(focgold_hit_location_for_3d6_total(16),
                    focgold_hit_right_arm,
                    "16 should hit an even-side arm");
    expect_location(focgold_hit_location_for_3d6_total(17),
                    focgold_hit_left_arm,
                    "17 should hit an odd-side arm");
    expect_location(focgold_hit_location_for_3d6_total(18),
                    focgold_hit_head,
                    "18 should map to head");
}

static void test_location_names(void) {
    expect_string(focgold_hit_location_name(focgold_hit_left_leg),
                  "Left Leg",
                  "left leg should have a name");
    expect_string(focgold_hit_location_name(focgold_hit_right_leg),
                  "Right Leg",
                  "right leg should have a name");
    expect_string(focgold_hit_location_name(focgold_hit_chest),
                  "Chest",
                  "chest should have a name");
    expect_string(focgold_hit_location_name(focgold_hit_abdomen),
                  "Abdomen",
                  "abdomen should have a name");
    expect_string(focgold_hit_location_name(focgold_hit_left_arm),
                  "Left Arm",
                  "left arm should have a name");
    expect_string(focgold_hit_location_name(focgold_hit_right_arm),
                  "Right Arm",
                  "right arm should have a name");
    expect_string(focgold_hit_location_name(focgold_hit_head),
                  "Head",
                  "head should have a name");
    expect_true(focgold_hit_location_name((focgold_hit_location)99) == 0,
                "invalid hit location should not have a name");
}

static void test_random_bullet_and_blast_locations(void) {
    int total;

    focgold_seed(113u);
    total = focgold_roll_d6() + focgold_roll_d6() + focgold_roll_d6();
    focgold_seed(113u);
    expect_location(focgold_roll_bullet_hit_location(),
                    focgold_hit_location_for_3d6_total(total),
                    "bullet hit location should use 3d6");

    focgold_seed(209u);
    total = focgold_roll_d6() + focgold_roll_d6() + focgold_roll_d6();
    focgold_seed(209u);
    expect_location(focgold_roll_blast_hit_location(),
                    focgold_hit_location_for_3d6_total(total),
                    "shotgun and grenade blast wounds should use 3d6 locations");
}

int main(void) {
    test_forced_3d6_locations();
    test_location_names();
    test_random_bullet_and_blast_locations();

    if (failures != 0) {
        fprintf(stderr, "%d failure(s)\n", failures);
        return 1;
    }

    printf("hit location tests passed\n");
    return 0;
}
