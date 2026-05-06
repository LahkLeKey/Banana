# Feature Specification: Native Sanitizer CI Lane (ASAN / UBSAN / MSAN)

**Feature Branch**: `073-native-sanitizer-ci-lane`
**Created**: 2026-05-01
**Status**: Stub (scaffolded by 067 cross-domain follow-up planning)
**Wave**: second
**Domain**: native
**Depends on**: none

## Success Criteria

- Feature is implemented according to the In Scope section
- All tasks in tasks.md are completed and verified
- Code changes are tested and pass CI/CD gates
- Documentation is updated to reflect new behavior


## Problem Statement

Native tests run with default flags. Memory leaks, undefined behavior, and use-after-free are not caught until production telemetry sees them (and there is no production telemetry yet either).

## In Scope *(mandatory)*

- Add a CMake preset that builds with `-fsanitize=address,undefined` (and a separate MSAN build).
- Run the existing `tests/native` suite under each sanitizer in CI.
- Triage failing cases as either fix-now or `LeakSanitizer.suppressions` entries with PR review.
- Publish the suppression file as an artifact with each run.

## Out of Scope *(mandatory)*

- TSAN coverage in v1 (deferred — needs MT model audit first).

## Notes for the planner

- Run `speckit.specify` against this stub to expand User Scenarios, Functional Requirements, and Success Criteria before `speckit.plan`.
- Cross-layer dependencies (API or native changes) MUST be enumerated in the plan artifact and parity-governed (feature 047) when applicable.
- This stub was generated as part of the cross-domain planning batch documented in [`.specify/specs/067-cross-domain-followup-planning/spec.md`](../067-cross-domain-followup-planning/spec.md).
