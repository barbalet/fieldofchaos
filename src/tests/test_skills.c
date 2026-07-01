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

static void expect_prerequisites(foc2018_skill_id skill,
                                 const foc2018_skill_id *expected,
                                 size_t expected_count,
                                 const char *message) {
    size_t count = 99;
    const foc2018_skill_id *actual;
    size_t index;

    actual = foc2018_skill_prerequisites(skill, &count);
    if (count != expected_count || (expected_count == 0 && actual != 0) ||
        (expected_count != 0 && actual == 0)) {
        fprintf(stderr, "FAIL: %s\n", message);
        failures++;
        return;
    }

    for (index = 0; index < expected_count; index++) {
        if (actual[index] != expected[index]) {
            fprintf(stderr, "FAIL: %s\n", message);
            failures++;
            return;
        }
    }
}

static void test_stat_names(void) {
    expect_string(foc2018_stat_name(foc2018_stat_regular_intelligence),
                  "Regular Intelligence",
                  "RE should have a public name");
    expect_string(foc2018_stat_name(foc2018_stat_irregular_intelligence),
                  "Irregular Intelligence",
                  "IR should have a public name");
    expect_string(foc2018_stat_name(foc2018_stat_appearance),
                  "Appearance",
                  "AP should have a public name");
    expect_string(foc2018_stat_name(foc2018_stat_physical_health),
                  "Physical Health",
                  "PH should have a public name");
    expect_string(foc2018_stat_name(foc2018_stat_mental_health),
                  "Mental Health",
                  "ME should have a public name");
    expect_true(foc2018_stat_name(foc2018_stat_count) == 0,
                "stat count should not name a real stat");
}

static void test_weapon_skill_metadata(void) {
    foc2018_skill_id firearm_advanced[] = {
        foc2018_skill_firearm_basic
    };

    expect_string(foc2018_skill_name(foc2018_skill_firearm_basic),
                  "Firearm Use, Basic",
                  "basic firearm skill should be named");
    expect_string(foc2018_skill_name(foc2018_skill_firearm_advanced),
                  "Firearm Use, Advanced",
                  "advanced firearm skill should be named");
    expect_true(foc2018_skill_stat(foc2018_skill_firearm_basic) ==
                    foc2018_stat_physical_health,
                "basic firearm should use PH");
    expect_true(foc2018_skill_stat(foc2018_skill_firearm_advanced) ==
                    foc2018_stat_mental_health,
                "advanced firearm should use ME");
    expect_prerequisites(foc2018_skill_firearm_basic, 0, 0,
                         "basic firearm should have no prerequisite");
    expect_prerequisites(foc2018_skill_firearm_advanced,
                         firearm_advanced,
                         1,
                         "advanced firearm should require basic firearm");

    expect_string(foc2018_skill_name(foc2018_skill_close_combat),
                  "Close Combat",
                  "close combat should be named");
    expect_true(foc2018_skill_stat(foc2018_skill_close_combat) ==
                    foc2018_stat_physical_health,
                "close combat should use PH");

    expect_string(foc2018_skill_name(foc2018_skill_improvised_weapon_basic),
                  "Improvised Weapon Use, Basic",
                  "improvised weapon skill should be named");
    expect_true(foc2018_skill_stat(foc2018_skill_improvised_weapon_basic) ==
                    foc2018_stat_irregular_intelligence,
                "improvised weapon should use IR");

    expect_string(foc2018_skill_name(foc2018_skill_clandestine_weapon_basic),
                  "Clandestine Weapon Use, Basic",
                  "clandestine weapon skill should be named");
    expect_true(foc2018_skill_stat(foc2018_skill_clandestine_weapon_basic) ==
                    foc2018_stat_irregular_intelligence,
                "clandestine weapon should use IR");
}

static void test_medical_and_chemistry_metadata(void) {
    foc2018_skill_id paramedic[] = {
        foc2018_skill_first_aid
    };
    foc2018_skill_id chemistry_advanced[] = {
        foc2018_skill_biology_basic,
        foc2018_skill_chemistry_basic
    };
    foc2018_skill_id chemistry_pharmaceutical[] = {
        foc2018_skill_chemistry_advanced
    };

    expect_string(foc2018_skill_name(foc2018_skill_first_aid),
                  "First Aid",
                  "first aid should be named");
    expect_string(foc2018_skill_name(foc2018_skill_paramedic),
                  "Paramedic",
                  "paramedic should be named");
    expect_true(foc2018_skill_stat(foc2018_skill_first_aid) ==
                    foc2018_stat_regular_intelligence,
                "first aid should use RE");
    expect_true(foc2018_skill_stat(foc2018_skill_paramedic) ==
                    foc2018_stat_regular_intelligence,
                "paramedic should use RE");
    expect_prerequisites(foc2018_skill_paramedic,
                         paramedic,
                         1,
                         "paramedic should require first aid");
    expect_true(foc2018_skill_improves_healing(foc2018_skill_first_aid),
                "first aid should improve healing");
    expect_true(foc2018_skill_improves_healing(foc2018_skill_paramedic),
                "paramedic should improve healing");

    expect_prerequisites(foc2018_skill_chemistry_advanced,
                         chemistry_advanced,
                         2,
                         "advanced chemistry should require biology and basic chemistry");
    expect_prerequisites(foc2018_skill_chemistry_pharmaceutical,
                         chemistry_pharmaceutical,
                         1,
                         "pharmaceutical chemistry should require advanced chemistry");
    expect_true(foc2018_skill_stat(foc2018_skill_chemistry_pharmaceutical) ==
                    foc2018_stat_regular_intelligence,
                "pharmaceutical chemistry should use RE");
    expect_true(!foc2018_skill_improves_healing(
                    foc2018_skill_chemistry_pharmaceutical),
                "pharmaceutical chemistry should not improve 2018 healing");
}

static void test_skill_percentages_and_lookup(void) {
    foc2018_skills skills = {0};

    expect_true(foc2018_validate_skills(&skills),
                "zeroed skills should be valid percentages");
    skills.firearm_basic = 100;
    skills.first_aid = 45;
    skills.paramedic = 1;
    expect_true(foc2018_validate_skills(&skills),
                "skills from 0 to 100 should be valid");
    expect_true(foc2018_skill_value(&skills, foc2018_skill_firearm_basic) == 100,
                "skill lookup should return firearm basic value");
    expect_true(foc2018_skill_value(&skills, foc2018_skill_first_aid) == 45,
                "skill lookup should return first aid value");

    skills.paramedic = 101;
    expect_true(!foc2018_validate_skills(&skills),
                "skill percentages above 100 should be invalid");
    skills.paramedic = -1;
    expect_true(!foc2018_validate_skills(&skills),
                "skill percentages below 0 should be invalid");

    expect_true(!foc2018_validate_skills(0),
                "missing skills should be invalid");
    expect_true(foc2018_skill_name(foc2018_skill_count) == 0,
                "skill count should not name a real skill");
    expect_true(foc2018_skill_stat(foc2018_skill_count) ==
                    foc2018_stat_count,
                "skill count should not map to a real stat");
    expect_true(foc2018_skill_value(0, foc2018_skill_first_aid) == 0,
                "missing skills should report zero value");
}

int main(void) {
    test_stat_names();
    test_weapon_skill_metadata();
    test_medical_and_chemistry_metadata();
    test_skill_percentages_and_lookup();

    if (failures != 0) {
        fprintf(stderr, "%d failure(s)\n", failures);
        return 1;
    }

    printf("skill tests passed\n");
    return 0;
}
