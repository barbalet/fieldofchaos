# Gold Rules Traceability Matrix

Source of truth: `pdf/foc-just-the-rules-gold.pdf`

Status legend:

- Implemented: represented by the C static library and covered by tests or CLI output.
- Out of scope: deliberately not represented in the C rules library.

## Section Coverage

| PDF section | Rule family | C/API coverage | Test/CLI coverage | Status |
| --- | --- | --- | --- | --- |
| 1 | Stats and character generation | `focgold_stats`, `focgold_generate_stats`, stat validation helpers | `test_stats` | Implemented |
| 1 | Skill accounting, one skill per stat point | `focgold_skill_count_for_stat`, `focgold_validate_character_skills` | `test_skills` | Implemented in Cycle 44 |
| 2 | Skill hierarchy and prerequisites | `focgold_skill_stat`, `focgold_skill_prerequisites`, `focgold_validate_skill_prerequisites` | `test_skills` | Implemented in Cycle 44 |
| 3 | Ranged weapons, ranges, thresholds, modifiers | weapon/range/modifier helpers and ranged resolver | `test_weapons`, `test_gold_actions`, CLI `attack` | Implemented through Cycle 43 |
| 4 | Shotgun and Grenade Gold rules | geometry helpers, reliability, area resolvers, CLI examples | `test_weapons`, `test_area_effects`, CLI `shotgun`, `grenade`, and `blast` | Implemented |
| 5 | Movement and grenade action economy | movement helpers and turn permissions | `test_gold_actions` | Implemented through Cycle 43 |
| 6 | Wounds, hit locations, melee | wound pools, status helpers, hit location helpers, wound mutation, melee resolver | `test_wounds`, `test_hit_locations`, `test_melee_healing`, CLI `wounds`, `locations`, and `melee` | Implemented |
| 7 | Healing | healing roll metadata, resolver, and target wound mutation | `test_melee_healing`, CLI `healing` and `heal` | Implemented |
| 8 | Special weapon and foe rules | reload/jam/called-head-shot/foe helpers, soldier armor wound integration | `test_weapons`, `test_gold_actions`, `test_foes`, CLI `foes` and `armor` | Implemented |
| 9 | Explicit 2024 exclusions | regression tests for excluded 2024 rules | `test_regressions` | Implemented |

## Character Generation And Stats

| PDF row/rule | C/API coverage | Status | Assigned cycle |
| --- | --- | --- | --- |
| RE, Regular Intelligence | `focgold_stat_regular_intelligence`, `focgold_stat_name`, `focgold_stat_value` | Implemented | 18 |
| IR, Irregular Intelligence | `focgold_stat_irregular_intelligence`, `focgold_stat_name`, `focgold_stat_value` | Implemented | 18 |
| AP, Appearance | `focgold_stat_appearance`, `focgold_stat_name`, `focgold_stat_value` | Implemented | 18 |
| PH, Physical Health | `focgold_stat_physical_health`, `focgold_stat_name`, `focgold_stat_value` | Implemented | 18 |
| ME, Mental Health | `focgold_stat_mental_health`, `focgold_stat_name`, `focgold_stat_value` | Implemented | 18 |
| Roll 3d6, invert two dice, one +2 and four +1, assign five values | `focgold_generate_stats` | Implemented | 18 |
| Stats range 0-10, initial cap 30, long-term cap 40 | stat validators | Implemented | 18 |
| One skill per stat point in each area | `focgold_validate_character_skills` | Implemented | 44 |
| Prerequisites count against skill accounting | selected prerequisite skills counted by stat | Implemented | 44 |
| No 2024 use-based advancement | no mutable use table; regression test | Implemented | 44 |

## Skill Hierarchy

| PDF row/rule | C/API coverage | Status | Assigned cycle |
| --- | --- | --- | --- |
| Firearm Use, Basic uses PH | skill metadata | Implemented | 19 |
| Firearm Use, Advanced uses ME and requires Basic | skill metadata and prerequisite validation | Implemented | 44 |
| Firearm Basic supplies +1 ranged skill die | `focgold_ranged_skill_dice` | Implemented | 42 |
| Firearm Advanced supplies +2 ranged skill dice | `focgold_ranged_skill_dice` | Implemented | 42 |
| Shotgun covered by Firearm Use, Basic | weapon metadata | Implemented | 22 |
| Grenade has no skill and Explosive Engineering does not modify it | grenade skill helpers | Implemented | 24 |
| Close Combat uses PH | skill metadata | Implemented | 19 |
| Improvised Weapon Use, Basic uses IR | skill metadata | Implemented | 19 |
| Clandestine Weapon Use, Basic uses IR | skill metadata | Implemented | 19 |
| Carpentry to Manufacture Clandestine Weapon | skill metadata and prerequisites | Implemented | 19 |
| Chemistry Basic to Explosive Engineering | skill metadata and prerequisites | Implemented | 19 |
| Biology Basic plus Chemistry Basic to Chemistry Advanced | skill metadata and prerequisites | Implemented | 19 |
| Chemistry Advanced to Pharmaceutical Chemistry | skill metadata and prerequisites | Implemented | 19 |
| First Aid to Paramedic | skill metadata and prerequisites | Implemented | 44 |
| Pharmaceutical Chemistry has no Gold healing effect | `focgold_skill_improves_healing` | Implemented | 31 |

## Ranged Weapons

| PDF row/rule | C/API coverage | Status | Assigned cycle |
| --- | --- | --- | --- |
| Sniper Rifle ranges 24/36/48 | `focgold_weapon_range_inches` | Implemented | 22 |
| Rifle ranges 18/27/36 | `focgold_weapon_range_inches` | Implemented | 22 |
| Carbine ranges 12/18/24 | `focgold_weapon_range_inches` | Implemented | 22 |
| Automatic ranges 8/12/16 | `focgold_weapon_range_inches` | Implemented | 22 |
| SubMG ranges 6/9/12 | `focgold_weapon_range_inches` | Implemented | 22 |
| Close range is 1d6, hit 4-6, head 6 | range dice and thresholds | Implemented | 42 |
| Standard range is 2d6, hit 10-12, head 12 | range dice and thresholds | Implemented | 42 |
| Long range is 3d6, hit 16-18, head 18 | range dice and thresholds | Implemented | 42 |
| Light cover -1 die | ranged modifier helper | Implemented | 23 |
| Heavy cover -2 dice | ranged modifier helper | Implemented | 23 |
| Standard target movement -1 die | ranged modifier helper | Implemented | 23 |
| Fast target movement -2 dice | ranged modifier helper | Implemented | 23 |
| Target Evade -1 die | ranged modifier helper | Implemented | 23 |
| Grenade debris acts as Light cover | ranged modifier helper | Implemented | 23 |
| Sniper Rifle one shot, Very Slow, cannot fire under 12 inches | fire rate, movement, minimum range helpers | Implemented | 43 |
| Rifle one shot, or two with Advanced, Slow | fire rate and movement helpers | Implemented | 43 |
| Carbine one shot, or two with Advanced, Standard | fire rate and movement helpers | Implemented | 43 |
| Automatic two shots, Standard, jam on 1 on 2d6, no head shot | fire rate, movement, jam, head-shot helpers | Implemented | 43 |
| SubMG three shots, Fast, jam on 1 on 1d6, no head shot | fire rate, movement, jam, head-shot helpers | Implemented | 43 |
| Shotgun one cone attack, Basic Firearm, referee movement note | shotgun metadata and referee movement flag | Implemented | 43 |
| Ammunition mutation, reload state, jam-clearing state | `focgold_apply_ranged_attack_ammo`, `focgold_reload_character`, `focgold_clear_jam_state` | Implemented | 50 |
| Called head-shot two-move turn state | `focgold_called_head_shot_progress_after_turn`, readiness, and miss helpers | Implemented | 50 |

## Shotgun

| PDF row/rule | C/API coverage | Status | Assigned cycle |
| --- | --- | --- | --- |
| Cone total length 15 inches | `focgold_shotgun_total_length_inches` | Implemented | 22 |
| Near band 0-5 inches, width 1, 2d6 wounds | shotgun band helpers and area resolver | Implemented | 45 |
| Middle band >5-10 inches, width 2, d6 wounds | shotgun band helpers and area resolver | Implemented | 45 |
| Far band >10-15 inches, width 3, d3 wounds | shotgun band helpers and area resolver | Implemented | 45 |
| Damage only nearest occupied cone band | `focgold_resolve_shotgun_area` | Implemented | 45 |
| Include allies in affected targets | `focgold_area_target.ally`, `focgold_area_damage.ally` | Implemented | 45 |
| Each wound gets normal 3d6 hit location | area resolver and hit location helper | Implemented | 45 |
| Apply wounds to character wound pools | `focgold_apply_area_damage_to_wounds` | Implemented | 47 |

## Grenade

| PDF row/rule | C/API coverage | Status | Assigned cycle |
| --- | --- | --- | --- |
| Throw range 12 inches | `focgold_grenade_throw_range_inches` | Implemented | 22 |
| Open inner radius 1 inch, d6 wounds | grenade band helpers and area resolver | Implemented | 46 |
| Open middle radius 3 inches, d3 wounds | grenade band helpers and area resolver | Implemented | 46 |
| Open outer radius 4 inches, d2 wounds | grenade band helpers and area resolver | Implemented | 46 |
| Building radii 3/6/9 inches | grenade band helpers and area resolver | Implemented | 46 |
| Reliability 2d6, dud on 4 or less | `focgold_resolve_grenade_reliability` | Implemented | 24 |
| Dud grenades cause no blast damage | `focgold_resolve_grenade_area` | Implemented | 46 |
| Debris gives Light cover | ranged modifier helper | Implemented | 23 |
| Include allies in affected targets | area target/result ally field | Implemented | 46 |
| Each wound gets normal 3d6 hit location | area resolver and hit location helper | Implemented | 46 |
| One grenade halves movement, rounded down | movement helper | Implemented | 26 |
| Stationary two grenades consumes turn | turn permissions helper | Implemented | 26 |
| Apply wounds to character wound pools | `focgold_apply_area_damage_to_wounds` | Implemented | 47 |

## Movement

| PDF row/rule | C/API coverage | Status | Assigned cycle |
| --- | --- | --- | --- |
| Very Slow 2 inches, +1 Evade | `focgold_movement_inches` | Implemented | 26 |
| Slow 3 inches, +1 Marching | `focgold_movement_inches` | Implemented | 26 |
| Standard 5 inches, +1 Marching | `focgold_movement_inches` | Implemented | 26 |
| Fast 8 inches, +2 Running | `focgold_movement_inches` | Implemented | 26 |
| One disabled leg halves movement | movement helper | Implemented | 26 |
| Two disabled legs prevent movement | movement helper | Implemented | 26 |
| Weapon-specific movement paces | `focgold_weapon_movement_rule` | Implemented | 43 |

## Wounds And Hit Locations

| PDF row/rule | C/API coverage | Status | Assigned cycle |
| --- | --- | --- | --- |
| Head 1W | `focgold_default_wounds` | Implemented | 27 |
| Chest 4W | `focgold_default_wounds` | Implemented | 27 |
| Abdomen 2W | `focgold_default_wounds` | Implemented | 27 |
| Left arm 1W | `focgold_default_wounds` | Implemented | 27 |
| Right arm 1W | `focgold_default_wounds` | Implemented | 27 |
| Left leg 2W | `focgold_default_wounds` | Implemented | 27 |
| Right leg 2W | `focgold_default_wounds` | Implemented | 27 |
| One leg at 0W halves movement | movement helper | Implemented | 28 |
| Both legs at 0W prevent movement | movement helper | Implemented | 28 |
| One arm at 0W prevents firearm use | `focgold_can_firearm` | Implemented | 28 |
| Head/chest/abdomen at 0W causes unconsciousness | `focgold_is_unconscious` | Implemented | 28 |
| Less than 6 total wounds is grave condition | `focgold_is_grave_condition` | Implemented | 28 |
| 0 total wounds is dead | `focgold_is_dead` | Implemented | 28 |
| 3-8 leg, odd left even right | `focgold_hit_location_for_3d6_total` | Implemented | 29 |
| 9-13 chest | hit location helper | Implemented | 29 |
| 14-15 abdomen | hit location helper | Implemented | 29 |
| 16-17 arm, odd left even right | hit location helper | Implemented | 29 |
| 18 head | hit location helper | Implemented | 29 |
| Mutate wound pools from resolved damage | `focgold_apply_wounds`, `focgold_apply_area_damage_to_wounds` | Implemented | 47 |

## Melee

| PDF row/rule | C/API coverage | Status | Assigned cycle |
| --- | --- | --- | --- |
| Blow hits on 3-6 | melee threshold and resolver | Implemented | 30 |
| Blow base 1d6 | melee resolver | Implemented | 30 |
| Blow +1/+2 skill dice | `focgold_melee_base_dice`, `focgold_melee_skill_dice`, melee result `skill_dice` | Implemented | 48 |
| Weapon hits on 4-6 | melee threshold and resolver | Implemented | 30 |
| Weapon base 1d6 | melee resolver | Implemented | 30 |
| Weapon +1/+2 skill dice | `focgold_melee_base_dice`, `focgold_melee_skill_dice`, melee result `skill_dice` | Implemented | 48 |

## Healing

| PDF row/rule | C/API coverage | Status | Assigned cycle |
| --- | --- | --- | --- |
| No skill rolls 1d6, recovers on 6 | healing resolver | Implemented as roll result | 31 |
| Bandage/equipment recovers on 5-6 | healing resolver | Implemented as roll result | 31 |
| First Aid rolls 2d6, use highest die | healing resolver | Implemented as roll result | 31 |
| Paramedic rolls 3d6, use highest die | healing resolver | Implemented as roll result | 31 |
| Successful healing restores one wound to a target pool | `focgold_apply_healing_result`, `focgold_apply_healing_result_to_location` | Implemented | 49 |
| Healing cannot exceed maximum wounds | `focgold_recover_wound` and healing application helpers clamp at maximum | Implemented | 49 |
| Pharmaceutical Chemistry has no healing effect | skill/healing metadata | Implemented | 31 |

## Special Rules And Foes

| PDF row/rule | C/API coverage | Status | Assigned cycle |
| --- | --- | --- | --- |
| Clip size 10 | public constant | Implemented | 24 |
| Changing clip takes one turn | public constant/helper plus `focgold_reload_character` | Implemented | 50 |
| Clearing jam takes one turn | public constant/helper plus `focgold_clear_jam_state` | Implemented | 50 |
| Automatic/SubMG jam checks | jam helpers | Implemented | 24 |
| Called head shot requires two moves and misses entirely on miss | called-head-shot progress, readiness, and miss helpers | Implemented | 50 |
| Animal head shot releases Psychnosis Gas | foe helper | Implemented | 32 |
| Animal head shot only succeeds on 5-6 on d6 | `focgold_resolve_animal_head_shot` | Implemented | 50 |
| Animal gas attack double rolls and half movement | gas attack roll-count and movement helpers | Implemented | 50 |
| Animal gas cloud d6+6 radius, shrink 2, drift 2 in d6 direction | foe helpers | Implemented | 32 |
| Animal gets +1 die on all attacks | foe attack modifier helper | Implemented | 32 |
| Hunter +1 movement | `focgold_foe_movement_inches` | Implemented | 50 |
| Soldier armor save 5-6 | armor helper | Implemented | 32 |
| Shotgun counts for Soldier armor | armor source helper and integrated damage helper | Implemented | 50 |
| Grenade does not count for Soldier armor unless referee says so | armor source helper and integrated damage helper | Implemented | 50 |
| Integrate Soldier armor with wound resolution | `focgold_apply_damage_with_soldier_armor` | Implemented | 50 |

## Exclusions

| Excluded 2024 row/rule | C/API coverage | Status | Assigned cycle |
| --- | --- | --- | --- |
| 2024 use-based skill advancement table | absent and regression-tested | Implemented | 44 |
| 2024 Pharmaceutical Chemistry healing effect | absent and regression-tested | Implemented | 31 |
| 2024 ranged thresholds | absent and regression-tested | Implemented | 22 |
| 2024 SubMG rewrite | absent and regression-tested | Implemented | 22 |
| 2024 body-only wound pool | absent and regression-tested | Implemented | 27 |
| 2024 grave-condition PH roll | absent and regression-tested | Implemented | 28 |
| 2024 winded/chest-body rule | absent and regression-tested | Implemented | 28 |
| Removal of 2018 called-head-shot procedure | regression-covered by retained metadata | Implemented | 25 |
