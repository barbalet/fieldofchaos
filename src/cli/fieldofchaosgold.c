#include "fieldofchaosgold.h"

#include <stdio.h>
#include <string.h>

static void print_weapon_ranges(void) {
    int weapon;

    puts("Gold firearm ranges");
    puts("weapon,close,standard,long");
    for (weapon = 0; weapon < focgold_weapon_count; weapon++) {
        focgold_weapon current = (focgold_weapon)weapon;
        printf("%s,%d,%d,%d\n",
               focgold_weapon_name(current),
               focgold_weapon_range_inches(current, focgold_range_close),
               focgold_weapon_range_inches(current, focgold_range_standard),
               focgold_weapon_range_inches(current, focgold_range_long));
    }
}

static void print_special_rules(void) {
    puts("");
    puts("Gold Shotgun");
    printf("cone_length_inches,%d\n", focgold_shotgun_total_length_inches());
    printf("near_band,%d,%d,%dd%d\n",
           focgold_shotgun_band_max_inches(focgold_shotgun_near),
           focgold_shotgun_band_width_inches(focgold_shotgun_near),
           focgold_shotgun_band_wounds(focgold_shotgun_near).dice_count,
           focgold_shotgun_band_wounds(focgold_shotgun_near).die_sides);
    printf("middle_band,%d,%d,%dd%d\n",
           focgold_shotgun_band_max_inches(focgold_shotgun_middle),
           focgold_shotgun_band_width_inches(focgold_shotgun_middle),
           focgold_shotgun_band_wounds(focgold_shotgun_middle).dice_count,
           focgold_shotgun_band_wounds(focgold_shotgun_middle).die_sides);
    printf("far_band,%d,%d,%dd%d\n",
           focgold_shotgun_band_max_inches(focgold_shotgun_far),
           focgold_shotgun_band_width_inches(focgold_shotgun_far),
           focgold_shotgun_band_wounds(focgold_shotgun_far).dice_count,
           focgold_shotgun_band_wounds(focgold_shotgun_far).die_sides);

    puts("");
    puts("Gold Grenade");
    printf("throw_range_inches,%d\n", focgold_grenade_throw_range_inches());
    printf("open_radii,%d,%d,%d\n",
           focgold_grenade_radius_inches(focgold_grenade_inner, false),
           focgold_grenade_radius_inches(focgold_grenade_middle, false),
           focgold_grenade_radius_inches(focgold_grenade_outer, false));
    printf("building_radii,%d,%d,%d\n",
           focgold_grenade_radius_inches(focgold_grenade_inner, true),
           focgold_grenade_radius_inches(focgold_grenade_middle, true),
           focgold_grenade_radius_inches(focgold_grenade_outer, true));
    printf("dud_roll,%dd6<=%d\n",
           focgold_grenade_reliability_dice,
           focgold_grenade_dud_threshold);
}

static void print_tables(void) {
    focgold_wounds wounds;

    print_weapon_ranges();
    print_special_rules();

    puts("");
    puts("Ranged thresholds");
    printf("close,%d,%d\n",
           focgold_ranged_hit_threshold(focgold_range_close),
           focgold_ranged_head_shot_threshold(focgold_range_close));
    printf("standard,%d,%d\n",
           focgold_ranged_hit_threshold(focgold_range_standard),
           focgold_ranged_head_shot_threshold(focgold_range_standard));
    printf("long,%d,%d\n",
           focgold_ranged_hit_threshold(focgold_range_long),
           focgold_ranged_head_shot_threshold(focgold_range_long));

    focgold_default_wounds(&wounds);
    puts("");
    puts("Default wounds");
    printf("head,%d\n", wounds.head);
    printf("chest,%d\n", wounds.chest);
    printf("abdomen,%d\n", wounds.abdomen);
    printf("left_arm,%d\n", wounds.left_arm);
    printf("right_arm,%d\n", wounds.right_arm);
    printf("left_leg,%d\n", wounds.left_leg);
    printf("right_leg,%d\n", wounds.right_leg);
    printf("total,%d\n", focgold_total_wounds(&wounds));
}

static focgold_character example_character(focgold_weapon weapon) {
    focgold_character character = {0};

    character.weapon = weapon;
    character.skills.firearm_basic = 50;
    character.skills.firearm_advanced = 50;
    character.skills.first_aid = 50;
    character.skills.paramedic = 50;
    focgold_default_wounds(&character.maximum_wounds);
    focgold_default_wounds(&character.current_wounds);
    character.rounds_in_clip = focgold_standard_clip_rounds;
    character.clips = 2;
    character.has_bandage = true;

    return character;
}

static void print_attack_example(void) {
    focgold_character attacker = example_character(focgold_weapon_rifle);
    focgold_character target = example_character(focgold_weapon_rifle);
    focgold_attack_modifiers modifiers = {
        focgold_cover_none,
        focgold_target_movement_none,
        false,
        false
    };
    focgold_attack_result result;

    focgold_seed(1001u);
    focgold_resolve_ranged_attack(&attacker,
                                  &target,
                                  focgold_range_close,
                                  &modifiers,
                                  false,
                                  &result);

    puts("Deterministic rifle attack");
    printf("can_attack,%d\n", result.can_attack ? 1 : 0);
    printf("dice,%d\n", result.dice_count);
    printf("total,%d\n", result.dice_total);
    printf("threshold,%d\n", result.required_to_hit);
    printf("hit,%d\n", result.hit ? 1 : 0);
    printf("location,%s\n", focgold_hit_location_name(result.location));
    printf("shots,%d\n", result.shots_fired);
}

static void print_blast_example(void) {
    focgold_grenade_reliability_result grenade;
    focgold_hit_location location;

    focgold_seed(2002u);
    focgold_resolve_grenade_reliability(&grenade);
    location = focgold_roll_blast_hit_location();

    puts("Deterministic blast example");
    printf("shotgun_near_damage,%dd%d\n",
           focgold_shotgun_band_wounds(focgold_shotgun_near).dice_count,
           focgold_shotgun_band_wounds(focgold_shotgun_near).die_sides);
    printf("grenade_reliability_total,%d\n", grenade.dice_total);
    printf("grenade_dud,%d\n", grenade.dud ? 1 : 0);
    printf("blast_location,%s\n", focgold_hit_location_name(location));
}

static void print_healing_example(void) {
    focgold_character healer = example_character(focgold_weapon_rifle);
    focgold_character target = example_character(focgold_weapon_rifle);
    focgold_healing_result result;

    focgold_seed(3003u);
    focgold_resolve_healing(&healer,
                            &target,
                            focgold_healing_paramedic,
                            &result);

    puts("Deterministic Paramedic healing");
    printf("dice,%d\n", result.dice_count);
    printf("highest,%d\n", result.highest_die);
    printf("threshold,%d\n", result.required_to_recover);
    printf("recovered,%d\n", result.recovered ? 1 : 0);
}

static int print_usage(const char *program) {
    fprintf(stderr, "usage: %s [tables|attack|blast|heal|all]\n", program);
    return 2;
}

int main(int argc, char **argv) {
    const char *command = argc > 1 ? argv[1] : "tables";

    if (strcmp(command, "tables") == 0) {
        print_tables();
        return 0;
    }
    if (strcmp(command, "attack") == 0) {
        print_attack_example();
        return 0;
    }
    if (strcmp(command, "blast") == 0) {
        print_blast_example();
        return 0;
    }
    if (strcmp(command, "heal") == 0) {
        print_healing_example();
        return 0;
    }
    if (strcmp(command, "all") == 0) {
        print_tables();
        puts("");
        print_attack_example();
        puts("");
        print_blast_example();
        puts("");
        print_healing_example();
        return 0;
    }

    return print_usage(argv[0]);
}
