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

static focgold_character test_character(void) {
    focgold_character character = {0};

    focgold_default_wounds(&character.maximum_wounds);
    focgold_default_wounds(&character.current_wounds);
    character.weapon = focgold_weapon_rifle;
    character.rounds_in_clip = focgold_standard_clip_rounds;

    return character;
}

static void test_melee_thresholds_and_skills(void) {
    focgold_skills skills = {0};

    expect_int(focgold_melee_hit_threshold(focgold_melee_blow),
               3,
               "blow should hit on 3-6");
    expect_int(focgold_melee_hit_threshold(focgold_melee_weapon),
               4,
               "weapon should hit on 4-6");
    expect_int(focgold_melee_base_dice(focgold_melee_blow),
               1,
               "blow should roll 1 base die");
    expect_int(focgold_melee_base_dice(focgold_melee_weapon),
               1,
               "weapon attack should roll 1 base die");
    expect_true(focgold_melee_hit_threshold(focgold_melee_blow) != 2,
                "blow should not use the 2024 2-6 threshold");
    expect_true(focgold_melee_hit_threshold(focgold_melee_weapon) != 3,
                "weapon should not use the 2024 3-6 threshold");

    expect_true(focgold_melee_skill_applies(focgold_melee_blow,
                                            focgold_skill_close_combat),
                "Close Combat should apply to blows");
    expect_true(!focgold_melee_skill_applies(
                    focgold_melee_blow,
                    focgold_skill_improvised_weapon_basic),
                "Improvised Weapon Use should not apply to blows");
    expect_true(focgold_melee_skill_applies(
                    focgold_melee_weapon,
                    focgold_skill_improvised_weapon_basic),
                "Improvised Weapon Use should apply to melee weapons");
    expect_true(focgold_melee_skill_applies(
                    focgold_melee_weapon,
                    focgold_skill_clandestine_weapon_basic),
                "Clandestine Weapon Use should apply to melee weapons");

    expect_int(focgold_melee_skill_modifier_dice(&skills,
                                                 focgold_melee_blow),
               0,
               "no melee skills should apply no modifier dice");
    expect_int(focgold_melee_skill_dice(&skills,
                                        focgold_melee_blow),
               0,
               "no melee skills should expose +0 skill dice");
    skills.close_combat = 20;
    expect_int(focgold_melee_skill_dice(&skills,
                                        focgold_melee_blow),
               1,
               "Close Combat should expose +1 blow skill die");
    expect_int(focgold_melee_skill_modifier_dice(&skills,
                                                 focgold_melee_blow),
               1,
               "Close Combat should add an explicit blow modifier die");
    expect_int(focgold_melee_skill_modifier_dice(&skills,
                                                 focgold_melee_weapon),
               0,
               "Close Combat should not modify weapon attacks");
    skills.improvised_weapon_basic = 10;
    skills.clandestine_weapon_basic = 15;
    expect_int(focgold_melee_skill_modifier_dice(&skills,
                                                 focgold_melee_weapon),
               2,
               "weapon skills should add explicit weapon modifier dice");
    expect_int(focgold_melee_skill_dice(&skills,
                                        focgold_melee_weapon),
               2,
               "two weapon skills should expose +2 melee skill dice");
}

static void test_melee_resolution(void) {
    focgold_character attacker = test_character();
    focgold_melee_result result;
    int expected_total;

    focgold_seed(301u);
    expect_true(focgold_resolve_melee(&attacker,
                                      focgold_melee_blow,
                                      &result),
                "blow should resolve");
    focgold_seed(301u);
    expected_total = focgold_roll_d6();
    expect_int(result.dice_count,
               1,
               "unskilled blow should roll 1d6");
    expect_int(result.skill_dice,
               0,
               "unskilled blow should expose +0 skill dice");
    expect_int(result.dice_total,
               expected_total,
               "melee result should expose the dice total");
    expect_int(result.required_to_hit,
               3,
               "blow result should expose threshold 3");
    expect_true(result.hit == (expected_total >= 3),
                "blow hit should follow 3-6 threshold");

    attacker.skills.close_combat = 25;
    focgold_seed(302u);
    expect_true(focgold_resolve_melee(&attacker,
                                      focgold_melee_blow,
                                      &result),
                "skilled blow should resolve");
    expect_int(result.dice_count,
               2,
               "Close Combat should add a die to blow resolution");
    expect_int(result.skill_dice,
               1,
               "skilled blow should expose +1 skill die");

    attacker.skills.improvised_weapon_basic = 25;
    attacker.skills.clandestine_weapon_basic = 25;
    focgold_seed(303u);
    expect_true(focgold_resolve_melee(&attacker,
                                      focgold_melee_weapon,
                                      &result),
                "two-skill weapon attack should resolve");
    expect_int(result.skill_dice,
               2,
               "weapon attack should expose +2 skill dice");
    expect_int(result.dice_count,
               3,
               "weapon attack with +2 skill dice should roll 3d6");

    expect_true(!focgold_resolve_melee(0,
                                       focgold_melee_blow,
                                       &result),
                "missing melee attacker should fail");
    expect_true(!focgold_resolve_melee(&attacker,
                                       focgold_melee_blow,
                                       0),
                "missing melee result should fail");
}

static int expected_highest_roll(int dice_count) {
    int highest = 0;
    int index;

    for (index = 0; index < dice_count; index++) {
        int roll = focgold_roll_d6();
        if (roll > highest) {
            highest = roll;
        }
    }

    return highest;
}

static void test_healing_metadata(void) {
    expect_int(focgold_healing_dice_count(focgold_healing_no_skill),
               1,
               "no medical skill should roll 1d6");
    expect_int(focgold_healing_required_to_recover(focgold_healing_no_skill),
               6,
               "no medical skill should recover on 6");
    expect_int(focgold_healing_dice_count(focgold_healing_bandage),
               1,
               "bandage should roll 1d6");
    expect_int(focgold_healing_required_to_recover(focgold_healing_bandage),
               5,
               "bandage should recover on 5-6");
    expect_int(focgold_healing_dice_count(focgold_healing_first_aid),
               2,
               "First Aid should roll 2d6");
    expect_true(focgold_healing_uses_highest_die(focgold_healing_first_aid),
                "First Aid should use the highest die");
    expect_int(focgold_healing_dice_count(focgold_healing_paramedic),
               3,
               "Paramedic should roll 3d6");
    expect_true(focgold_healing_uses_highest_die(focgold_healing_paramedic),
                "Paramedic should use the highest die");
    expect_true(!focgold_skill_improves_healing(
                    focgold_skill_chemistry_pharmaceutical),
                "Pharmaceutical Chemistry should not improve Gold healing");
}

static void test_healing_resolution(void) {
    focgold_character healer = test_character();
    focgold_character target = test_character();
    focgold_healing_result result;
    int expected_highest;

    focgold_seed(401u);
    expect_true(focgold_resolve_healing(&healer,
                                        &target,
                                        focgold_healing_no_skill,
                                        &result),
                "no-skill healing should resolve");
    focgold_seed(401u);
    expected_highest = expected_highest_roll(1);
    expect_int(result.dice_count,
               1,
               "no-skill healing should roll 1d6");
    expect_int(result.highest_die,
               expected_highest,
               "healing result should expose the highest die");
    expect_int(result.required_to_recover,
               6,
               "no-skill healing should require 6");
    expect_true(result.recovered == (expected_highest >= 6),
                "no-skill recovery should match threshold");

    focgold_seed(402u);
    expect_true(focgold_resolve_healing(&healer,
                                        &target,
                                        focgold_healing_first_aid,
                                        &result),
                "First Aid healing should resolve");
    focgold_seed(402u);
    expected_highest = expected_highest_roll(2);
    expect_int(result.dice_count,
               2,
               "First Aid healing should roll 2d6");
    expect_int(result.highest_die,
               expected_highest,
               "First Aid should use the highest of 2d6");
    expect_int(result.required_to_recover,
               5,
               "First Aid should recover on 5-6");

    focgold_seed(403u);
    expect_true(focgold_resolve_healing(&healer,
                                        &target,
                                        focgold_healing_paramedic,
                                        &result),
                "Paramedic healing should resolve");
    expect_int(result.dice_count,
               3,
               "Paramedic healing should roll 3d6");

    expect_true(!focgold_resolve_healing(&healer,
                                         &target,
                                         (focgold_healing_method)99,
                                         &result),
                "invalid healing method should fail");
    expect_true(!focgold_resolve_healing(&healer,
                                         &target,
                                         focgold_healing_no_skill,
                                         0),
                "missing healing result should fail");
}

static void test_healing_application(void) {
    focgold_character healer = test_character();
    focgold_character target = test_character();
    focgold_healing_result result = {0};

    target.current_wounds.abdomen = 1;
    result.recovered = true;
    result.wounds_recovered = 1;
    result.location = focgold_hit_abdomen;

    expect_true(focgold_apply_healing_result(&target, &result),
                "successful healing should apply to rolled location");
    expect_int(target.current_wounds.abdomen,
               2,
               "successful healing should restore one abdomen wound");
    expect_true(!focgold_apply_healing_result(&target, &result),
                "healing should not exceed maximum wound value");
    expect_int(target.current_wounds.abdomen,
               2,
               "healing should clamp at maximum wound value");

    target.current_wounds.chest = 2;
    result.location = focgold_hit_head;
    expect_true(focgold_apply_healing_result_to_location(&target,
                                                        &result,
                                                        focgold_hit_chest),
                "successful healing can apply to a selected location");
    expect_int(target.current_wounds.chest,
               3,
               "selected-location healing should restore exactly one wound");

    focgold_seed(404u);
    expect_true(focgold_resolve_healing(&healer,
                                        &target,
                                        focgold_healing_bandage,
                                        &result),
                "bandage healing should resolve before state application");
    if (result.recovered) {
        focgold_apply_healing_result(&target, &result);
    }
    expect_true(target.current_wounds.head <= target.maximum_wounds.head,
                "rolled-location healing should never exceed maximum head wounds");
}

int main(void) {
    test_melee_thresholds_and_skills();
    test_melee_resolution();
    test_healing_metadata();
    test_healing_resolution();
    test_healing_application();

    if (failures != 0) {
        fprintf(stderr, "%d failure(s)\n", failures);
        return 1;
    }

    printf("melee and healing tests passed\n");
    return 0;
}
