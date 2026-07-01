# Field Of Chaos Gold Rewrite Plan

Current cycle: 40 / 40

Source of truth: `pdf/foc-just-the-rules-gold.pdf`

Reference-only PDFs to keep:

- `pdf/foc-just-the-rules-2018.pdf`
- `pdf/foc-just-the-rules-2024.pdf`
- `pdf/field-of-chaos-rpg-070524.pdf`
- `pdf/foc_rpg_051018.pdf`

Execution rule: when asked to "Run the next X cycles", complete cycles in order, update this file's current cycle value, and only advance past a cycle when its acceptance criteria are satisfied. If a later discovery invalidates a completed cycle, mark the affected cycle as needing redo in this file before continuing.

Gold pivot note: Cycles 1-20 were completed against the pure 2018 just-the-rules PDF. After Cycle 20, the source of truth changed to Gold: the 2018 core rules plus the agreed Shotgun and Grenade rules adapted from 2024. Cycle 21 must audit the remaining 2018 references, naming, and pure-2018 test expectations before implementation resumes. Cycles 22-40 must update the code, tests, CLI, Makefile, and README to match Gold.

## Gold Rule Decisions

- `pdf/foc-just-the-rules-gold.pdf` is the active source of truth.
- Gold keeps the 2018 stats, character generation, skill accounting, ranged thresholds, ranged modifiers, weapon ranges, movement paces, wound model, bullet hit locations, melee thresholds, healing rules, called head shots, and combat foe rules unless explicitly listed below.
- Shotgun is covered by Firearm Use, Basic. Gold does not add a separate Shotgun skill.
- Grenade has no skill requirement. Explosive Engineering exists as a skill but is not relevant to the Gold grenade rule.
- No skill modifies grenade throw range, reliability, dud chance, blast radius, or damage.
- Shotgun cone total length is 15 inches, with bands of 5 inches / 1 inch / 2d6 wounds, 10 inches / 2 inches / d6 wounds, and 15 inches / 3 inches / d3 wounds.
- Grenade maximum throw range is 12 inches.
- Grenade radius bands are 1 inch / d6 wounds, 3 inches / d3 wounds, and 4 inches / d2 wounds.
- Grenade in a building with at least two walls uses doubled-radius bands of 3 inches, 6 inches, and 9 inches.
- Grenade reliability is 2d6 each throw; on 4 or less, the grenade is a dud and does not fire.
- Grenade debris creates Light cover for shots through the blast area.
- Shotgun and grenade damage can affect all models in the affected area, including allies.
- Shotgun applies damage only to models in the nearest occupied cone band.
- Each shotgun or grenade wound is allocated by the normal 2018 random 3d6 hit-location procedure.
- Throwing one grenade halves movement for the turn, rounded down.
- A stationary character may throw two grenades; throwing two grenades consumes the turn and prevents movement, firearm attack, melee attack, reload, jam clearing, healing, and called-head-shot progress in that turn.
- Gold excludes the 2024 use-based skill advancement table, Pharmaceutical Chemistry healing effect, ranged thresholds, SubMG rewrite, body-only wound pool, grave-condition PH roll, winded/chest-body rule, and removal of the 2018 called-head-shot procedure.

## Gold Naming Decision

Cycle 21 decision: active source files, public API symbols, library names, CLI names, tests, and Makefile outputs should migrate to Gold naming: `fieldofchaosgold` for file/library/CLI names and `focgold_` for public API symbols. The project should not retain `fieldofchaos2018` / `foc2018_` as active compatibility names unless a later cycle deliberately adds and tests a compatibility shim.

`2018` remains valid only for retained reference PDFs, historical cycle notes, and explanatory text that identifies rules inherited from the 2018 core.

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

- `pdf/foc-just-the-rules-gold.pdf` exists.
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
- No retained doc contradicts the active source-of-truth PDF.

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

### Cycle 21 - Audit 2018 References Before Gold Migration

Check all remaining 2018 references after the Gold pivot and decide which are historical, which are still valid compatibility names, and which must migrate to Gold naming or behavior.

Acceptance criteria:

- `rg --files` and targeted `rg -n "2018|foc2018|fieldofchaos2018|libfieldofchaos2018|fieldofchaosgold|focgold|Makefile"` results are reviewed.
- Every remaining 2018 reference is classified as a retained reference PDF, historical cycle note, compatibility API/source name, pure-2018 test expectation needing Gold update, or stale active reference.
- A naming decision is recorded in this plan for whether the source/API should migrate to `fieldofchaosgold` / `focgold_` or retain `fieldofchaos2018` / `foc2018_` compatibility names while implementing Gold behavior.
- Tests that currently assert pure-2018 behavior, especially Shotgun and Grenade rejection, are identified for Gold update.
- No rules implementation is changed in this cycle; this is a review and plan-correction cycle.
- Current cycle advances only after the audit result and naming decision are recorded in the cycle notes.

### Cycle 22 - Implement Gold Weapon Ranges, Special Geometry, And Ranged Thresholds

Implement 2018 firearm range bands in inches, Gold Shotgun and Grenade geometry, and 2018 ranged attack threshold logic.

Acceptance criteria:

- Sniper Rifle ranges are 24, 36, 48 inches.
- Rifle ranges are 18, 27, 36 inches.
- Carbine ranges are 12, 18, 24 inches.
- Automatic ranges are 8, 12, 16 inches.
- SubMG ranges are 6, 9, 12 inches.
- Shotgun is added as a Gold firearm covered by Firearm Use, Basic.
- Shotgun cone total length is 15 inches.
- Shotgun damage bands are 5 inches / 1 inch / 2d6, 10 inches / 2 inches / d6, and 15 inches / 3 inches / d3.
- Grenade is represented as a Gold explosive action, not a standard firearm range-band weapon.
- Grenade throw range is 12 inches.
- Grenade radius bands are 1 inch / d6, 3 inches / d3, and 4 inches / d2.
- Grenade building doubled-radius bands are 3 inches, 6 inches, and 9 inches.
- No yard-based range helper remains in the public Gold API.
- Close range hit threshold is 4.
- Standard range hit threshold is 10.
- Long range hit threshold is 16.
- Close head-shot threshold is 6.
- Standard head-shot threshold is 12.
- Long head-shot threshold is 18.

### Cycle 23 - Implement Ranged Modifiers

Implement Gold ranged attack dice modifiers.

Acceptance criteria:

- Light cover applies -1 die.
- Heavy cover applies -2 dice.
- Standard movement applies -1 die.
- Fast movement applies -2 dice.
- Target Evade applies -1 die.
- Grenade debris creates Light cover for shots through the blast area.
- Modifier cancellation behavior is documented and tested.

### Cycle 24 - Implement Weapon Fire Rates And Jam Rules

Implement shots per turn, clip size, reload timing, and jams.

Acceptance criteria:

- Standard clip size is 10 rounds.
- Rifle and Carbine can fire 2 shots with Firearm Advanced.
- Shotgun has 1 cone attack and uses Firearm Use, Basic.
- Automatic fires 2 shots and jams on 1 on 2d6.
- SubMG fires 3 shots and jams on 1 on 1d6.
- Automatic and SubMG cannot produce head shots through their no-head-shot rule.
- Changing a clip takes 1 turn in exposed rule data or action results.
- Clearing a jam takes 1 turn in exposed rule data or action results.
- Grenade reliability rolls 2d6 each throw and is a dud on 4 or less.
- No skill modifies grenade reliability.

### Cycle 25 - Implement Called Head Shots

Represent the 2018 called head-shot procedure.

Acceptance criteria:

- Called head shot requires being called two moves prior.
- The firing character misses two moves before firing.
- Missed called head shots miss altogether.
- API or CLI exposes this rule clearly enough to test.

### Cycle 26 - Implement Gold Movement

Implement movement paces in inches plus Gold grenade action economy.

Acceptance criteria:

- Very Slow is 2 inches, +1 with Evade.
- Slow is 3 inches, +1 with Marching.
- Standard is 5 inches, +1 with Marching.
- Fast is 8 inches, +2 with Running.
- One disabled leg halves movement.
- Two disabled legs prevent movement.
- Throwing one grenade halves movement for the turn, rounded down.
- One grenade movement values are Very Slow 1 inch, Slow 1 inch, Standard 2 inches, and Fast 4 inches.
- A stationary character can throw two grenades.
- Throwing two grenades consumes the turn and prevents movement, firearm attack, melee attack, reload, jam clearing, healing, and called-head-shot progress in that turn.

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

### Cycle 29 - Implement Bullet And Blast Hit Locations

Implement 2018 bullet hit locations using 3d6, and use them for Gold shotgun/grenade wound allocation.

Acceptance criteria:

- 3-8 maps to leg.
- 9-13 maps to chest.
- 14-15 maps to abdomen.
- 16-17 maps to arm.
- 18 maps to head.
- Odd/even side selection applies for arms and legs.
- Tests can force or verify each location class.
- Each shotgun or grenade wound uses the normal 2018 random 3d6 hit-location procedure.

### Cycle 30 - Implement Melee Rules

Implement 2018 melee hit rules.

Acceptance criteria:

- Blow hits on 3-6.
- Weapon hits on 4-6.
- Relevant skill modifiers are represented explicitly.
- No 2024 2-6 or 3-6 melee thresholds remain.

### Cycle 31 - Implement Healing Rules

Implement Gold healing.

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
- Shotgun wounds count as shot wounds for Soldier armor save.
- Grenade wounds do not count as shot wounds unless a referee ruling says otherwise.

### Cycle 33 - Build C CLI Program

Create the Gold C command line program.

Acceptance criteria:

- `src/cli/fieldofchaosgold.c` exists.
- CLI links against the static library.
- CLI can print Gold rule tables.
- CLI can run at least one deterministic attack example.
- CLI can run at least one deterministic shotgun or grenade example.
- CLI can run at least one deterministic healing example.

### Cycle 34 - Create Makefile

Create the final Makefile for the C static library, CLI, and tests.

Acceptance criteria:

- `make` builds the library and CLI.
- `make lib` builds `build/libfieldofchaosgold.a`.
- `make test` builds and runs the tests.
- `make clean` removes build artifacts.
- No Swift, Xcode, or Package.swift command is used.

### Cycle 35 - Write Unit Tests

Write C tests that lock the Gold rules and prevent unwanted 2024 regression.

Acceptance criteria:

- Tests cover wounds, ranges, weapons, hit locations, movement, melee, healing, and stat caps.
- Tests cover Shotgun cone bands and Grenade throw/radius/dud/action-economy rules.
- Tests assert Grenade is not treated as a standard firearm range-band weapon.
- Tests assert no body-only wound pool.
- Tests assert no yard-based rifle range.
- Tests assert no 2024 skill advancement, Pharmaceutical Chemistry healing, SubMG rewrite, 2024 ranged thresholds, grave-condition PH roll, or winded/chest-body rule.
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
- No active doc says 2018-only or 2024-only rules are canonical.
- No active source API, file, library, CLI, or Makefile output names use `2018` unless they are retained reference PDFs, historical notes, or deliberately documented compatibility shims.
- The 2024 PDFs remain only as reference PDFs.

### Cycle 38 - Rewrite README

Rewrite `README.md` for the Gold C static library and CLI.

Acceptance criteria:

- README names `pdf/foc-just-the-rules-gold.pdf` as source of truth.
- README explains retained 2018 and 2024 PDFs as reference-only.
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

Confirm the project is fully converted to the Gold C-only scope.

Acceptance criteria:

- Current cycle is updated to 40 / 40.
- `make test` passes from `src/`.
- `README.md` is final for the new project shape.
- The retained PDFs are present.
- `pdf/foc-just-the-rules-gold.pdf` is present.
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
- Cycle 9: App packaging assets removed. Removed only `packaging/`, which contained `AppIcon.icns` and `Info.plist` for the retired app bundle. Verified `packaging/` no longer exists. Verified active entry points (`README.md` and `.github/`) do not document or run a release app bundle workflow; remaining script/doc references are historical and deferred to the upcoming script and docs review cycles. No C release packaging decision was introduced.
- Cycle 10: Scripts reviewed and classified. Reviewed `scripts/run_ui_exploration.sh`, `scripts/generate_app_icon.swift`, `scripts/package_release.sh`, and `scripts/smoke_playable.sh`; all four are Swift/Xcode/app-bundle workflow scripts and are marked for removal in Cycle 11. No current script is useful for the 2018 C-only library/CLI project. Future C-only helper needs should be recreated later if useful: a focused PDF extraction/traceability helper, a C build/test helper, or a rule-table generation helper.
- Cycle 11: Scripts directory cleaned. Removed `scripts/` entirely because Cycle 10 found no C/PDF-relevant scripts to keep. Verified `scripts/` no longer exists and `README.md` has no active script, Swift, Xcode, package, UI exploration, or app packaging command references. Updated `README.md` wording so only historical docs, not scripts, are described as awaiting review.
- Cycle 12: Documentation reviewed and classified. Reviewed every file currently under `docs/`: architecture, sample JSON, cycle logs, rules backlog, UI/Metal/campaign/release/playable docs, and the extracted/natural-language rules documents. All docs are either derived from the 2024 rules extraction/prototype or tied to the retired Swift/Xcode/Metal/playable-app direction. No minimal 2018 traceability doc was found; the active 2018 source of truth remains `pdf/foc-just-the-rules-2018.pdf`. `docs/` is marked for removal in Cycle 13, with any future 2018 traceability document to be recreated deliberately from the 2018 PDF.
- Cycle 13: Documentation directory cleaned. Removed `docs/` entirely because Cycle 12 found no minimal 2018-relevant traceability docs to keep. Updated `README.md` so it no longer references deleted docs or sample JSON files under `docs/`; examples now use `/tmp/foc-alice.json` and `/tmp/foc-bob.json`. Verified `docs/` no longer exists, active entry points no longer reference deleted docs, and no retained documentation contradicts `pdf/foc-just-the-rules-2018.pdf` as source of truth.
- Cycle 14: Old `src/` frozen for rewrite. Reviewed the existing C header, implementation, CLIs, tests, Makefile, Swift module map, and build outputs. Current source is a 2024/playable-app prototype with JSON character persistence, event logs, snapshots, event buffers, board helpers, movement paces, targeting previews, scenario generation, AI actions, campaign advancement, grenades, healing, and Swift bridge support. The old source layout is marked remove-and-recreate inside `src/` during Cycle 15; no current file should be carried forward as-is. No 2018 code was added in this cycle.
- Cycle 15: Final C source layout defined. Removed the old `src/` tree and recreated only `src/include/`, `src/lib/`, `src/cli/`, and `src/tests/`, each with a `.gitkeep` placeholder so the skeleton can be committed before implementation files are added. Verified the old Swift `module.modulemap` is gone and `src/.DS_Store` is absent. Updated `README.md` to stop documenting deleted prototype source files and commands. No 2018 rules implementation was added in this cycle.
- Cycle 16: Public 2018 header created. Added `src/include/fieldofchaos2018.h` and removed the `src/include/.gitkeep` placeholder. The header uses `foc2018_` public names and defines 2018 stats, skills, weapons, range bands, movement paces, wounds, hit locations, characters, attack results, melee results, and healing results. It models the 2018 weapon list as Sniper Rifle, Rifle, Carbine, Automatic, and SubMG; wound pools as head, chest, abdomen, arms, and legs; movement in inches; and healing as no skill, bandage, First Aid, and Paramedic. Verified with `cc -std=c11 -Wall -Wextra -pedantic -fsyntax-only` and checked that stale 2024/app terms such as shotgun, grenade, yards, body-only wounds, JSON, events, buffers, scenarios, campaign, Swift, and Xcode are absent from the header.
- Cycle 17: Dice and deterministic seeding implemented. Added `src/lib/fieldofchaos2018.c` with `foc2018_seed` and `foc2018_roll_d6`, backed by a small deterministic LCG rather than platform `rand()`. Added internal helper declarations in `src/lib/fieldofchaos2018_internal.h` for `foc2018_roll_d6_sum` and `foc2018_roll_d6_highest`. Added `src/tests/test_dice.c` so tests can verify repeated seeded d6 sequences plus sum/highest helper behavior before the final Makefile exists. Verified with `cc -std=c11 -Wall -Wextra -pedantic -Isrc/include -Isrc/lib src/lib/fieldofchaos2018.c src/tests/test_dice.c -o /tmp/foc2018_test_dice` followed by `/tmp/foc2018_test_dice`.
- Cycle 18: Stats and character generation implemented. Added stat total, initial cap validation at 30, long-term cap validation at 40, and `foc2018_generate_stats`. Updated `foc2018_stat_generation_choice` with `stat_value_indices[5]` so the player's assignment of the five final values to RE, IR, AP, PH, and ME is represented explicitly. Generation now validates three d6 values, two distinct inverted dice, one +2 final-value index, and a five-value stat assignment permutation before applying the 2018 +2/+1 bonus procedure. Added `src/tests/test_stats.c` for caps, bounds, valid generation, reassignment behavior, and invalid choices. Verified dice and stat tests with direct `cc` builds because the final Makefile is planned for Cycle 34.
- Cycle 19: Skill representation implemented from `pdf/foc-just-the-rules-2018.pdf`. Added stat and skill count sentinels, stat names, skill names, skill percentage validation, enum-based skill value lookup, skill-to-stat metadata, direct prerequisite arrays, and `foc2018_skill_improves_healing`. Represented Firearm Use Basic/Advanced, Close Combat, Improvised Weapon Use Basic, Clandestine Weapon Use Basic, First Aid, Paramedic, and the pharmaceutical chemistry chain. Pharmaceutical Chemistry is represented but does not improve 2018 healing. Added `src/tests/test_skills.c` for names, stats, prerequisites, percentage bounds, and healing relevance. Verified dice, stat, and skill tests with direct `cc` builds; `rg -n "advancement|experience|use-based|use based|XP" src/include src/lib` returned no matches.
- Cycle 20: 2018 weapon metadata implemented from `pdf/foc-just-the-rules-2018.pdf`. Added a weapon count sentinel, `foc2018_weapon_name`, and `foc2018_parse_weapon` for Sniper Rifle, Rifle, Carbine, Automatic, and SubMG. Added `src/tests/test_weapons.c` to verify the five-weapon list, name parsing round-trips, invalid inputs, and rejection of Shotgun and Grenade as core 2018 weapons. Verified dice, stat, skill, and weapon tests with direct `cc` builds; `rg -n "foc2018_weapon_(shotgun|grenade)" src/include src/lib` returned no matches.
- Gold pivot after Cycle 20: Created `pdf/foc-just-the-rules-gold.pdf` as the new source of truth. Gold keeps the 2018 core rules and adds agreed Shotgun and Grenade rules adapted from 2024: Shotgun is covered by Basic Firearm Use with a 15-inch cone and 5/10/15-inch damage bands; Grenade has 12-inch throw range, 1/3/4-inch radius bands, 3/6/9-inch building bands, 2d6 dud on 4 or less, Light cover debris, random 2018 hit-location wound allocation, friendly fire, and the grenade action-economy movement rule. Cycle 21 is reserved for auditing the remaining 2018 references and deciding Gold naming before code migration resumes in Cycle 22. Cycle 20 remains a historical completed cycle and will be superseded by upcoming Gold weapon work.
- Cycle 21: 2018 reference audit completed. `git status --short` was clean at the start of the cycle. `rg --files` showed the active tree now consists of `PLAN.md`, `README.md`, retained PDFs, `LICENSE`, and the C source/test files under `src/`. The targeted scan `rg -n "2018|foc2018|fieldofchaos2018|libfieldofchaos2018|fieldofchaosgold|focgold|Makefile"` was reviewed; match counts were `src/include/fieldofchaos2018.h` 158, `src/lib/fieldofchaos2018.c` 125, `src/lib/fieldofchaos2018_internal.h` 4, `src/tests/test_skills.c` 65, `src/tests/test_stats.c` 36, `src/tests/test_weapons.c` 26, `src/tests/test_dice.c` 20, `PLAN.md` 81, `README.md` 4, and `pdf/foc-just-the-rules-2018.pdf` 1. Classification: retained reference PDF references are valid; Cycle 0-20 notes are historical; README and plan text that say Gold keeps the 2018 core are valid explanatory text; `src/include/fieldofchaos2018.h`, `src/lib/fieldofchaos2018.c`, `src/lib/fieldofchaos2018_internal.h`, and `foc2018_` symbols in tests are pre-pivot active names that must migrate to `fieldofchaosgold` / `focgold_`; `src/tests/test_weapons.c` currently asserts pure-2018 behavior by expecting five weapons and rejecting Shotgun/Grenade, and must be updated for Gold in Cycle 22. Naming decision recorded above: active source/API/library/CLI/Makefile/test names should migrate to Gold naming; `2018` should remain only for retained PDFs, historical notes, and explicit inherited-rule explanations. No rules implementation was changed in this cycle.
- Cycle 22: Gold naming, weapon ranges, Shotgun/Grenade geometry, and ranged thresholds implemented. Renamed active C files and symbols from `fieldofchaos2018` / `foc2018_` to `fieldofchaosgold` / `focgold_`. Added Shotgun as a Gold firearm covered by Firearm Use, Basic, with 15-inch cone geometry and 5/10/15-inch damage bands of 2d6/d6/d3. Grenade is represented as a separate explosive action rather than a standard firearm range-band weapon, with 12-inch throw range, 1/3/4-inch open radii, 3/6/9-inch building radii, and d6/d3/d2 damage bands. Implemented 2018 inherited firearm range bands and hit/head-shot thresholds. Verified no active `foc2018`, `fieldofchaos2018`, or yard-based helper remains under `src/`.
- Cycle 23: Gold ranged modifiers implemented. Light cover applies -1 die, Heavy cover applies -2 dice, Standard movement applies -1 die, Fast movement applies -2 dice, and target Evade applies -1 die. Grenade debris is modeled as Light cover for shots through the blast area; it upgrades no-cover shots to Light cover and does not stack beyond existing Heavy cover. Stacking behavior is covered in `src/tests/test_gold_actions.c`.
- Cycle 24: Weapon fire rates, reload/jam metadata, and grenade reliability implemented. Standard clip size remains 10 rounds. Rifle and Carbine fire 2 shots with Firearm Advanced; Shotgun fires 1 cone attack; Automatic fires 2 shots and exposes a 2d6 jam roll with threshold 1; SubMG fires 3 shots and exposes a 1d6 jam roll with threshold 1. Automatic and SubMG cannot produce head shots. Changing a clip and clearing a jam each expose a 1-turn cost. Grenade reliability rolls 2d6 and is a dud on 4 or less; no skill modifies grenade reliability.
- Cycle 25: Called head-shot setup metadata implemented. The API exposes the two-move setup requirement, readiness after two moves, and the rule that missed called head shots miss altogether.
- Cycle 26: Gold movement and grenade action economy implemented. Movement paces are Very Slow 2 inches with +1 from Evade, Slow 3 inches with +1 from Marching, Standard 5 inches with +1 from Marching, and Fast 8 inches with +2 from Running. One disabled leg halves movement rounded down; two disabled legs prevent movement. Throwing one grenade halves movement rounded down. A stationary character can throw two grenades; the two-grenade turn permissions prevent movement, firearm attack, melee attack, reload, jam clearing, healing, and called-head-shot progress. Verified with direct `cc -std=c11 -Wall -Wextra -pedantic` builds and runs of `/tmp/focgold_test_dice`, `/tmp/focgold_test_stats`, `/tmp/focgold_test_skills`, `/tmp/focgold_test_weapons`, and `/tmp/focgold_test_gold_actions`.
- Cycle 27: 2018 inherited wound model implemented for Gold. `focgold_default_wounds` now assigns Head 1W, Chest 4W, Abdomen 2W, each Arm 1W, and each Leg 2W for a 13W total. The API uses explicit body parts and has no body-only torso wound pool. Added `src/tests/test_wounds.c`.
- Cycle 28: Wound effects implemented. One arm at 0W prevents firearm use; Head, Chest, or Abdomen at 0W causes unconsciousness; less than 6 total wounds and more than 0 total wounds marks grave condition; no wounds remaining marks dead. Verified the implementation contains no 2024 grave-condition PH roll and no winded/chest-body rule.
- Cycle 29: Bullet and blast hit locations implemented. Forced 3d6 total mapping is exposed for tests: 3-8 legs with odd/even side selection, 9-13 chest, 14-15 abdomen, 16-17 arms with odd/even side selection, and 18 head. Random bullet locations roll 3d6 through that mapping, and Gold blast locations for Shotgun/Grenade use the same 3d6 procedure. Added `src/tests/test_hit_locations.c`.
- Cycle 30: 2018 inherited melee rules implemented for Gold. Blow threshold is 3 on d6, Weapon threshold is 4 on d6, and 2024 2-6 / 3-6 melee thresholds are not used. Relevant skill modifiers are explicit: Close Combat applies to blows, while Improvised Weapon Use Basic and Clandestine Weapon Use Basic apply to melee weapon attacks. Added melee coverage in `src/tests/test_melee_healing.c`.
- Cycle 31: Gold healing implemented. No-skill healing rolls 1d6 and recovers on 6; bandage/equipment rolls 1d6 and recovers on 5-6; First Aid rolls 2d6 and uses the highest die; Paramedic rolls 3d6 and uses the highest die. Pharmaceutical Chemistry remains represented as a skill but does not improve healing. Verified with direct `cc -std=c11 -Wall -Wextra -pedantic` builds and runs of `/tmp/focgold_test_dice`, `/tmp/focgold_test_stats`, `/tmp/focgold_test_skills`, `/tmp/focgold_test_weapons`, `/tmp/focgold_test_gold_actions`, `/tmp/focgold_test_wounds`, `/tmp/focgold_test_hit_locations`, and `/tmp/focgold_test_melee_healing`.
- Cycle 32: Combat foe rule data implemented. Animal head-shot gas, d6+6 gas radius, 2-inch shrink, 2-inch d6-direction drift, Animal +1 attack die, Hunter +1 movement, Soldier 5-6 armor save, Shotgun-as-shot armor treatment, and default Grenade-not-shot armor treatment are exposed in the public API. Added `src/tests/test_foes.c`.
- Cycle 33: Gold CLI program created at `src/cli/fieldofchaosgold.c`. It links against the static library through the Makefile and can print rule tables plus deterministic `attack`, `blast`, and `heal` examples.
- Cycle 34: Final `src/Makefile` created. `make` builds `build/libfieldofchaosgold.a` and `build/fieldofchaosgold`; `make lib` builds the static library; `make test` builds and runs all focused C tests; `make clean` removes `src/build`.
- Cycle 35: Unit and regression coverage completed. Tests cover wounds, ranges, weapons, hit locations, movement, melee, healing, stat caps, Shotgun cone bands, Grenade throw/radius/dud/action economy, Grenade not being a standard firearm, no body-only wound pool, no yard-based rifle range, no 2024 skill advancement side effects, no Pharmaceutical Chemistry healing, no SubMG rewrite, no 2024 ranged thresholds, no grave-condition PH roll, and no winded/chest-body rule. `make test` passes.
- Cycle 36: Full C verification completed from `src/`. `make clean`, `make`, and `make test` all succeeded. CLI smoke examples succeeded for `tables`, `attack`, `blast`, and `heal`.
- Cycle 37: Final directory audit completed. No Swift files or Xcode project remain. Active files under `README.md`, `src/`, and `.github/` do not reference retired app workflows or stale active 2018-only names. The 2024 PDFs remain reference-only.
- Cycle 38: README rewritten for the final Gold C static library and CLI. It names `pdf/foc-just-the-rules-gold.pdf` as source of truth, explains retained 2018 and 2024 PDFs as reference-only, documents the library layout, documents `make`, `make test`, and `make clean`, and includes CLI examples.
- Cycle 39: Final consistency pass completed. `git status --short` was reviewed during the cycle, active-file `rg` scans found no stale app workflow references, and this plan was updated to reflect the final completed state.
- Cycle 40: Completion signoff completed. Current cycle is 40 / 40, `make test` passes from `src/`, `README.md` is final for the Gold C-only project shape, retained PDFs are present including `pdf/foc-just-the-rules-gold.pdf`, and no known required cleanup remains.

## Classification Inventory

| Path | Classification | Notes |
|---|---|---|
| `pdf/` | Keep | Contains the Gold source of truth plus retained 2018/2024 reference PDFs. |
| `src/` | Complete Gold C source | Public Gold header, static library implementation, CLI, Makefile, and focused tests are present. |
| `docs/` | Removed | Cycle 12 found only 2024 extraction/prototype and retired playable-app docs; removed entirely in Cycle 13. |
| `scripts/` | Removed | Cycle 10 found only Swift/Xcode/app scripts; removed entirely in Cycle 11. Recreate any future C/PDF helper deliberately later. |
| `AppShell/` | Remove | SwiftUI/Metal app source, outside C-only 2018 scope. |
| `FieldOfChaos.xcodeproj/` | Remove | Xcode project, outside Makefile C-only scope. |
| `UITests/` | Remove | App-level UI tests, outside C-only scope. |
| `packaging/` | Remove | App bundle packaging assets, outside C-only scope. |
| `Package.swift` | Remove | Swift package manifest, outside C-only scope. |
| `README.md` | Final | Documents the Gold source of truth, retained reference PDFs, C library layout, Makefile workflow, and CLI examples. |
| `LICENSE` | Keep | Repository license. |
| `.github/` | Keep | Funding metadata can remain unless later repository policy says otherwise. |
| `.gitignore` | Keep | Should be updated later if new C build artifacts require ignore rules. |
| `tmp/` | Remove | Generated intermediate PDF/text/render artifacts. |
| `.DS_Store` files | Remove | Local filesystem metadata, not source. |

## Script Review Inventory

| Path | Classification | Notes |
|---|---|---|
| `scripts/run_ui_exploration.sh` | Removed | Ran the deleted UI test suite against a packaged macOS app. |
| `scripts/generate_app_icon.swift` | Removed | Swift/AppKit icon generator for deleted app packaging assets. |
| `scripts/package_release.sh` | Removed | Built Swift release app bundle and copied deleted packaging/AppShell assets. |
| `scripts/smoke_playable.sh` | Removed | Ran C tests, then Swift build, Xcode build, app packaging, and packaged resource checks. |

## Documentation Review Inventory

| Path | Classification | Notes |
|---|---|---|
| `docs/architecture.md` | Removed | Retired SwiftUI/Metal/app architecture and 2024 prototype structure. |
| `docs/alice.json` | Removed | Sample character JSON for the old prototype/app workflow, not a 2018 source document. |
| `docs/bob.json` | Removed | Sample character JSON for the old prototype/app workflow, not a 2018 source document. |
| `docs/campaign-system.md` | Removed | Campaign app design from cycles 71-80. |
| `docs/cycles-01-10-execution.md` | Removed | Historical playable-app cycle log with Swift package workflow. |
| `docs/cycles-11-20-execution.md` | Removed | Historical playable-app cycle log with SwiftUI app work. |
| `docs/cycles-21-30-execution.md` | Removed | Historical playable-app cycle log with Metal/UI work. |
| `docs/cycles-31-40-execution.md` | Removed | Historical playable-app cycle log with scenario/AI/campaign work. |
| `docs/cycles-41-50-execution.md` | Removed | Historical playable-app cycle log with tutorial/rules overlay/campaign work. |
| `docs/cycles-51-60-execution.md` | Removed | Historical playable-app cycle log with release candidate packaging work. |
| `docs/cycles-61-80-execution.md` | Removed | Historical playable-app cycle log with campaign/autosave/Swift/Xcode verification. |
| `docs/cycles-81-100-execution.md` | Removed | Historical playable-app cycle log with Xcode/package/icon/smoke verification. |
| `docs/field-of-chaos-rules-extracted.md` | Removed | Extracted from `pdf/field-of-chaos-rpg-070524.pdf`, not the 2018 source of truth. |
| `docs/field-of-chaos-rules-natural-english.md` | Removed | Natural-language rewrite of the 2024 extraction. |
| `docs/field-of-chaos-rules-source.txt` | Removed | Extracted text from 2024 pages, not the 2018 source of truth. |
| `docs/metal-qa.md` | Removed | Metal board QA for the retired app workflow. |
| `docs/playable-game-60-cycle-plan.md` | Removed | Retired SwiftUI/Metal playable game plan. |
| `docs/playable-loop-scope.md` | Removed | Retired playable app scope. |
| `docs/playable-mac-acceptance-checklist.md` | Removed | Retired playable Mac acceptance checklist. |
| `docs/release-candidate-qa.md` | Removed | Retired release candidate app QA checklist. |
| `docs/rules-gap-backlog.md` | Removed | Backlog compares old C prototype to 2024 extracted rules. |
| `docs/ui-test-suite.md` | Removed | Retired UI test suite documentation. |

## Source Freeze Inventory (Pre-Rewrite)

| Path | Classification | Notes |
|---|---|---|
| `src/Makefile` | Replaced | Old Makefile was removed with the old source; final Gold Makefile was created in Cycle 34. |
| `src/fieldofchaos.c` | Replace | Old JSON create/duel CLI tied to the prototype API and `/tmp` examples. |
| `src/character_create.c` | Replace | Duplicate old character JSON creator CLI. |
| `src/engine/include/fieldofchaos_engine.h` | Replace | Old public API uses `foc_` names and exposes 2024/playable-app concepts: snapshots, event buffers, board helpers, scenario, AI, campaign, JSON, and Swift-facing types. |
| `src/engine/fieldofchaos_engine.c` | Replace | Monolithic old implementation mixes character generation, combat, JSON parsing, event logs, board actions, healing, grenades, scenario generation, AI, and campaign advancement. |
| `src/engine/include/module.modulemap` | Remove | Swift module map from the retired app bridge. |
| `src/tests/test_engine.c` | Replace | Tests lock old prototype features including JSON schema, event buffers, board helpers, scenario, AI, campaign, healing, and grenades. |
| `src/fieldofchaos` | Remove | Compiled executable build artifact. |
| `src/character_create` | Remove | Compiled executable build artifact. |
| `src/tests/test_engine` | Remove | Compiled test executable build artifact. |
| `src/engine/fieldofchaos_engine.o` | Remove | Compiled object build artifact. |
| `src/.DS_Store` | Remove | Local filesystem metadata. |

## Replacement Status

- Character generation and validation were rebuilt from Gold with `focgold_generate_stats`, initial 30-point cap validation, and long-term 40-point cap validation.
- Wound mechanics were rebuilt in Cycles 27-29 from Gold: explicit 2018 inherited head/chest/abdomen/arms/legs wound pools, wound effects, and 3d6 hit locations with no body-only torso pool, grave PH roll, or winded/chest-body rule.
- Weapon/range mechanics were rebuilt in Cycles 22-24 from Gold: inherited 2018 firearm ranges plus agreed Shotgun and Grenade rules.
- Healing mechanics were rebuilt in Cycle 31 from Gold: no-skill, bandage/equipment, First Aid, and Paramedic methods with no Pharmaceutical Chemistry healing improvement.
- Retired playable systems remain omitted unless represented by the Gold source of truth; no scenario, campaign, JSON event log, or board UI workflow is active.

## Current Source Layout

| Path | Purpose |
|---|---|
| `src/include/fieldofchaosgold.h` | Public API header for the Gold static library. |
| `src/include/` | Public headers for the Gold static library. |
| `src/lib/fieldofchaosgold.c` | Gold static library implementation. |
| `src/lib/fieldofchaosgold_internal.h` | Internal helpers for multi-die rolling. |
| `src/lib/` | C implementation files for the Gold rules library. |
| `src/cli/fieldofchaosgold.c` | Gold CLI rule tables and deterministic examples. |
| `src/Makefile` | Builds `build/libfieldofchaosgold.a`, `build/fieldofchaosgold`, and all focused tests. |
| `src/tests/test_dice.c` | Focused dice/seeding tests. |
| `src/tests/test_stats.c` | Focused stats and character-generation tests. |
| `src/tests/test_skills.c` | Focused skill metadata and hierarchy tests. |
| `src/tests/test_weapons.c` | Focused Gold weapon, range, Shotgun, Grenade, fire-rate, jam, and reliability tests. |
| `src/tests/test_gold_actions.c` | Focused Gold ranged modifier, called head-shot, movement, and grenade action-economy tests. |
| `src/tests/test_wounds.c` | Focused Gold wound model and wound effect tests. |
| `src/tests/test_hit_locations.c` | Focused Gold bullet/blast hit-location tests. |
| `src/tests/test_melee_healing.c` | Focused Gold melee and healing tests. |
| `src/tests/test_foes.c` | Focused Gold combat foe rule-data tests. |
| `src/tests/test_regressions.c` | Focused regression tests against unwanted 2024 behavior. |
| `src/tests/` | C test source for the Gold rules library and CLI. |
