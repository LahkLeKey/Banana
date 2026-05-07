# Feature Specification: Load Testing Baseline (k6 / Locust)

**Feature Branch**: `094-perf-load-testing-baseline`
**Created**: 2026-05-01
**Status**: Stub (scaffolded by 087 additional planning batch)
**Wave**: fourth
**Domain**: tests / infra
**Depends on**: none

## Success Criteria

- Feature is implemented according to the In Scope section
- All tasks in tasks.md are completed and verified
- Code changes are tested and pass CI/CD gates
- Documentation is updated to reflect new behavior


## Problem Statement

There is no documented capacity baseline. We do not know at what RPS the verdict pipeline degrades, nor when the native pipeline becomes the bottleneck.

## In Scope *(mandatory)*

- Author k6 scripts covering the four canonical user journeys.
- Run nightly against an ephemeral compose stack; publish RPS-vs-latency curves.
- Capacity-planning doc updated each release with the latest curves.
- PR lane optionally runs a small load-smoke (1k requests) on perf-sensitive changes.

## Out of Scope *(mandatory)*

- Continuous load against production.

## Notes for the planner

- Run `speckit.specify` against this stub to expand User Scenarios, Functional Requirements, and Success Criteria before `speckit.plan`.
- Cross-layer dependencies (API or native changes) MUST be enumerated in the plan artifact and parity-governed (feature 047) when applicable.
- This stub was generated as part of the additional cross-domain planning batch documented in [`.specify/specs/087-additional-followup-planning/spec.md`](../087-additional-followup-planning/spec.md).

## Status: Implemented
