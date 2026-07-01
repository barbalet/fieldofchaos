# Field Of Chaos 2018 Rewrite Plan

Current cycle: 8 / 40

Source of truth: `pdf/foc-just-the-rules-2018.pdf`

Reference-only PDFs to keep:

- `pdf/foc-just-the-rules-2024.pdf`
- `pdf/field-of-chaos-rpg-070524.pdf`
- `pdf/foc_rpg_051018.pdf`

Execution rule: when asked to "Run the next X cycles", complete cycles in order, update this file's current cycle value, and only advance past a cycle when its acceptance criteria are satisfied. If a later discovery invalidates a completed cycle, mark the affected cycle as needing redo in this file before continuing.

## Development Cycles

### Cycle 1 - Confirm Repository Baseline

Review the current tracked and untracked file set before any cleanup.

Acceptance criteria:

- `git status --short` has been recorded in the cycle notes.
- The existing top-level directories have been reviewed.
- No source files have been deleted in this cycle.

### Cycle 2 - Classify Keep, Rewrite, Remove

Create a precise inventory of files and directories to keep, rewrite, remove, or defer.

Acceptance criteria:

- `pdf/`, `src/`, `docs/`, `scripts/`, `AppShell/`, `FieldOfChaos.xcodeproj/`, `UITests/`, `packaging/`, `Package.swift`, `README.md`, `LICENSE`, `.github/`, and `tmp/` are classified.
- Any uncertain item is marked "defer" rather than removed.
- The classification is reflected in this plan or a temporary cycle note.

### Cycle 3 - Preserve PDF Sources

Ensure all required PDF rule sources are present and will survive cleanup.

Acceptance criteria:

- `pdf/foc-just-the-rules-2018.pdf` exists.
- `pdf/foc-just-the-rules-2024.pdf` exists.
- `pdf/field-of-chaos-rpg-070524.pdf` exists.
- `pdf/foc_rpg_051018.pdf` exists.
- No PDF listed above is deleted or renamed.

### Cycle 4 - Remove Temporary Generated Artifacts

Remove temporary generated artifacts that are not part of the final repository.

Acceptance criteria:

- `tmp/` is removed.
- No file outside `tmp/` is removed in this cycle.
- `git status --short` confirms only expected temp cleanup.

### Cycle 5 - Remove Swift App Source

Remove the SwiftUI/Metal application source because it is outside the 2018 C library scope.

Acceptance criteria:

- `AppShell/` is removed.
- No C source in `src/` is removed in this cycle.
- No PDF is removed.

### Cycle 6 - Remove Xcode Project

Remove Xcode project files and workspace state.

Acceptance criteria:

- `FieldOfChaos.xcodeproj/` is removed.
- No replacement Xcode project is added.
- The plan still identifies `make` as the target build system.

### Cycle 7 - Remove Swift Package Manifest

Remove Swift package configuration after Swift sources are gone.

Acceptance criteria:

- `Package.swift` is removed.
- No Swift build command remains documented as active project workflow.
- No C Makefile is removed in this cycle.

### Cycle 8 - Remove UI Tests

Remove the app-level exploratory UI test suite.

Acceptance criteria:

- `UITests/` is removed.
- No C tests are removed in this cycle.
- No UI test script remains active after later script cleanup.

### Cycle 9 - Remove App Packaging Assets

Remove app bundle packaging assets tied to the deleted Swift/Xcode app.

Acceptance criteria:

- `packaging/` is removed.
- No release app bundle workflow remains active.
- No C release packaging decision is made yet unless explicitly needed.

### Cycle 10 - Review Scripts

Review all scripts and decide whether any are still useful for the C-only project.

Acceptance criteria:

- Each file in `scripts/` is classified.
- Swift/Xcode/app scripts are marked for removal.
- Any future C-only helper script need is noted before removal.

### Cycle 11 - Clean Scripts Directory

Remove or reduce scripts based on Cycle 10.

Acceptance criteria:

- Swift/Xcode/app packaging scripts are removed.
- `scripts/` is either removed entirely or contains only C/PDF-relevant scripts.
- Any kept script runs without Swift/Xcode assumptions.

### Cycle 12 - Review Documentation Directory

Review docs and identify which files are 2024/playable-app artifacts.

Acceptance criteria:

- Each file in `docs/` is classified.
- 2024 extraction, app, Metal, UI, campaign, and cycle docs are marked for removal or archival.
- Any minimal 2018 traceability docs are identified before deletion.

### Cycle 13 - Clean Documentation Directory

Remove or reduce docs based on Cycle 12.

Acceptance criteria:

- 2024/playable-app docs are removed from active documentation.
- `docs/` is either removed entirely or contains only minimal 2018-relevant notes.
- No retained doc contradicts `pdf/foc-just-the-rules-2018.pdf` as source of truth.

### Cycle 14 - Freeze Old `src` For Rewrite

Review the current `src/` one last time and record what needs replacement.

Acceptance criteria:

- Current 2024-specific mechanics in `src/` are identified.
- Current source layout is marked as replace-in-place or remove-and-recreate.
- No new 2018 code is added before the rewrite shape is decided.

### Cycle 15 - Define Final C Source Layout

Create the final C-only `src/` directory structure.

Acceptance criteria:

- `src/include/` exists.
- `src/lib/` exists.
- `src/cli/` exists.
- `src/tests/` exists.
- The old Swift module map is gone.

### Cycle 16 - Create Public 2018 Header

Create the public C API header for the 2018 rules library.

Acceptance criteria:

- `src/include/fieldofchaos2018.h` exists.
- Public names use the `foc2018_` prefix.
- Header types include stats, skills, weapons, range bands, movement paces, wounds, hit locations, characters, attack results, and healing results.
- Header includes no 2024-only weapon or wound concepts.

### Cycle 17 - Implement Dice And Deterministic Seeding

Implement core dice utilities and deterministic seeding.

Acceptance criteria:

- `foc2018_seed` exists.
- `foc2018_roll_d6` exists.
- Internal helpers support rolling multiple d6.
- Tests can use deterministic seeds.

### Cycle 18 - Implement Stats And Character Generation

Implement 2018 stat bounds, caps, and generation helpers.

Acceptance criteria:

- Five stats are represented: RE, IR, AP, PH, ME.
- Initial cap validation supports 30.
- Long-term cap validation supports 40.
- Generation supports three d6, two inverted dice, one +2 value, four +1 values.
- Player choice is represented or exposed rather than silently erased.

### Cycle 19 - Implement Skill Representation

Implement weapon and healing skills relevant to the 2018 just-the-rules PDF.

Acceptance criteria:

- Firearm Basic and Firearm Advanced are represented.
- Close Combat, Improvised Weapon Use, and Clandestine Weapon Use are represented.
- First Aid and Paramedic are represented.
- Pharmaceutical Chemistry may be represented as a skill, but has no 2018 healing effect.
- No 2024 skill advancement table is implemented.

### Cycle 20 - Implement 2018 Weapons

Implement the 2018 weapon enum and weapon metadata.

Acceptance criteria:

- Sniper Rifle, Rifle, Carbine, Automatic, and SubMG exist.
- Shotgun and Grenade do not exist as core 2018 weapons.
- Weapon names round-trip through string helpers.

### Cycle 21 - Implement Weapon Ranges

Implement 2018 range bands in inches.

Acceptance criteria:

- Sniper Rifle ranges are 24, 36, 48 inches.
- Rifle ranges are 18, 27, 36 inches.
- Carbine ranges are 12, 18, 24 inches.
- Automatic ranges are 8, 12, 16 inches.
- SubMG ranges are 6, 9, 12 inches.
- No yard-based range helper remains in the public 2018 API.

### Cycle 22 - Implement Ranged Attack Thresholds

Implement 2018 ranged attack threshold logic.

Acceptance criteria:

- Close range hit threshold is 4.
- Standard range hit threshold is 10.
- Long range hit threshold is 16.
- Close head-shot threshold is 6.
- Standard head-shot threshold is 12.
- Long head-shot threshold is 18.

### Cycle 23 - Implement Ranged Modifiers

Implement 2018 ranged attack dice modifiers.

Acceptance criteria:

- Light cover applies -1 die.
- Heavy cover applies -2 dice.
- Standard movement applies -1 die.
- Fast movement applies -2 dice.
- Target Evade applies -1 die.
- Modifier cancellation behavior is documented and tested.

### Cycle 24 - Implement Weapon Fire Rates And Jam Rules

Implement shots per turn, clip size, reload timing, and jams.

Acceptance criteria:

- Standard clip size is 10 rounds.
- Rifle and Carbine can fire 2 shots with Firearm Advanced.
- Automatic fires 2 shots and jams on 1 on 2d6.
- SubMG fires 3 shots and jams on 1 on 1d6.
- Automatic and SubMG cannot produce head shots through their no-head-shot rule.
- Changing a clip takes 1 turn in exposed rule data or action results.

### Cycle 25 - Implement Called Head Shots

Represent the 2018 called head-shot procedure.

Acceptance criteria:

- Called head shot requires being called two moves prior.
- The firing character misses two moves before firing.
- Missed called head shots miss altogether.
- API or CLI exposes this rule clearly enough to test.

### Cycle 26 - Implement 2018 Movement

Implement movement paces in inches.

Acceptance criteria:

- Very Slow is 2 inches, +1 with Evade.
- Slow is 3 inches, +1 with Marching.
- Standard is 5 inches, +1 with Marching.
- Fast is 8 inches, +2 with Running.
- One disabled leg halves movement.
- Two disabled legs prevent movement.

### Cycle 27 - Implement 2018 Wound Model

Implement the 2018 body distribution and wound status.

Acceptance criteria:

- Head is 1W.
- Chest is 4W.
- Abdomen is 2W.
- Each arm is 1W.
- Each leg is 2W.
- Total default wounds are 13W.
- Body-only torso wounds are gone from the 2018 implementation.

### Cycle 28 - Implement Wound Effects

Implement 2018 wound consequences.

Acceptance criteria:

- One arm at 0W prevents firearm use.
- Head at 0W causes unconsciousness.
- Chest at 0W causes unconsciousness.
- Abdomen at 0W causes unconsciousness.
- Less than 6 total wounds marks grave condition.
- No wounds remaining marks dead.
- No 2024 grave-condition PH roll is implemented.
- No 2024 winded/chest-body rule is implemented.

### Cycle 29 - Implement Bullet Hit Locations

Implement 2018 bullet hit locations using 3d6.

Acceptance criteria:

- 3-8 maps to leg.
- 9-13 maps to chest.
- 14-15 maps to abdomen.
- 16-17 maps to arm.
- 18 maps to head.
- Odd/even side selection applies for arms and legs.
- Tests can force or verify each location class.

### Cycle 30 - Implement Melee Rules

Implement 2018 melee hit rules.

Acceptance criteria:

- Blow hits on 3-6.
- Weapon hits on 4-6.
- Relevant skill modifiers are represented explicitly.
- No 2024 2-6 or 3-6 melee thresholds remain.

### Cycle 31 - Implement Healing Rules

Implement 2018 healing.

Acceptance criteria:

- No medical skill rolls 1d6 and recovers on 6.
- Bandage/equipment recovers on 5-6.
- First Aid rolls 2d6 and uses the highest die.
- Paramedic rolls 3d6 and uses the highest die.
- Pharmaceutical Chemistry does not improve healing.

### Cycle 32 - Implement Combat Foe Rule Data

Implement 2018 combat-affecting foe rule data where useful.

Acceptance criteria:

- Animal head-shot gas rule is represented.
- Animal gas cloud uses d6 + 6 inches radius.
- Animal gas cloud shrinks 2 inches per turn and drifts 2 inches in d6 direction.
- Animal +1 attack die is represented.
- Hunter +1 movement is represented.
- Soldier armor save on 5-6 is represented.

### Cycle 33 - Build C CLI Program

Create the 2018 C command line program.

Acceptance criteria:

- `src/cli/fieldofchaos2018.c` exists.
- CLI links against the static library.
- CLI can print 2018 rule tables.
- CLI can run at least one deterministic attack example.
- CLI can run at least one deterministic healing example.

### Cycle 34 - Create Makefile

Create the final Makefile for the C static library, CLI, and tests.

Acceptance criteria:

- `make` builds the library and CLI.
- `make lib` builds `build/libfieldofchaos2018.a`.
- `make test` builds and runs the tests.
- `make clean` removes build artifacts.
- No Swift, Xcode, or Package.swift command is used.

### Cycle 35 - Write Unit Tests

Write C tests that lock the 2018 rules and prevent 2024 regression.

Acceptance criteria:

- Tests cover wounds, ranges, weapons, hit locations, movement, melee, healing, and stat caps.
- Tests assert no shotgun or grenade core weapon.
- Tests assert no body-only wound pool.
- Tests assert no yard-based rifle range.
- `make test` passes.

### Cycle 36 - Run Full C Verification

Run and record the full C build/test verification.

Acceptance criteria:

- `make clean` succeeds.
- `make` succeeds.
- `make test` succeeds.
- CLI smoke examples succeed.
- Failures, if any, are fixed before advancing.

### Cycle 37 - Final Directory Audit

Audit the cleaned repository for stale app or 2024 source references.

Acceptance criteria:

- No Swift source remains unless deliberately retained and documented.
- No Xcode project remains.
- No active doc says 2024 is canonical.
- No active source API names imply 2024 behavior.
- The 2024 PDFs remain only as reference PDFs.

### Cycle 38 - Rewrite README

Rewrite `README.md` for the 2018 C static library and CLI.

Acceptance criteria:

- README names `pdf/foc-just-the-rules-2018.pdf` as source of truth.
- README explains retained 2024 PDFs as reference-only.
- README documents the library layout.
- README documents `make`, `make test`, and `make clean`.
- README includes CLI examples.
- README contains no Swift/Xcode/app workflow.

### Cycle 39 - Final Consistency Pass

Perform a repository-wide consistency pass.

Acceptance criteria:

- `rg` finds no stale references to deleted app workflows in active files.
- `git status --short` is reviewed.
- `PLAN.md` accurately shows completed cycle state.
- Any discovered mismatch is assigned to a redo cycle before proceeding.

### Cycle 40 - Completion Signoff

Confirm the project is fully converted to the 2018 C-only scope.

Acceptance criteria:

- Current cycle is updated to 40 / 40.
- `make test` passes from `src/`.
- `README.md` is final for the new project shape.
- The retained PDFs are present.
- No known required cleanup remains.
- Final summary is ready for the user.

## Cycle Notes

- Cycle 0: Plan initialized. No cleanup or rewrite cycles have been run yet.
- Cycle 1: Baseline confirmed. `git status --short` showed `?? PLAN.md` and `?? tmp/`. Top-level directories reviewed: `packaging/`, `UITests/`, `pdf/`, `docs/`, `AppShell/`, `FieldOfChaos.xcodeproj/`, `scripts/`, `.github/`, `.git/`, `tmp/`, and `src/`. Top-level files reviewed: `.DS_Store`, `LICENSE`, `README.md`, `.gitignore`, `Package.swift`, and `PLAN.md`. No source files were deleted.
- Cycle 2: Classification completed. Keep: `pdf/`, `LICENSE`, `.gitignore`, `.github/`, and `PLAN.md`. Rewrite: `src/` and `README.md`. Remove in later cycles: `AppShell/`, `FieldOfChaos.xcodeproj/`, `UITests/`, `packaging/`, and `Package.swift`. Defer until specific review cycles: `docs/` and `scripts/`, because they may either be removed entirely or reduced to minimal 2018/C-only support. Remove later as generated/non-source material: `tmp/`, `.DS_Store`, `src/.DS_Store`, and current `src` build outputs (`src/fieldofchaos`, `src/character_create`, `src/engine/fieldofchaos_engine.o`, `src/tests/test_engine`). No files were deleted.
- Cycle 3: PDF sources preserved. Confirmed these files exist: `pdf/foc-just-the-rules-2018.pdf`, `pdf/foc-just-the-rules-2024.pdf`, `pdf/field-of-chaos-rpg-070524.pdf`, and `pdf/foc_rpg_051018.pdf`. `find pdf -maxdepth 1 -type f -print | sort` showed only those four PDF files. `git status --short pdf` was clean. No PDFs were deleted or renamed.
- Cycle 4: Temporary generated artifacts removed. Removed only `tmp/`, which contained generated/intermediate PDF extraction and render files. Verified `tmp/` no longer exists, `git status --short` shows only `?? PLAN.md`, and retained PDFs remain present. No files outside `tmp/` were removed.
- Cycle 5: Swift app source removed. Removed only `AppShell/`, including the SwiftUI app files and Metal shader under `AppShell/Shaders/`. Verified `AppShell/` no longer exists, `src/` C files remain present, and all retained PDFs remain present. `git status --short` shows deletions only under `AppShell/` plus the untracked `PLAN.md`.
- Cycle 6: Xcode project removed. Removed only `FieldOfChaos.xcodeproj/`, including project, workspace user state, and scheme management files. Verified `find . -name "*.xcodeproj" -print` returns no Xcode project bundles. Verified `PLAN.md` still identifies `make` as the target build system and no replacement Xcode project was added.
- Cycle 7: Swift package manifest removed. Removed `Package.swift` and verified `test ! -e Package.swift` passes. Verified `src/Makefile` remains present. Updated `README.md` to name `pdf/foc-just-the-rules-2018.pdf` as the active source of truth and to mark Swift/Xcode/app workflows as retired; `rg -n "swift (build|run|test)|xcodebuild" README.md` returns no active README commands. Historical docs/scripts still mention Swift/Xcode and remain deferred to their planned review cycles.
- Cycle 8: UI tests removed. Removed only `UITests/`, which contained `FieldOfChaosExploratoryUITests.swift`. Verified `UITests/` no longer exists and `src/tests/test_engine.c` plus the current `src/tests/test_engine` build artifact remain present. Verified `README.md` no longer documents an active UI test command; the remaining historical UI exploration script remains deferred to the scripts review/cleanup cycles.

## Classification Inventory

| Path | Classification | Notes |
|---|---|---|
| `pdf/` | Keep | Contains the 2018 source of truth and retained 2024/reference PDFs. |
| `src/` | Rewrite | Current C code targets 2024 mechanics and includes generated build outputs. |
| `docs/` | Defer | Review in cycles 12-13; likely remove or reduce to minimal 2018 traceability. |
| `scripts/` | Defer | Review in cycles 10-11; likely remove Swift/Xcode/app scripts, possibly recreate C-only helpers. |
| `AppShell/` | Remove | SwiftUI/Metal app source, outside C-only 2018 scope. |
| `FieldOfChaos.xcodeproj/` | Remove | Xcode project, outside Makefile C-only scope. |
| `UITests/` | Remove | App-level UI tests, outside C-only scope. |
| `packaging/` | Remove | App bundle packaging assets, outside C-only scope. |
| `Package.swift` | Remove | Swift package manifest, outside C-only scope. |
| `README.md` | Rewrite | Must be recreated last for the final 2018 C library/CLI shape. |
| `LICENSE` | Keep | Repository license. |
| `.github/` | Keep | Funding metadata can remain unless later repository policy says otherwise. |
| `.gitignore` | Keep | Should be updated later if new C build artifacts require ignore rules. |
| `tmp/` | Remove | Generated intermediate PDF/text/render artifacts. |
| `.DS_Store` files | Remove | Local filesystem metadata, not source. |
