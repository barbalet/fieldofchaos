#include "fieldofchaos2018.h"

#include <stdio.h>
#include <string.h>

static int failures = 0;

static void expect_true(int condition, const char *message) {
    if (!condition) {
        fprintf(stderr, "FAIL: %s\n", message);
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

static void test_2018_weapon_names(void) {
    expect_true(foc2018_weapon_count == 5,
                "2018 core weapon list should contain five weapons");
    expect_string(foc2018_weapon_name(foc2018_weapon_sniper_rifle),
                  "Sniper Rifle",
                  "sniper rifle should be named");
    expect_string(foc2018_weapon_name(foc2018_weapon_rifle),
                  "Rifle",
                  "rifle should be named");
    expect_string(foc2018_weapon_name(foc2018_weapon_carbine),
                  "Carbine",
                  "carbine should be named");
    expect_string(foc2018_weapon_name(foc2018_weapon_automatic),
                  "Automatic",
                  "automatic should be named");
    expect_string(foc2018_weapon_name(foc2018_weapon_submg),
                  "SubMG",
                  "submg should be named");
    expect_true(foc2018_weapon_name(foc2018_weapon_count) == 0,
                "weapon count should not name a real weapon");
}

static void test_weapon_round_trips(void) {
    int index;

    for (index = 0; index < foc2018_weapon_count; index++) {
        foc2018_weapon expected = (foc2018_weapon)index;
        foc2018_weapon actual = foc2018_weapon_count;
        const char *name = foc2018_weapon_name(expected);

        expect_true(foc2018_parse_weapon(name, &actual),
                    "weapon name should parse");
        expect_true(actual == expected,
                    "weapon name should round-trip to original enum");
    }
}

static void test_non_2018_core_weapons_do_not_parse(void) {
    foc2018_weapon weapon = foc2018_weapon_rifle;

    expect_true(!foc2018_parse_weapon("Shotgun", &weapon),
                "shotgun should not parse as a 2018 core weapon");
    expect_true(weapon == foc2018_weapon_rifle,
                "failed parse should not change output weapon");
    expect_true(!foc2018_parse_weapon("Grenade", &weapon),
                "grenade should not parse as a 2018 core weapon");
    expect_true(!foc2018_parse_weapon(0, &weapon),
                "missing weapon name should fail");
    expect_true(!foc2018_parse_weapon("Rifle", 0),
                "missing output weapon should fail");
}

int main(void) {
    test_2018_weapon_names();
    test_weapon_round_trips();
    test_non_2018_core_weapons_do_not_parse();

    if (failures != 0) {
        fprintf(stderr, "%d failure(s)\n", failures);
        return 1;
    }

    printf("weapon tests passed\n");
    return 0;
}
