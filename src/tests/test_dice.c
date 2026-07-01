#include "fieldofchaos2018.h"
#include "../lib/fieldofchaos2018_internal.h"

#include <stdio.h>

static int failures = 0;

static void expect_true(int condition, const char *message) {
    if (!condition) {
        fprintf(stderr, "FAIL: %s\n", message);
        failures++;
    }
}

static void test_seed_repeats_sequence(void) {
    int first[8];
    int second[8];
    int index;

    foc2018_seed(2018u);
    for (index = 0; index < 8; index++) {
        first[index] = foc2018_roll_d6();
        expect_true(first[index] >= 1 && first[index] <= 6, "roll should stay within d6 bounds");
    }

    foc2018_seed(2018u);
    for (index = 0; index < 8; index++) {
        second[index] = foc2018_roll_d6();
        expect_true(second[index] == first[index], "same seed should repeat d6 sequence");
    }
}

static void test_multiple_dice_helpers(void) {
    int a;
    int b;
    int c;
    int total;
    int highest;
    int expected_highest;

    expect_true(foc2018_roll_d6_sum(0) == 0, "zero dice sum should be zero");
    expect_true(foc2018_roll_d6_highest(0) == 0, "zero dice highest should be zero");

    foc2018_seed(42u);
    total = foc2018_roll_d6_sum(3);
    foc2018_seed(42u);
    a = foc2018_roll_d6();
    b = foc2018_roll_d6();
    c = foc2018_roll_d6();
    expect_true(total == a + b + c, "sum helper should match individual rolls");

    foc2018_seed(99u);
    highest = foc2018_roll_d6_highest(3);
    foc2018_seed(99u);
    a = foc2018_roll_d6();
    b = foc2018_roll_d6();
    c = foc2018_roll_d6();
    expected_highest = a;
    if (b > expected_highest) {
        expected_highest = b;
    }
    if (c > expected_highest) {
        expected_highest = c;
    }
    expect_true(highest == expected_highest, "highest helper should match individual rolls");
}

int main(void) {
    test_seed_repeats_sequence();
    test_multiple_dice_helpers();

    if (failures != 0) {
        fprintf(stderr, "%d failure(s)\n", failures);
        return 1;
    }

    printf("dice tests passed\n");
    return 0;
}
