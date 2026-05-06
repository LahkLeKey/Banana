# Feature Specification: Pre-commit Hook Baseline (lint + format + secret scan)

**Feature Branch**: `089-devex-pre-commit-hooks-baseline`
**Created**: 2026-05-01
**Status**: Stub (scaffolded by 087 additional planning batch)
**Wave**: first
**Domain**: devex
**Depends on**: none

## Success Criteria

- Feature is implemented according to the In Scope section
- All tasks in tasks.md are completed and verified
- Code changes are tested and pass CI/CD gates
- Documentation is updated to reflect new behavior


## Problem Statement

Lint + format + secret-detection only run in CI; broken commits land on feature branches and waste review time.

## In Scope *(mandatory)*

- Adopt `pre-commit` framework with hooks for prettier, biome, dotnet format, clang-format, ruff, and gitleaks.
- Hooks use the same configs as the CI lanes (single source of truth).
- Document opt-in install (`pre-commit install`) and the bypass policy.
- CI lane verifies the hook config still matches the CI checks (no drift).

## Out of Scope *(mandatory)*

- Forcing hooks for all contributors (opt-in only).

## Notes for the planner

- Run `speckit.specify` against this stub to expand User Scenarios, Functional Requirements, and Success Criteria before `speckit.plan`.
- Cross-layer dependencies (API or native changes) MUST be enumerated in the plan artifact and parity-governed (feature 047) when applicable.
- This stub was generated as part of the additional cross-domain planning batch documented in [`.specify/specs/087-additional-followup-planning/spec.md`](../087-additional-followup-planning/spec.md).
