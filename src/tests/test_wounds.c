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

static void test_default_wounds(void) {
    focgold_wounds wounds = {0};

    focgold_default_wounds(&wounds);

    expect_int(wounds.head, 1, "head should have 1W");
    expect_int(wounds.chest, 4, "chest should have 4W");
    expect_int(wounds.abdomen, 2, "abdomen should have 2W");
    expect_int(wounds.left_arm, 1, "left arm should have 1W");
    expect_int(wounds.right_arm, 1, "right arm should have 1W");
    expect_int(wounds.left_leg, 2, "left leg should have 2W");
    expect_int(wounds.right_leg, 2, "right leg should have 2W");
    expect_int(focgold_total_wounds(&wounds),
               13,
               "default wound total should be 13W");
}

static void test_unconsciousness(void) {
    focgold_wounds wounds;

    focgold_default_wounds(&wounds);
    expect_true(!focgold_is_unconscious(&wounds),
                "full wounds should not be unconscious");

    wounds.head = 0;
    expect_true(focgold_is_unconscious(&wounds),
                "head at 0W should cause unconsciousness");

    focgold_default_wounds(&wounds);
    wounds.chest = 0;
    expect_true(focgold_is_unconscious(&wounds),
                "chest at 0W should cause unconsciousness");

    focgold_default_wounds(&wounds);
    wounds.abdomen = 0;
    expect_true(focgold_is_unconscious(&wounds),
                "abdomen at 0W should cause unconsciousness");
}

static void test_grave_dead_and_firearm_effects(void) {
    focgold_wounds wounds;

    focgold_default_wounds(&wounds);
    expect_true(!focgold_is_grave_condition(&wounds),
                "full wounds should not be grave condition");
    expect_true(!focgold_is_dead(&wounds),
                "full wounds should not be dead");
    expect_true(focgold_can_firearm(&wounds),
                "full wounds should allow firearm use");

    wounds.chest = 1;
    wounds.abdomen = 1;
    wounds.left_arm = 0;
    wounds.right_arm = 1;
    wounds.left_leg = 1;
    wounds.right_leg = 0;
    expect_int(focgold_total_wounds(&wounds),
               5,
               "test wounds should total 5W");
    expect_true(focgold_is_grave_condition(&wounds),
                "less than 6 total wounds should be grave condition");
    expect_true(!focgold_can_firearm(&wounds),
                "one arm at 0W should prevent firearm use");

    wounds.head = 0;
    wounds.chest = 0;
    wounds.abdomen = 0;
    wounds.left_arm = 0;
    wounds.right_arm = 0;
    wounds.left_leg = 0;
    wounds.right_leg = 0;
    expect_true(focgold_is_dead(&wounds),
                "no wounds remaining should mark dead");
    expect_true(!focgold_can_firearm(&wounds),
                "dead character should not fire");
}

static void test_damage_application(void) {
    focgold_wounds wounds;

    focgold_default_wounds(&wounds);
    expect_int(focgold_wound_value(&wounds, focgold_hit_chest),
               4,
               "wound value helper should expose chest wounds");
    expect_true(focgold_apply_wounds(&wounds,
                                     focgold_hit_chest,
                                     2),
                "specific-location damage should apply");
    expect_int(wounds.chest,
               2,
               "specific-location damage should subtract wounds");
    expect_true(focgold_apply_wounds(&wounds,
                                     focgold_hit_chest,
                                     10),
                "overkill damage should still apply");
    expect_int(wounds.chest,
               0,
               "overkill damage should clamp wound pool at zero");
    expect_true(focgold_is_unconscious(&wounds),
                "wound effects should update after damage application");
    expect_true(!focgold_apply_wounds(&wounds,
                                      focgold_hit_head,
                                      -1),
                "negative damage should be rejected");
}

static void test_area_damage_application(void) {
    focgold_wounds wounds;
    focgold_area_damage damage = {0};

    focgold_default_wounds(&wounds);
    damage.affected = true;
    damage.wounds_inflicted = 4;
    damage.hit_locations[0] = focgold_hit_left_arm;
    damage.hit_locations[1] = focgold_hit_left_arm;
    damage.hit_locations[2] = focgold_hit_right_leg;
    damage.hit_locations[3] = focgold_hit_head;

    expect_true(focgold_apply_area_damage_to_wounds(&wounds, &damage),
                "area damage should apply one wound at a time");
    expect_int(wounds.left_arm,
               0,
               "repeated area hits should clamp a limb at zero");
    expect_int(wounds.right_leg,
               1,
               "area hit should subtract one right leg wound");
    expect_int(wounds.head,
               0,
               "area hit should apply to head wound pool");
    expect_true(!focgold_can_firearm(&wounds),
                "area damage should update firearm eligibility through wounds");

    damage.affected = false;
    wounds.chest = 4;
    expect_true(focgold_apply_area_damage_to_wounds(&wounds, &damage),
                "unaffected area damage result should be a valid no-op");
    expect_int(wounds.chest,
               4,
               "unaffected area damage should not mutate wounds");
}

static void test_healing_clamping(void) {
    focgold_character target = {0};
    focgold_healing_result result = {0};

    focgold_default_wounds(&target.maximum_wounds);
    focgold_default_wounds(&target.current_wounds);
    target.current_wounds.left_arm = 0;
    target.current_wounds.chest = 3;

    result.recovered = true;
    result.wounds_recovered = 1;
    result.location = focgold_hit_left_arm;

    expect_true(focgold_apply_healing_result(&target, &result),
                "healing result should restore its rolled location");
    expect_int(target.current_wounds.left_arm,
               1,
               "healing should restore exactly one selected wound");
    expect_true(!focgold_apply_healing_result(&target, &result),
                "healing should not exceed maximum wound value");
    expect_int(target.current_wounds.left_arm,
               1,
               "healing clamp should keep limb at maximum");

    expect_true(focgold_apply_healing_result_to_location(&target,
                                                        &result,
                                                        focgold_hit_chest),
                "healing can apply to an explicitly selected location");
    expect_int(target.current_wounds.chest,
               4,
               "selected-location healing should restore one wound");

    result.recovered = false;
    expect_true(focgold_apply_healing_result_to_location(&target,
                                                        &result,
                                                        focgold_hit_chest),
                "failed healing result should be a valid no-op");
}

int main(void) {
    test_default_wounds();
    test_unconsciousness();
    test_grave_dead_and_firearm_effects();
    test_damage_application();
    test_area_damage_application();
    test_healing_clamping();

    if (failures != 0) {
        fprintf(stderr, "%d failure(s)\n", failures);
        return 1;
    }

    printf("wound tests passed\n");
    return 0;
}
