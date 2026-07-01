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

static void expect_string(const char *actual,
                          const char *expected,
                          const char *message) {
    if (actual == 0 || strcmp(actual, expected) != 0) {
        fprintf(stderr, "FAIL: %s\n", message);
        failures++;
    }
}

static void expect_prerequisites(focgold_skill_id skill,
                                 const focgold_skill_id *expected,
                                 size_t expected_count,
                                 const char *message) {
    size_t count = 99;
    const focgold_skill_id *actual;
    size_t index;

    actual = focgold_skill_prerequisites(skill, &count);
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
    expect_string(focgold_stat_name(focgold_stat_regular_intelligence),
                  "Regular Intelligence",
                  "RE should have a public name");
    expect_string(focgold_stat_name(focgold_stat_irregular_intelligence),
                  "Irregular Intelligence",
                  "IR should have a public name");
    expect_string(focgold_stat_name(focgold_stat_appearance),
                  "Appearance",
                  "AP should have a public name");
    expect_string(focgold_stat_name(focgold_stat_physical_health),
                  "Physical Health",
                  "PH should have a public name");
    expect_string(focgold_stat_name(focgold_stat_mental_health),
                  "Mental Health",
                  "ME should have a public name");
    expect_true(focgold_stat_name(focgold_stat_count) == 0,
                "stat count should not name a real stat");
}

static void test_weapon_skill_metadata(void) {
    focgold_skill_id firearm_advanced[] = {
        focgold_skill_firearm_basic
    };

    expect_string(focgold_skill_name(focgold_skill_firearm_basic),
                  "Firearm Use, Basic",
                  "basic firearm skill should be named");
    expect_string(focgold_skill_name(focgold_skill_firearm_advanced),
                  "Firearm Use, Advanced",
                  "advanced firearm skill should be named");
    expect_true(focgold_skill_stat(focgold_skill_firearm_basic) ==
                    focgold_stat_physical_health,
                "basic firearm should use PH");
    expect_true(focgold_skill_stat(focgold_skill_firearm_advanced) ==
                    focgold_stat_mental_health,
                "advanced firearm should use ME");
    expect_prerequisites(focgold_skill_firearm_basic, 0, 0,
                         "basic firearm should have no prerequisite");
    expect_prerequisites(focgold_skill_firearm_advanced,
                         firearm_advanced,
                         1,
                         "advanced firearm should require basic firearm");

    expect_string(focgold_skill_name(focgold_skill_close_combat),
                  "Close Combat",
                  "close combat should be named");
    expect_true(focgold_skill_stat(focgold_skill_close_combat) ==
                    focgold_stat_physical_health,
                "close combat should use PH");

    expect_string(focgold_skill_name(focgold_skill_improvised_weapon_basic),
                  "Improvised Weapon Use, Basic",
                  "improvised weapon skill should be named");
    expect_true(focgold_skill_stat(focgold_skill_improvised_weapon_basic) ==
                    focgold_stat_irregular_intelligence,
                "improvised weapon should use IR");

    expect_string(focgold_skill_name(focgold_skill_clandestine_weapon_basic),
                  "Clandestine Weapon Use, Basic",
                  "clandestine weapon skill should be named");
    expect_true(focgold_skill_stat(focgold_skill_clandestine_weapon_basic) ==
                    focgold_stat_irregular_intelligence,
                "clandestine weapon should use IR");
}

static void test_medical_and_chemistry_metadata(void) {
    focgold_skill_id paramedic[] = {
        focgold_skill_first_aid
    };
    focgold_skill_id chemistry_advanced[] = {
        focgold_skill_biology_basic,
        focgold_skill_chemistry_basic
    };
    focgold_skill_id chemistry_pharmaceutical[] = {
        focgold_skill_chemistry_advanced
    };

    expect_string(focgold_skill_name(focgold_skill_first_aid),
                  "First Aid",
                  "first aid should be named");
    expect_string(focgold_skill_name(focgold_skill_paramedic),
                  "Paramedic",
                  "paramedic should be named");
    expect_true(focgold_skill_stat(focgold_skill_first_aid) ==
                    focgold_stat_regular_intelligence,
                "first aid should use RE");
    expect_true(focgold_skill_stat(focgold_skill_paramedic) ==
                    focgold_stat_regular_intelligence,
                "paramedic should use RE");
    expect_prerequisites(focgold_skill_paramedic,
                         paramedic,
                         1,
                         "paramedic should require first aid");
    expect_true(focgold_skill_improves_healing(focgold_skill_first_aid),
                "first aid should improve healing");
    expect_true(focgold_skill_improves_healing(focgold_skill_paramedic),
                "paramedic should improve healing");

    expect_prerequisites(focgold_skill_chemistry_advanced,
                         chemistry_advanced,
                         2,
                         "advanced chemistry should require biology and basic chemistry");
    expect_prerequisites(focgold_skill_chemistry_pharmaceutical,
                         chemistry_pharmaceutical,
                         1,
                         "pharmaceutical chemistry should require advanced chemistry");
    expect_true(focgold_skill_stat(focgold_skill_chemistry_pharmaceutical) ==
                    focgold_stat_regular_intelligence,
                "pharmaceutical chemistry should use RE");
    expect_true(!focgold_skill_improves_healing(
                    focgold_skill_chemistry_pharmaceutical),
                "pharmaceutical chemistry should not improve 2018 healing");
}

static void test_skill_percentages_and_lookup(void) {
    focgold_skills skills = {0};

    expect_true(focgold_validate_skills(&skills),
                "zeroed skills should be valid percentages");
    skills.firearm_basic = 100;
    skills.first_aid = 45;
    skills.paramedic = 1;
    expect_true(focgold_validate_skills(&skills),
                "skills from 0 to 100 should be valid");
    expect_true(focgold_skill_value(&skills, focgold_skill_firearm_basic) == 100,
                "skill lookup should return firearm basic value");
    expect_true(focgold_skill_value(&skills, focgold_skill_first_aid) == 45,
                "skill lookup should return first aid value");

    skills.paramedic = 101;
    expect_true(!focgold_validate_skills(&skills),
                "skill percentages above 100 should be invalid");
    skills.paramedic = -1;
    expect_true(!focgold_validate_skills(&skills),
                "skill percentages below 0 should be invalid");

    expect_true(!focgold_validate_skills(0),
                "missing skills should be invalid");
    expect_true(focgold_skill_name(focgold_skill_count) == 0,
                "skill count should not name a real skill");
    expect_true(focgold_skill_stat(focgold_skill_count) ==
                    focgold_stat_count,
                "skill count should not map to a real stat");
    expect_true(focgold_skill_value(0, focgold_skill_first_aid) == 0,
                "missing skills should report zero value");
}

static focgold_character accounting_character(void) {
    focgold_character character = {0};

    character.stats.regular_intelligence = 2;
    character.stats.irregular_intelligence = 1;
    character.stats.appearance = 0;
    character.stats.physical_health = 2;
    character.stats.mental_health = 1;
    character.skills.firearm_basic = 20;
    character.skills.firearm_advanced = 20;
    character.skills.close_combat = 20;
    character.skills.first_aid = 20;
    character.skills.paramedic = 20;

    return character;
}

static void test_skill_accounting(void) {
    focgold_character character = accounting_character();
    focgold_skills missing_prerequisite = {0};

    expect_true(focgold_stat_value(&character.stats,
                                   focgold_stat_regular_intelligence) == 2,
                "stat lookup should expose Regular Intelligence value");
    expect_true(focgold_skill_count_for_stat(&character.skills,
                                             focgold_stat_physical_health) == 2,
                "PH should count Firearm Basic and Close Combat");
    expect_true(focgold_skill_count_for_stat(&character.skills,
                                             focgold_stat_mental_health) == 1,
                "ME should count Firearm Advanced");
    expect_true(focgold_skill_count_for_stat(&character.skills,
                                             focgold_stat_regular_intelligence) == 2,
                "RE should count First Aid and Paramedic prerequisite chain");
    expect_true(focgold_validate_skill_prerequisites(&character.skills),
                "selected prerequisites should validate");
    expect_true(focgold_validate_character_skills(&character),
                "character should validate when skill counts fit stat values");

    character.skills.marching = 20;
    expect_true(!focgold_validate_character_skills(&character),
                "selected skills over a stat value should be rejected");

    missing_prerequisite.firearm_advanced = 20;
    expect_true(!focgold_validate_skill_prerequisites(&missing_prerequisite),
                "Firearm Advanced without Basic should fail prerequisites");
    character = accounting_character();
    character.skills.firearm_basic = 0;
    expect_true(!focgold_validate_character_skills(&character),
                "character skill validation should reject missing prerequisites");

    expect_true(focgold_skill_count_for_stat(0,
                                             focgold_stat_regular_intelligence) == 0,
                "missing skills should have zero selected skill count");
    expect_true(!focgold_validate_character_skills(0),
                "missing character should fail skill accounting validation");
}

int main(void) {
    test_stat_names();
    test_weapon_skill_metadata();
    test_medical_and_chemistry_metadata();
    test_skill_percentages_and_lookup();
    test_skill_accounting();

    if (failures != 0) {
        fprintf(stderr, "%d failure(s)\n", failures);
        return 1;
    }

    printf("skill tests passed\n");
    return 0;
}
