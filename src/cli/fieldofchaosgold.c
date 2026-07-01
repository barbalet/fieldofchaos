#include "fieldofchaosgold.h"

#include <stdio.h>
#include <string.h>

static const char *movement_name(focgold_movement_pace pace) {
    switch (pace) {
    case focgold_movement_very_slow:
        return "Very Slow";
    case focgold_movement_slow:
        return "Slow";
    case focgold_movement_standard:
        return "Standard";
    case focgold_movement_fast:
        return "Fast";
    }

    return "Unknown";
}

static void print_damage_roll(const char *label, focgold_damage_roll roll) {
    printf("%s,%dd%d\n", label, roll.dice_count, roll.die_sides);
}

static void print_stats_rules(void) {
    focgold_stat_generation_choice choice = {
        {2, 4, 6},
        0,
        2,
        1,
        {0, 1, 2, 3, 4}
    };
    focgold_stats stats;

    puts("Gold stats and character generation");
    puts("stat,name");
    printf("RE,%s\n", focgold_stat_name(focgold_stat_regular_intelligence));
    printf("IR,%s\n", focgold_stat_name(focgold_stat_irregular_intelligence));
    printf("AP,%s\n", focgold_stat_name(focgold_stat_appearance));
    printf("PH,%s\n", focgold_stat_name(focgold_stat_physical_health));
    printf("ME,%s\n", focgold_stat_name(focgold_stat_mental_health));
    puts("generation,roll 3d6, invert two chosen dice, add one +2 and four +1 values, assign to stats");
    puts("initial_cap,30");
    puts("long_term_cap,40");

    if (focgold_generate_stats(&choice, &stats)) {
        printf("example_stats,%d,%d,%d,%d,%d,total,%d\n",
               stats.regular_intelligence,
               stats.irregular_intelligence,
               stats.appearance,
               stats.physical_health,
               stats.mental_health,
               focgold_stat_total(&stats));
    }
}

static void print_skill_rules(void) {
    focgold_skills skills = {0};
    focgold_character character = {0};
    int skill;

    puts("Gold skill hierarchy");
    puts("skill,stat,prerequisite_count,healing_skill");
    for (skill = 0; skill < focgold_skill_count; skill++) {
        size_t count = 0;
        focgold_skill_id skill_id = (focgold_skill_id)skill;
        const focgold_skill_id *prerequisites =
            focgold_skill_prerequisites(skill_id, &count);

        printf("%s,%s,%lu,%d",
               focgold_skill_name(skill_id),
               focgold_stat_name(focgold_skill_stat(skill_id)),
               (unsigned long)count,
               focgold_skill_improves_healing(skill_id) ? 1 : 0);
        if (count > 0 && prerequisites != 0) {
            size_t index;
            printf(",requires");
            for (index = 0; index < count; index++) {
                printf(",%s", focgold_skill_name(prerequisites[index]));
            }
        }
        puts("");
    }

    skills.firearm_basic = 25;
    skills.firearm_advanced = 25;
    skills.first_aid = 25;
    skills.paramedic = 25;
    character.stats.regular_intelligence = 2;
    character.stats.physical_health = 1;
    character.stats.mental_health = 1;
    character.skills = skills;
    puts("skill_accounting_example");
    printf("RE_selected,%lu\n",
           (unsigned long)focgold_skill_count_for_stat(
               &skills,
               focgold_stat_regular_intelligence));
    printf("PH_selected,%lu\n",
           (unsigned long)focgold_skill_count_for_stat(
               &skills,
               focgold_stat_physical_health));
    printf("ME_selected,%lu\n",
           (unsigned long)focgold_skill_count_for_stat(
               &skills,
               focgold_stat_mental_health));
    printf("valid,%d\n", focgold_validate_character_skills(&character) ? 1 : 0);
}

static void print_weapon_rules(void) {
    int weapon;

    puts("Gold firearm ranges and movement");
    puts("weapon,close,standard,long,movement,min_range,shots_basic,shots_advanced,head_shot,jam_dice,jam_threshold");
    for (weapon = 0; weapon < focgold_weapon_count; weapon++) {
        focgold_weapon current = (focgold_weapon)weapon;
        printf("%s,%d,%d,%d,%s,%d,%d,%d,%d,%d,%d\n",
               focgold_weapon_name(current),
               focgold_weapon_range_inches(current, focgold_range_close),
               focgold_weapon_range_inches(current, focgold_range_standard),
               focgold_weapon_range_inches(current, focgold_range_long),
               movement_name(focgold_weapon_movement_rule(current)),
               focgold_weapon_minimum_range_inches(current),
               focgold_weapon_shots_per_turn(current, false),
               focgold_weapon_shots_per_turn(current, true),
               focgold_weapon_can_head_shot(current) ? 1 : 0,
               focgold_weapon_jam_dice_count(current),
               focgold_weapon_jam_threshold(current));
    }

    puts("ranged_dice,base,hit,head");
    printf("close,%d,%d,%d\n",
           focgold_ranged_base_dice(focgold_range_close),
           focgold_ranged_hit_threshold(focgold_range_close),
           focgold_ranged_head_shot_threshold(focgold_range_close));
    printf("standard,%d,%d,%d\n",
           focgold_ranged_base_dice(focgold_range_standard),
           focgold_ranged_hit_threshold(focgold_range_standard),
           focgold_ranged_head_shot_threshold(focgold_range_standard));
    printf("long,%d,%d,%d\n",
           focgold_ranged_base_dice(focgold_range_long),
           focgold_ranged_hit_threshold(focgold_range_long),
           focgold_ranged_head_shot_threshold(focgold_range_long));
}

static void print_shotgun_rules(void) {
    puts("Gold Shotgun");
    printf("cone_length_inches,%d\n", focgold_shotgun_total_length_inches());
    printf("covered_by_basic_firearm,%d\n",
           focgold_weapon_uses_firearm_basic(focgold_weapon_shotgun) ? 1 : 0);
    printf("referee_movement_rule,%d\n",
           focgold_weapon_has_referee_movement_rule(focgold_weapon_shotgun) ? 1 : 0);
    printf("band,max_inches,width_inches,damage\n");
    print_damage_roll("near,5,1", focgold_shotgun_band_wounds(focgold_shotgun_near));
    print_damage_roll("middle,10,2", focgold_shotgun_band_wounds(focgold_shotgun_middle));
    print_damage_roll("far,15,3", focgold_shotgun_band_wounds(focgold_shotgun_far));
}

static void print_grenade_rules(void) {
    puts("Gold Grenade");
    printf("throw_range_inches,%d\n", focgold_grenade_throw_range_inches());
    printf("dud_roll,%dd6<=%d\n",
           focgold_grenade_reliability_dice,
           focgold_grenade_dud_threshold);
    printf("uses_skill,%d\n", focgold_grenade_uses_skill() ? 1 : 0);
    puts("band,open_radius,building_radius,damage");
    printf("inner,%d,%d,%dd%d\n",
           focgold_grenade_radius_inches(focgold_grenade_inner, false),
           focgold_grenade_radius_inches(focgold_grenade_inner, true),
           focgold_grenade_band_wounds(focgold_grenade_inner).dice_count,
           focgold_grenade_band_wounds(focgold_grenade_inner).die_sides);
    printf("middle,%d,%d,%dd%d\n",
           focgold_grenade_radius_inches(focgold_grenade_middle, false),
           focgold_grenade_radius_inches(focgold_grenade_middle, true),
           focgold_grenade_band_wounds(focgold_grenade_middle).dice_count,
           focgold_grenade_band_wounds(focgold_grenade_middle).die_sides);
    printf("outer,%d,%d,%dd%d\n",
           focgold_grenade_radius_inches(focgold_grenade_outer, false),
           focgold_grenade_radius_inches(focgold_grenade_outer, true),
           focgold_grenade_band_wounds(focgold_grenade_outer).dice_count,
           focgold_grenade_band_wounds(focgold_grenade_outer).die_sides);
}

static void print_movement_rules(void) {
    focgold_skills no_skills = {0};
    focgold_skills skills = {0};
    focgold_wounds wounds;

    focgold_default_wounds(&wounds);
    skills.evade = 1;
    skills.marching = 1;
    skills.running = 1;

    puts("Gold movement");
    puts("pace,base,skilled,one_grenade");
    printf("very_slow,%d,%d,%d\n",
           focgold_movement_inches(focgold_movement_very_slow, &no_skills, &wounds),
           focgold_movement_inches(focgold_movement_very_slow, &skills, &wounds),
           focgold_movement_after_one_grenade_inches(
               focgold_movement_very_slow,
               &no_skills,
               &wounds));
    printf("slow,%d,%d,%d\n",
           focgold_movement_inches(focgold_movement_slow, &no_skills, &wounds),
           focgold_movement_inches(focgold_movement_slow, &skills, &wounds),
           focgold_movement_after_one_grenade_inches(
               focgold_movement_slow,
               &no_skills,
               &wounds));
    printf("standard,%d,%d,%d\n",
           focgold_movement_inches(focgold_movement_standard, &no_skills, &wounds),
           focgold_movement_inches(focgold_movement_standard, &skills, &wounds),
           focgold_movement_after_one_grenade_inches(
               focgold_movement_standard,
               &no_skills,
               &wounds));
    printf("fast,%d,%d,%d\n",
           focgold_movement_inches(focgold_movement_fast, &no_skills, &wounds),
           focgold_movement_inches(focgold_movement_fast, &skills, &wounds),
           focgold_movement_after_one_grenade_inches(
               focgold_movement_fast,
               &no_skills,
               &wounds));
    printf("two_grenades_stationary,%d\n", focgold_can_throw_two_grenades(true) ? 1 : 0);
}

static void print_wound_rules(void) {
    focgold_wounds wounds;

    focgold_default_wounds(&wounds);
    puts("Gold wounds");
    printf("head,%d\n", wounds.head);
    printf("chest,%d\n", wounds.chest);
    printf("abdomen,%d\n", wounds.abdomen);
    printf("left_arm,%d\n", wounds.left_arm);
    printf("right_arm,%d\n", wounds.right_arm);
    printf("left_leg,%d\n", wounds.left_leg);
    printf("right_leg,%d\n", wounds.right_leg);
    printf("total,%d\n", focgold_total_wounds(&wounds));
    puts("effects,one_arm_zero_blocks_firearm,head_chest_abdomen_zero_unconscious,under_6_grave,zero_dead");
}

static void print_hit_location_rules(void) {
    int total;

    puts("Gold hit locations");
    puts("3d6_total,location");
    for (total = 3; total <= 18; total++) {
        printf("%d,%s\n",
               total,
               focgold_hit_location_name(
                   focgold_hit_location_for_3d6_total(total)));
    }
}

static void print_melee_rules(void) {
    focgold_skills no_skills = {0};
    focgold_skills weapon_skills = {0};

    weapon_skills.improvised_weapon_basic = 1;
    weapon_skills.clandestine_weapon_basic = 1;
    puts("Gold melee");
    printf("blow,base,%d,skill,%d,threshold,%d\n",
           focgold_melee_base_dice(focgold_melee_blow),
           focgold_melee_skill_dice(&no_skills, focgold_melee_blow),
           focgold_melee_hit_threshold(focgold_melee_blow));
    printf("weapon,base,%d,skill,%d,threshold,%d\n",
           focgold_melee_base_dice(focgold_melee_weapon),
           focgold_melee_skill_dice(&weapon_skills, focgold_melee_weapon),
           focgold_melee_hit_threshold(focgold_melee_weapon));
}

static void print_healing_rules(void) {
    puts("Gold healing");
    puts("method,dice,threshold,highest_die");
    printf("no_skill,%d,%d,%d\n",
           focgold_healing_dice_count(focgold_healing_no_skill),
           focgold_healing_required_to_recover(focgold_healing_no_skill),
           focgold_healing_uses_highest_die(focgold_healing_no_skill) ? 1 : 0);
    printf("bandage,%d,%d,%d\n",
           focgold_healing_dice_count(focgold_healing_bandage),
           focgold_healing_required_to_recover(focgold_healing_bandage),
           focgold_healing_uses_highest_die(focgold_healing_bandage) ? 1 : 0);
    printf("first_aid,%d,%d,%d\n",
           focgold_healing_dice_count(focgold_healing_first_aid),
           focgold_healing_required_to_recover(focgold_healing_first_aid),
           focgold_healing_uses_highest_die(focgold_healing_first_aid) ? 1 : 0);
    printf("paramedic,%d,%d,%d\n",
           focgold_healing_dice_count(focgold_healing_paramedic),
           focgold_healing_required_to_recover(focgold_healing_paramedic),
           focgold_healing_uses_highest_die(focgold_healing_paramedic) ? 1 : 0);
    printf("pharmaceutical_chemistry_healing,%d\n",
           focgold_skill_improves_healing(
               focgold_skill_chemistry_pharmaceutical) ? 1 : 0);
}

static void print_foe_rules(void) {
    puts("Gold foes");
    printf("animal_attack_dice,%d\n",
           focgold_foe_attack_modifier_dice(focgold_foe_animal));
    printf("animal_head_shot_confirm,%d-6_on_d6\n",
           focgold_animal_head_shot_success_threshold());
    printf("animal_gas_attack_rolls,%d\n",
           focgold_animal_gas_attack_required_rolls());
    printf("animal_gas_half_move,%d\n",
           focgold_animal_gas_attack_halves_movement() ? 1 : 0);
    printf("animal_gas_radius,d6+6\n");
    printf("animal_gas_shrink,%d\n", focgold_animal_gas_shrink_per_turn_inches());
    printf("animal_gas_drift,%d\n", focgold_animal_gas_drift_inches_per_turn());
    printf("hunter_movement,%d\n",
           focgold_foe_movement_modifier_inches(focgold_foe_hunter));
    printf("soldier_armor_save,%d-6_on_d6\n",
           focgold_soldier_armor_save_threshold());
}

static void print_tables(void) {
    print_stats_rules();
    puts("");
    print_skill_rules();
    puts("");
    print_weapon_rules();
    puts("");
    print_shotgun_rules();
    puts("");
    print_grenade_rules();
    puts("");
    print_movement_rules();
    puts("");
    print_wound_rules();
    puts("");
    print_hit_location_rules();
    puts("");
    print_melee_rules();
    puts("");
    print_healing_rules();
    puts("");
    print_foe_rules();
}

static focgold_character example_character(focgold_weapon weapon) {
    focgold_character character = {0};

    character.weapon = weapon;
    character.stats.regular_intelligence = 3;
    character.stats.irregular_intelligence = 2;
    character.stats.appearance = 1;
    character.stats.physical_health = 4;
    character.stats.mental_health = 2;
    character.skills.firearm_basic = 50;
    character.skills.firearm_advanced = 50;
    character.skills.close_combat = 50;
    character.skills.improvised_weapon_basic = 50;
    character.skills.clandestine_weapon_basic = 50;
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
    focgold_apply_ranged_attack_ammo(&attacker, &result);

    puts("Deterministic rifle attack");
    printf("can_attack,%d\n", result.can_attack ? 1 : 0);
    printf("base_dice,%d\n", focgold_ranged_base_dice(focgold_range_close));
    printf("skill_dice,%d\n", result.skill_dice);
    printf("modifier_dice,%d\n", result.modifier_dice);
    printf("dice,%d\n", result.dice_count);
    printf("total,%d\n", result.dice_total);
    printf("threshold,%d\n", result.required_to_hit);
    printf("hit,%d\n", result.hit ? 1 : 0);
    printf("location,%s\n", focgold_hit_location_name(result.location));
    printf("shots,%d\n", result.shots_fired);
    printf("rounds_left,%d\n", attacker.rounds_in_clip);
}

static void print_shotgun_example(void) {
    focgold_area_target targets[] = {
        {1, false, 7, 0},
        {2, true, 8, 1},
        {3, false, 13, 0}
    };
    focgold_area_damage results[3];
    size_t affected = 0;
    size_t index;

    focgold_seed(2001u);
    focgold_resolve_shotgun_area(targets, 3, results, 3, &affected);

    puts("Deterministic shotgun area");
    printf("affected,%lu\n", (unsigned long)affected);
    for (index = 0; index < 3; index++) {
        printf("target_%d,affected,%d,wounds,%d",
               results[index].id,
               results[index].affected ? 1 : 0,
               results[index].wounds_inflicted);
        if (results[index].affected && results[index].wounds_inflicted > 0) {
            printf(",first_location,%s",
                   focgold_hit_location_name(results[index].hit_locations[0]));
        }
        puts("");
    }
}

static void print_grenade_example(void) {
    focgold_area_target targets[] = {
        {10, false, 1, 0},
        {11, true, 3, 0},
        {12, false, 5, 0}
    };
    focgold_area_damage results[3];
    focgold_grenade_reliability_result grenade;
    size_t affected = 0;

    focgold_seed(2002u);
    focgold_resolve_grenade_reliability(&grenade);
    focgold_resolve_grenade_area(targets,
                                 3,
                                 false,
                                 grenade.dud,
                                 results,
                                 3,
                                 &affected);

    puts("Deterministic grenade area");
    printf("reliability_total,%d\n", grenade.dice_total);
    printf("dud,%d\n", grenade.dud ? 1 : 0);
    printf("affected,%lu\n", (unsigned long)affected);
}

static void print_melee_example(void) {
    focgold_character attacker = example_character(focgold_weapon_rifle);
    focgold_melee_result result;

    focgold_seed(2501u);
    focgold_resolve_melee(&attacker, focgold_melee_weapon, &result);

    puts("Deterministic melee weapon attack");
    printf("dice,%d\n", result.dice_count);
    printf("skill_dice,%d\n", result.skill_dice);
    printf("total,%d\n", result.dice_total);
    printf("threshold,%d\n", result.required_to_hit);
    printf("hit,%d\n", result.hit ? 1 : 0);
}

static void print_healing_example(void) {
    focgold_character healer = example_character(focgold_weapon_rifle);
    focgold_character target = example_character(focgold_weapon_rifle);
    focgold_healing_result result;

    target.current_wounds.chest = 3;
    focgold_seed(3003u);
    focgold_resolve_healing(&healer,
                            &target,
                            focgold_healing_paramedic,
                            &result);
    if (result.recovered) {
        focgold_apply_healing_result_to_location(&target,
                                                &result,
                                                focgold_hit_chest);
    }

    puts("Deterministic Paramedic healing");
    printf("dice,%d\n", result.dice_count);
    printf("highest,%d\n", result.highest_die);
    printf("threshold,%d\n", result.required_to_recover);
    printf("recovered,%d\n", result.recovered ? 1 : 0);
    printf("target_chest,%d\n", target.current_wounds.chest);
}

static void print_armor_example(void) {
    focgold_wounds wounds;
    bool saved = false;

    focgold_default_wounds(&wounds);
    focgold_seed(4004u);
    focgold_apply_damage_with_soldier_armor(&wounds,
                                            focgold_damage_shotgun,
                                            false,
                                            focgold_hit_chest,
                                            2,
                                            &saved);

    puts("Deterministic Soldier armor");
    printf("saved,%d\n", saved ? 1 : 0);
    printf("chest,%d\n", wounds.chest);
}

static int print_usage(const char *program) {
    fprintf(stderr,
            "usage: %s [stats|skills|weapons|shotgun|grenade|movement|wounds|locations|melee|healing|foes|tables|attack|blast|heal|armor|all]\n",
            program);
    return 2;
}

int main(int argc, char **argv) {
    const char *command = argc > 1 ? argv[1] : "tables";

    if (strcmp(command, "stats") == 0) {
        print_stats_rules();
        return 0;
    }
    if (strcmp(command, "skills") == 0) {
        print_skill_rules();
        return 0;
    }
    if (strcmp(command, "weapons") == 0) {
        print_weapon_rules();
        return 0;
    }
    if (strcmp(command, "shotgun") == 0) {
        print_shotgun_rules();
        puts("");
        print_shotgun_example();
        return 0;
    }
    if (strcmp(command, "grenade") == 0) {
        print_grenade_rules();
        puts("");
        print_grenade_example();
        return 0;
    }
    if (strcmp(command, "movement") == 0) {
        print_movement_rules();
        return 0;
    }
    if (strcmp(command, "wounds") == 0) {
        print_wound_rules();
        return 0;
    }
    if (strcmp(command, "locations") == 0) {
        print_hit_location_rules();
        return 0;
    }
    if (strcmp(command, "melee") == 0) {
        print_melee_rules();
        puts("");
        print_melee_example();
        return 0;
    }
    if (strcmp(command, "healing") == 0) {
        print_healing_rules();
        return 0;
    }
    if (strcmp(command, "foes") == 0) {
        print_foe_rules();
        return 0;
    }
    if (strcmp(command, "tables") == 0) {
        print_tables();
        return 0;
    }
    if (strcmp(command, "attack") == 0) {
        print_attack_example();
        return 0;
    }
    if (strcmp(command, "blast") == 0) {
        print_shotgun_example();
        puts("");
        print_grenade_example();
        return 0;
    }
    if (strcmp(command, "heal") == 0) {
        print_healing_example();
        return 0;
    }
    if (strcmp(command, "armor") == 0) {
        print_armor_example();
        return 0;
    }
    if (strcmp(command, "all") == 0) {
        print_tables();
        puts("");
        print_attack_example();
        puts("");
        print_shotgun_example();
        puts("");
        print_grenade_example();
        puts("");
        print_melee_example();
        puts("");
        print_healing_example();
        puts("");
        print_armor_example();
        return 0;
    }

    return print_usage(argv[0]);
}
