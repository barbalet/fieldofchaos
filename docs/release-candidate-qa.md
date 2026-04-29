# Release Candidate QA

Cycle 59 and 60 smoke checklist:

1. Build C tests with `cd src && make test`.
2. Build Swift app with `swift build`.
3. Run a deterministic CLI duel with `--event-log`.
4. Start the tutorial duel.
5. Use Back, Next, Rule, Restart, Skip, and Practice Fight.
6. Generate a skirmish scenario, move, attack, wait, reload, heal, grenade, and run AI.
7. Complete a skirmish and advance campaign.
8. Export a character, dice log, and campaign backup.
9. Open the rules overlay from tutorial, log, and skirmish.
10. Package the app with `sh scripts/package_release.sh`.

Release candidate acceptance:

- The app bundle exists at `dist/Field of Chaos.app`.
- The C engine, SwiftUI app, and Metal renderer build together.
- JSON exports are valid JSON/JSON-lines.
- AI turns always resolve to an action or wait instead of stalling.
- Application Support contains recoverable log and campaign snapshots after play.

Cycle 100 smoke checklist:

1. Run `sh scripts/smoke_playable.sh`.
2. Confirm `dist/Field of Chaos.app/Contents/Resources/AppIcon.icns` exists.
3. Confirm `dist/Field of Chaos.app/Contents/Resources/alice.json` exists.
4. Confirm `dist/Field of Chaos.app/Contents/Resources/bob.json` exists.
5. Confirm `dist/Field of Chaos.app/Contents/Resources/field-of-chaos-rules-natural-english.md` exists.
6. Change settings and confirm preferences are saved under Application Support.
7. Relaunch with Play-on-launch enabled and confirm the Play surface opens first.
