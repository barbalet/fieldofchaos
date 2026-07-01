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

int main(void) {
    test_default_wounds();
    test_unconsciousness();
    test_grave_dead_and_firearm_effects();

    if (failures != 0) {
        fprintf(stderr, "%d failure(s)\n", failures);
        return 1;
    }

    printf("wound tests passed\n");
    return 0;
}
