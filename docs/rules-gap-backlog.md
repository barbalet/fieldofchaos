# Rules Coverage And Gap Backlog

This backlog compares the current C prototype against `docs/field-of-chaos-rules-extracted.md` for the first playable-game cycles.

## Implemented Or Partially Implemented

| Area | Current State |
|---|---|
| Character stats | Generation, stat storage, total validation, and JSON save/load exist. |
| Core skills | Firearm, movement, close-combat, evade, and medical flags exist as simplified booleans. |
| Ranged combat | Range dice, thresholds, cover/evade/running penalties, firearm bonuses, ammo, reload, and SubMG jam are implemented. |
| Hit locations | Bullet location table is implemented with 2d6 mapping. |
| Wounds | Body-part wounds, unconscious/dead states, chest wind, grave-condition checks, and armor saves exist. |
| Melee | Basic fallback melee exists in simplified form. |
| Shotgun | Damage by range band exists, but not board cone geometry. |
| Persistence | Character JSON exists and is now schema-versioned on save. |
| Logging | Structured combat events now exist for future dice/result UI. |

## Known Gaps Before Playable Release

| Priority | Gap | Notes |
|---|---|---|
| High | Board state | No grid, terrain, line of sight, cover geometry, deployment, or objective state yet. |
| High | SwiftUI gameplay UI | Cycle 10 only proves the app shell can call the C engine. |
| High | AI opponent | No tactical AI yet. |
| High | Scenario generation | No objective, terrain, or opposition generator yet. |
| High | Campaign advancement | No persistent mission history or post-battle advancement yet. |
| Medium | Skill percentages | Skills are booleans, while rules use skill slots and percentage advancement. |
| Medium | Starting stat cap | Rules distinguish initial cap 30 and long-term cap 40; current validation uses the long-term cap. |
| Medium | Movement timing | Movement rates exist in docs but are not yet part of a board action economy. |
| Medium | Healing | Healing rules are documented but not yet implemented in combat actions. |
| Medium | Grenades | Grenade range, reliability, radius, and debris cover are not implemented. |
| Medium | Special opponents | Animal, hunter, and soldier variants are only partially represented by armor/movement concepts. |
| Low | Dispute/house rules | Tabletop adjudication rules are documented, but not represented in game systems. |

## Rule Risk

The most important early risk is translating tabletop distances and turn language into a digital board without distorting weapon ranges, movement, and cover. Cycles 23-31 should create board math that asks the C rules engine for calculations instead of duplicating rule logic in Swift.

