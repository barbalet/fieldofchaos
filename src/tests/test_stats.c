#include "fieldofchaos2018.h"

#include <stdio.h>

static int failures = 0;

static void expect_true(int condition, const char *message) {
    if (!condition) {
        fprintf(stderr, "FAIL: %s\n", message);
        failures++;
    }
}

static void test_stat_caps(void) {
    foc2018_stats initial_max = {6, 6, 6, 6, 6};
    foc2018_stats above_initial = {6, 6, 6, 6, 7};
    foc2018_stats long_term_max = {10, 10, 10, 5, 5};
    foc2018_stats above_long_term = {10, 10, 10, 6, 5};
    foc2018_stats too_low = {-1, 6, 6, 6, 6};
    foc2018_stats too_high = {11, 6, 6, 6, 6};

    expect_true(foc2018_stat_total(&initial_max) == 30, "stat total should add five stats");
    expect_true(foc2018_validate_initial_stats(&initial_max), "initial cap should allow 30");
    expect_true(!foc2018_validate_initial_stats(&above_initial), "initial cap should reject 31");
    expect_true(foc2018_validate_long_term_stats(&above_initial), "long-term cap should allow 31");
    expect_true(foc2018_validate_long_term_stats(&long_term_max), "long-term cap should allow 40");
    expect_true(!foc2018_validate_long_term_stats(&above_long_term), "long-term cap should reject 41");
    expect_true(!foc2018_validate_initial_stats(&too_low), "stats below 0 should be invalid");
    expect_true(!foc2018_validate_initial_stats(&too_high), "stats above 10 should be invalid");
    expect_true(foc2018_stat_total(0) == 0, "missing stats total should be zero");
    expect_true(!foc2018_validate_initial_stats(0), "missing initial stats should be invalid");
    expect_true(!foc2018_validate_long_term_stats(0), "missing long-term stats should be invalid");
}

static void test_generation_uses_player_choices(void) {
    foc2018_stat_generation_choice choice = {
        {2, 5, 6},
        0,
        1,
        2,
        {
            2,
            0,
            1,
            3,
            4
        }
    };
    foc2018_stat_generation_choice reassigned = choice;
    foc2018_stats stats;
    foc2018_stats other_stats;

    expect_true(foc2018_generate_stats(&choice, &stats), "valid generation choice should succeed");
    expect_true(stats.regular_intelligence == 8, "RE should use selected final value");
    expect_true(stats.irregular_intelligence == 3, "IR should use assigned final value");
    expect_true(stats.appearance == 6, "AP should use assigned final value");
    expect_true(stats.physical_health == 6, "PH should use first inverted value with +1");
    expect_true(stats.mental_health == 3, "ME should use second inverted value with +1");
    expect_true(foc2018_stat_total(&stats) == 26, "generated stats should keep total");
    expect_true(foc2018_validate_initial_stats(&stats), "generated stats should satisfy initial cap");

    reassigned.stat_value_indices[foc2018_stat_regular_intelligence] = 0;
    reassigned.stat_value_indices[foc2018_stat_irregular_intelligence] = 2;
    expect_true(foc2018_generate_stats(&reassigned, &other_stats), "reassigned generation should succeed");
    expect_true(other_stats.regular_intelligence == 3, "assignment choice should affect RE");
    expect_true(other_stats.irregular_intelligence == 8, "assignment choice should affect IR");
}

static void test_generation_rejects_invalid_choices(void) {
    foc2018_stat_generation_choice choice = {
        {2, 5, 6},
        0,
        1,
        2,
        {
            2,
            0,
            1,
            3,
            4
        }
    };
    foc2018_stats stats;

    choice.rolled_dice[0] = 0;
    expect_true(!foc2018_generate_stats(&choice, &stats), "die rolls below 1 should fail");
    choice.rolled_dice[0] = 2;

    choice.second_inverted_die = 0;
    expect_true(!foc2018_generate_stats(&choice, &stats), "same die cannot be inverted twice");
    choice.second_inverted_die = 1;

    choice.plus_two_value = 5;
    expect_true(!foc2018_generate_stats(&choice, &stats), "+2 value index must be in range");
    choice.plus_two_value = 2;

    choice.stat_value_indices[4] = 3;
    expect_true(!foc2018_generate_stats(&choice, &stats), "stat assignments must be a permutation");
    choice.stat_value_indices[4] = 4;

    expect_true(!foc2018_generate_stats(0, &stats), "missing generation choice should fail");
    expect_true(!foc2018_generate_stats(&choice, 0), "missing output stats should fail");
}

int main(void) {
    test_stat_caps();
    test_generation_uses_player_choices();
    test_generation_rejects_invalid_choices();

    if (failures != 0) {
        fprintf(stderr, "%d failure(s)\n", failures);
        return 1;
    }

    printf("stat tests passed\n");
    return 0;
}
