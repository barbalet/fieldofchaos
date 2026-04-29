# Campaign System

Cycles 71-80 turn campaign play from a result summary into a persistent loop.

## State

The campaign stores:

- Mission totals, wins, losses, XP, advances, injuries, and last summary.
- Mission history with seed, difficulty, objective, rewards, and injury count.
- Campaign roster records with missions, XP earned, injuries, recovery, and last result.

Autosave path:

```text
Application Support/FieldOfChaos/campaign-autosave.json
```

## Mission Flow

1. Choose campaign roster members in Scenario.
2. Generate a campaign mission.
3. Play the skirmish to victory, defeat, or draw.
4. Advance the campaign from the skirmish result.
5. Carry the player character's wounds and state back into the roster.
6. Spend available advances on PH, ME, Firearm Advanced, Evade, or Running.

## Consequences

- Wins grant 3 XP and one advancement.
- Losses and draws grant 1 XP.
- Wounds lost during the skirmish add campaign injuries.
- Injuries create short recovery timers in the campaign roster.
- Each mission ticks existing recovery down by one.

## Backup

Campaign backup export/import round-trips the whole campaign state, including mission history and roster records.
