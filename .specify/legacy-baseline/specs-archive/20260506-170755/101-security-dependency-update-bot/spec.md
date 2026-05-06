# Feature Specification: Dependency Update Bot (Renovate)

**Feature Branch**: `101-security-dependency-update-bot`
**Created**: 2026-05-01
**Status**: Stub (scaffolded by 087 additional planning batch)
**Wave**: first
**Domain**: security / devex
**Depends on**: none

## Success Criteria

- Feature is implemented according to the In Scope section
- All tasks in tasks.md are completed and verified
- Code changes are tested and pass CI/CD gates
- Documentation is updated to reflect new behavior


## Problem Statement

Dependabot covers npm but not Bun lockfiles, NuGet metadata, or CMake `FetchContent`. Patch-level CVEs sit until someone notices.

## In Scope *(mandatory)*

- Adopt Renovate with a config covering Bun, npm, NuGet, CMake, GitHub Actions, and Dockerfile bases.
- Group patch updates; auto-merge on green CI for non-major bumps.
- Major bumps create a draft PR with release-notes preview.
- Document the security-disclosure-window policy.

## Out of Scope *(mandatory)*

- Replacing Dependabot for surfaces it already covers (Renovate is canonical going forward).

## Notes for the planner

- Run `speckit.specify` against this stub to expand User Scenarios, Functional Requirements, and Success Criteria before `speckit.plan`.
- Cross-layer dependencies (API or native changes) MUST be enumerated in the plan artifact and parity-governed (feature 047) when applicable.
- This stub was generated as part of the additional cross-domain planning batch documented in [`.specify/specs/087-additional-followup-planning/spec.md`](../087-additional-followup-planning/spec.md).
