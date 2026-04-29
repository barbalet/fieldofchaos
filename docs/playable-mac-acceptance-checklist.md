# Playable Mac Acceptance Checklist

This checklist defines the cycle-100 target for the playable Mac build.

## Core Play

- The player can choose a player character, opponent, seed, difficulty, max turns, and AI mode before starting.
- Generated skirmishes always resolve to victory, defeat, or draw.
- Dead or unconscious actors cannot keep the turn loop alive.
- AI turns resolve to attack, reload, clear, move, or wait without stalling.
- The player can save and resume an active skirmish.

## Campaign

- Campaign state survives app relaunch through Application Support autosaves.
- Completed missions update history, XP, injuries, advances, and campaign roster records.
- Player character wounds and advancement carry forward into the roster.
- Campaign backups can be exported and imported.
- The campaign can generate the next mission from current progress.

## Mac App

- `FieldOfChaos.xcodeproj` builds the app target.
- `swift build` remains valid for package builds.
- C engine tests pass from `make test`.
- The app bundles sample characters and rule text for Xcode launches.
- The packaged app bundles sample characters, rule text, shaders, and `AppIcon.icns`.
- Preferences survive app relaunch and can set default AI, pace, automation, max turns, sound, reduced motion, and launch behavior.
- The Play surface provides entry points to continue, generate, tutorial, campaign, rules, and scenario setup.
- Keyboard commands cover the core Mac loop: generate, save/load skirmish, wait, reload, clear jam, and AI turn.
- The board exposes an accessibility summary with scenario, dimensions, turn, actor sides, and actor positions.
- `scripts/smoke_playable.sh` verifies the C engine, Swift package, Xcode app, package script, and packaged resources.
