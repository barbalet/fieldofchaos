# Field of Chaos RPG Rules (Extracted)

## Source
- Primary source PDF: `pdf/field-of-chaos-rpg-070524.pdf`
- Extracted text source used during this pass: `docs/field-of-chaos-rules-source.txt`
- Source pages used for rules extraction: pages 10-19 and 21-23.
- Non-rule opinion/background sections were intentionally excluded.

## 1. Core Play Structure

### Roles
- One `Controller` runs narration and adjudication.
- One to ten `Players` each run a `Character`.

### Materials
- At least five d6.
- Paper and pencil/pen for records.

### House Rules
- House rules are explicitly allowed and encouraged.
- House rules must be written down.
- All participants must agree to house rules before play starts.

### Dispute Resolution
- If a rule dispute cannot be resolved in about 2 minutes, resolve with a die roll.
- 2-way dispute:
  - One side picks odd, one picks even.
  - Roll 1d6; odd/even decides winner.
- 3-way dispute:
  - Each side picks 1, 2, or 3.
  - Roll 1d6.
  - 1 or 4 -> side `1` wins.
  - 2 or 5 -> side `2` wins.
  - 3 or 6 -> side `3` wins.
- More than 3 sides:
  - Reduce to 2 or 3 options (agreement or vote), then roll.

## 2. Character Statistics

### Primary stats
- Regular Intelligence (`RE`)
- Irregular Intelligence (`IR`)
- Appearance (`AP`)
- Physical Health (`PH`)
- Mental Health (`ME`)

### Limits
- Each stat is between `0` and `10`.
- Initial sum of all five stats must not exceed `30`.
- Long-term maximum combined stat total is `40`.

## 3. Statistic Generation
1. Roll 3d6 and record values: `x, y, z`.
2. Choose any two dice and take opposite faces (`7-x` and `7-y` for chosen dice).
3. You now have five numbers: `x, y, z, 7-a, 7-b`.
4. Add `+2` to one chosen number.
5. Add `+1` to the other four numbers.
6. Assign the five final numbers to `RE, IR, AP, PH, ME`.

Notes from source:
- Typical generated totals range from `21` to `26`.
- Statistics are expected to grow through play.

## 4. Skills

### Skill slots
- A character gets one skill slot per stat point in each stat area.
- Prerequisite skills must be taken as part of that accounting.

### Skill advancement
- Each time a skill is used, roll 1d6.
- If successful, increase that skill by `+1%`.

| Current Skill % | d6 Needed To Advance 1% |
|---|---|
| Less than 17% | 2-6 |
| 17% to 33% | 3-6 |
| 34% to 50% | 4-6 |
| 51% to 67% | 5-6 |
| Greater than 67% | 6 |

- Expanding from a parent skill into a derived skill requires additional work.

### Skill lists by stat

#### Regular Intelligence (`RE`)
- Anti-Viral Software (requires Computer Programming)
- Artificial Intelligence (requires Computer Programming)
- Biology, Basic
- Botany (requires Basic Biology and Horticulture)
- Cartography (requires Basic Geography and Visual Art)
- Chemistry, Basic
- Chemistry, Advanced (requires Basic Chemistry)
- Chemistry, Pharmaceutical (requires Advanced Chemistry and Basic Biology)
- Computer Operation
- Computer Programming (requires Computer Operation)
- Computer Repair
- Cryptography (requires Advanced Mathematics)
- Copyediting (requires Primary Written Language)
- Drive Tracked Vehicle (requires Drive Car)
- Electronics, Basic
- Electronics Repair (requires Basic Electronics)
- Electronics Operation (requires Basic Electronics)
- First Aid
- Geography, Basic
- Gun-Smithing, Basic (requires Advanced Firearm Use (any))
- History, Basic
- Horticulture
- Language Written, Primary (requires Primary Language Spoken)
- Language Written, Secondary (requires Secondary Language Spoken)
- Mathematics, Basic
- Mathematics, Advanced (requires Basic Mathematics)
- Mechanic, Auto Basic (requires Drive Car or Drive Motorcycle)
- Mechanic, Auto Advanced (requires Basic Auto Mechanic)
- Musical Performance
- Paramedic (requires First Aid)
- Psychology, Basic
- Zoology (requires Basic Biology)

#### Irregular Intelligence (`IR`)
- Computer Virus Creation (requires Computer Programming)
- Explosive Engineering (requires Basic Chemistry)
- Forgery (requires Visual Art)
- Lock-picking
- Hacking (requires Computer Operation)
- Hunting
- Manufacture Clandestine Weapon (requires Carpentry)
- Track (requires Disguise)
- Improvised Weapon Use, Basic
- Clandestine Weapon Use, Basic
- Wilderness Survival, Advanced (requires Basic Wilderness Survival)

#### Appearance (`AP`)
- Disguise
- Evade
- Etiquette (requires Basic Manners)
- Orate (requires Primary Spoken Language)
- Scouting (requires Basic Geography and Advanced Wilderness Survival)

#### Physical Health (`PH`)
- Canoeing
- Carpentry
- Close Combat
- Cycling
- Marching
- Running
- Sailboating
- Swimming
- Firearm Use, Basic (All firearms)
- Wilderness Survival, Basic
- Firearm Use, Shotgun (requires Basic Firearm Use)

#### Mental Health (`ME`)
- Drive Car
- Drive Motorcycle
- Firearm Use, Advanced (requires Basic Firearm Use) (specific to Rifle or SubMG)
- Firearm Use, Sniper (requires Firearm Use, Advanced (Rifle))
- Interrogation (requires Basic Psychology)
- Impersonate (requires Disguise)
- Language Spoken, Primary
- Language Spoken, Secondary (requires Primary Language Spoken)
- Manners, Basic
- Pilot Plane (requires Drive Car)
- Visual Art

## 5. Setting Defaults (Rules-Relevant)
- Two languages: `Majority` and `Elite`.

## 6. Combat Rules

### 6.1 Weapon range bands

| Weapon | Close Range | Standard Range | Long Range |
|---|---:|---:|---:|
| Sniper Rifle | 288 yards | 432 yards | 576 yards |
| Rifle | 216 yards | 324 yards | 432 yards |
| SubMG | 96 yards | 192 yards | - |

### 6.2 Ranged attack roll structure

Per range band, use this `x d6` and target:

| Range Band | x d6 | Required To Hit |
|---|---|---|
| Close | `1 / +1* / +2**` | 4-6 |
| Standard | `2 / +1* / +2**` | 8-12 |
| Long | `3 / +1* / +2**` | 14-18 |

Modifiers (each applies `-1d`):
- Some Cover
- Target Moving Fast
- Target Has Evade

Skill notes:
- `*` = Has Firearm Use, Basic (or Firearm Use, Advanced for Sniper Rifle).
- `**` = Has weapon-specific Firearm Use, Advanced (or Firearm Use, Sniper for Sniper Rifle).

### 6.3 Fire rate and movement

| Weapon | Shots Per Turn | Movement Rule | Special Rule |
|---|---|---|---|
| Sniper Rifle | 1 | Cannot move and fire | - |
| Rifle | 1 or 2** | Slow / Standard** | - |
| SubMG | 4 | Fast | Jams on any `1` in the used roll |

Ammo rules:
- Standard clip size: `10` rounds.
- Reloading a clip: `1` turn.
- Clearing a jam: `1` turn.

### 6.4 Movement rates

| Pace | Yards Per Second | Skill Effect |
|---|---|---|
| Slow | `2 / +1` | Has Evade Skill |
| Standard | `4 / +2` | Has Marching Skill |
| Fast | `7 / +3` | Has Running Skill |

### 6.5 Bullet hit location (critical formatting rule)
- Roll `2d6` for hit location.

| 2d6 Result | Location | Probability |
|---|---|---|
| 4 | Left Arm | 3/36 |
| 6 | Left Leg | 5/36 |
| 8 | Right Leg | 5/36 |
| 10 | Right Arm | 3/36 |
| 12 | Head | 1/36 |
| Any other result | Body Hit | 19/36 |

Important: a headshot from standard bullet-hit resolution is specifically `2d6 = 12`.

### 6.6 Wound pools by body part
- Head: `1W`
- Left Arm: `1W`
- Right Arm: `1W`
- Body: `4W`
- Left Leg: `2W`
- Right Leg: `2W`

Effects:
- One leg at 0W: movement halved.
- Both legs at 0W: cannot move.
- One arm at 0W: cannot fire a firearm.
- Head at 0W or Body at 0W: unconscious.
- Less than 6 total wounds remaining: grave condition.
  - Roll `2d6` each turn.
  - If roll > PH, character becomes unconscious.
- 0 wounds remaining overall: dead.
- Single wound to chest can wind the character:
  - Roll `2d6`.
  - If roll > PH, character cannot perform actions for `(roll - PH)` turns.

### 6.7 Melee hit rolls

| Check | Requirement |
|---|---|
| x d6 | `1 / +1* / +2**` |
| To hit from blow | 2-6 |
| To hit from weapon | 3-6 |

Skill notes:
- `*` = Close Combat (for blow or weapon) OR Clandestine/Improvised Weapon Use (where applicable).
- `**` = Close Combat (for weapon) AND Clandestine/Improvised Weapon Use (where applicable).

### 6.8 Healing
- No medical skills:
  - Roll 1d6 per turn.
  - On 6, recover 1 wound.
  - With bandage/medical equipment: recover on 5-6.
- First Aid:
  - Roll 2d6, keep highest.
- Paramedic:
  - Roll 3d6, keep highest.
- Pharmaceutical Chemistry:
  - Roll 4d6, keep top two dice.
  - Up to two wounds can recover depending on success thresholds (6 or 5-6 with equipment).

### 6.9 Initiative
- Player side has initiative when battle starts.
- Order between players: `2d6 + Mental Health`.

## 7. Foe-Specific Rules

### Animal
- Headshot releases Psychnosis Gas (spelling as in source).
- Animal headshot check: achieved only on `5-6` on d6.
- Gas attack: Psychnosis Gas (`double rolls required / half move`).
- Gas cloud radius: `3d6 + 12` yards.
- Cloud shrinks by `6` yards per turn.
- Cloud drift: moves 6 yards in d6-determined direction each turn.
- Animal gets `+1` die for all attacks.

### Hunter
- `+1` movement.

### Soldiers
- Every shot wound has armor save on `5-6` on d6, including headshots.

## 8. Weapon Special Rules

### Shotgun
- Blast cone is 45 yards long and 9 yards wide at the end.
- If any model is in first zone (10 yards long, 2 yards wide): each such model takes `2d6` wounds.
- Else if any model is in second zone (20 yards long, 4 yards wide): each such model takes `d6` wounds.
- Else if any model is in third zone (30 yards long, 6 yards wide): each such model takes `d3` wounds (`d6 / 2`, round up).

### Grenade
- Throw range: up to 50 yards.
- Base blast radius: 3 yards.
- Damage by distance:
  - Within 3 yards: `d6` wounds.
  - Within 6 yards: `d3` wounds (`d6 / 2`, round up).
  - Within 9 yards: `d2` wounds (`d6 / 3`, round down, resulting in 0-2).
- In building with at least two walls: effective radius is doubled.
- Throwing a grenade halves movement/other abilities for that turn.
- A stationary character can technically throw two grenades in a turn.
- Reliability check per thrown grenade: roll `2d6`; on `<= 4`, grenade is a dud.
- Explosion debris creates `Some Cover` for shots through blast area.

## 9. Character Sheet and Quick-Reference Templates

### Character sheet fields (pages 21-22)
- Name
- Primary weapon
- Ammunition
- Clips
- Secondary weapon
- Melee weapon
- Equipment notes
- RE/IR/AP/PH/ME values
- Skills listed per stat area

### Repeated quick-reference blocks (page 23)
Repeated five times:
- Weapon (None / Basic / Advanced)
- Clips (`1 4 1` shown in source)
- Rounds (`2 2` shown in source)
- Physical Health `6 (?)`
- Medical level (None / First Aid / Paramedic)
- Movement traits (Evade / Marching / Run)
