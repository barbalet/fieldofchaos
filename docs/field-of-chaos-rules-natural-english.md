# Field of Chaos RPG Rules (Natural English)

This is a plain-language rewrite of the extracted rules from `pdf/field-of-chaos-rpg-070524.pdf`. It is written for fast human review and implementation.

## 1. What You Need
- One Controller (GM-equivalent).
- One to ten Players.
- At least five six-sided dice.
- Paper for character and session tracking.

## 2. How Play Is Governed
- The Controller narrates and adjudicates.
- Players control characters.
- House rules are welcome, but must be written down and agreed before the session.

### Fast dispute rule
If a rule argument cannot be settled in about 2 minutes, roll dice and move on.
- Two sides: odd vs even on 1d6.
- Three sides: assign 1, 2, 3; on 1/4 side 1 wins, on 2/5 side 2 wins, on 3/6 side 3 wins.
- More than three sides: consolidate to 2 or 3 options first.

## 3. Character Stats
Each character has five core stats:
- Regular Intelligence (RE)
- Irregular Intelligence (IR)
- Appearance (AP)
- Physical Health (PH)
- Mental Health (ME)

Limits:
- Each stat is 0-10.
- Starting total across all five is at most 30.
- Long-term cap is 40 total.

## 4. Character Stat Generation (Step-by-Step)
1. Roll 3d6.
2. Choose two of those dice and also take each die's opposite face value (7 minus die face).
3. You now have 5 numbers.
4. Add +2 to one chosen number.
5. Add +1 to the other four numbers.
6. Assign the final five numbers to RE, IR, AP, PH, and ME.

## 5. Skills
- You get skill slots based on stat points in each stat area.
- Prerequisites must be satisfied.
- Skills improve through use.

### Skill improvement
Every time you use a skill, roll 1d6. On success, gain +1% in that skill.
- Skill <17%: improve on 2-6.
- 17-33%: improve on 3-6.
- 34-50%: improve on 4-6.
- 51-67%: improve on 5-6.
- >67%: improve on 6 only.

### Skill families
- RE mainly covers technical/academic/medical/logistics skills.
- IR covers covert/field/adaptive skills.
- AP covers social disguise and movement blending.
- PH covers movement and close-combat physical skills.
- ME covers driving, advanced firearm use, language/social resilience.

For exact skill names and prerequisites, use: `docs/field-of-chaos-rules-extracted.md`.

## 6. Core Combat

### 6.1 Range bands by weapon
- Sniper Rifle: close 288y, standard 432y, long 576y.
- Rifle: close 216y, standard 324y, long 432y.
- SubMG: close 96y, standard 192y, no long-band value listed.

### 6.2 Ranged hit method
Use a dice pool that depends on range, then compare to the required band total.
- Close: roll `1d6` (plus possible skill dice), hit on 4-6.
- Standard: roll `2d6` (plus possible skill dice), hit on 8-12.
- Long: roll `3d6` (plus possible skill dice), hit on 14-18.

Skill-based extra dice:
- `*` tier: +1 die.
- `**` tier: +2 dice.

Penalties (each removes 1 die):
- Target in some cover.
- Target moving fast.
- Target has Evade.

### 6.3 Fire rate and movement limits
- Sniper Rifle: 1 shot/turn, cannot move and fire.
- Rifle: 1 shot/turn, or 2 shots with higher skill tier.
- SubMG: 4 shots/turn, can move fast, but jams if any used roll includes a 1.

Ammo handling:
- Clip size is 10 rounds.
- Reload takes 1 turn.
- Clearing jam takes 1 turn.

### 6.4 Movement speeds
- Slow: 2 yards/sec, +1 with Evade.
- Standard: 4 yards/sec, +2 with Marching.
- Fast: 7 yards/sec, +3 with Running.

### 6.5 Hit location (important)
When a bullet hits, roll 2d6 for hit location:
- 4: left arm.
- 6: left leg.
- 8: right leg.
- 10: right arm.
- 12: head.
- Anything else: body.

Important nuance: standard bullet headshots come from `2d6 = 12`.

### 6.6 Wound model
Starting body-part wound pools:
- Head 1
- Body 4
- Each arm 1
- Each leg 2

Effects:
- One leg at zero -> movement halved.
- Both legs at zero -> cannot move.
- One arm at zero -> cannot fire firearms.
- Head or body at zero -> unconscious.

Critical states:
- Less than 6 total wounds remaining -> grave condition.
- In grave condition, roll 2d6 each turn; if roll > PH, fall unconscious.
- At no wounds remaining, character is dead.
- Chest wound can wind a target: if 2d6 > PH, target loses actions for `(roll - PH)` turns.

### 6.7 Melee
- Melee also uses `1 / +1* / +2**` style dice scaling.
- Unarmed blow hits on 2-6.
- Weapon blow hits on 3-6.
- Better close-combat/weapon skills improve dice access.

### 6.8 Healing
No medical skill:
- Roll 1d6 each turn.
- Recover 1 wound on a 6.
- If bandages/equipment are used, recover on 5-6.

With medical skill:
- First Aid: roll 2d6, keep highest.
- Paramedic: roll 3d6, keep highest.
- Pharmaceutical Chemistry: roll 4d6, keep top two for potential two-wound recovery.

### 6.9 Initiative
- Player side starts with initiative when combat begins.
- Order among players is based on `2d6 + Mental Health`.

## 7. Special Opponent Rules

### Animal
- Special headshot behavior: only on 5-6 on d6.
- Headshot releases Psychnosis/Psychosis gas.
- Gas attack causes double-roll requirement and half movement.
- Gas cloud radius is `3d6 + 12` yards.
- Cloud shrinks by 6 yards per turn and drifts 6 yards in a d6-selected direction.
- Animal attacks get +1 die.

### Hunter
- +1 movement.

### Soldiers
- Every shot wound (including headshots) gets armor save on 5-6 (d6).

## 8. Special Weapons

### Shotgun
Treat shotgun fire as a cone with stepped damage:
- Up to 10y (2y width): 2d6 wounds.
- Else up to 20y (4y width): d6 wounds.
- Else up to 30y (6y width): d3 wounds.

### Grenades
- Throw range up to 50y.
- Base radius 3y.
- Damage bands:
  - 3y: d6.
  - 6y: d3.
  - 9y: d2.
- In enclosed structures (at least two walls), effective radius doubles.
- Throwing a grenade halves movement/other ability in that turn.
- If stationary, a character can throw two grenades in one turn.
- Reliability check per throw: roll 2d6; 4 or less means dud.
- Blast debris gives Some Cover through the blast area.

## 9. Character Record Sheets
The source PDF includes record sheets for:
- Character identity and loadout.
- Per-stat skill tracking.
- Repeated mini quick-reference blocks for weapon/ammo/health/movement-medical profile.

Use `docs/field-of-chaos-rules-extracted.md` as the canonical numeric/mechanical reference and this file as the easier-reading interpretation.
