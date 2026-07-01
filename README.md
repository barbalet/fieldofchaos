# fieldofchaos

Programmatic exploration of the Field of Chaos tabletop skirmish RPG, with a focus on producing and reviewing a C-language implementation of the rules.

## Rewrite Status

This repository is being rewritten around `pdf/foc-just-the-rules-2018.pdf` as the source of truth. The Swift, Xcode, and macOS app workflow has been retired.

## Rules Sources

- Canonical source PDF: `pdf/foc-just-the-rules-2018.pdf`
- Reference-only newer PDF: `pdf/foc-just-the-rules-2024.pdf`
- Reference-only full newer PDF: `pdf/field-of-chaos-rpg-070524.pdf`
- Reference-only full 2018 PDF: `pdf/foc_rpg_051018.pdf`

## Project Aim

- Build a C-language program version of the game rules for validation, review, and debugging.
- Keep source material traceable back to the original PDF while maintaining implementation-friendly Markdown documentation.
- Replace the prior playable SwiftUI/Metal/C app direction with a 2018-focused C static library and C program.
- Track the current rewrite in `PLAN.md`.

## Architecture

- Public C API headers: `src/include/`
- Static library implementation: `src/lib/`
- C command-line program source: `src/cli/`
- C tests: `src/tests/`

## Retired App Workflow

The prior Swift package, Xcode project, app packaging, and UI-test workflows are no longer active project workflows. The README will be rewritten near the end of the 40-cycle plan for the final C-only project shape.

## C Source Status

The old prototype source has been removed. The new 2018-focused C header, library, CLI, tests, and Makefile will be recreated by the remaining development cycles.
