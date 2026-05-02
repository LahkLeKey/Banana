# Feature Specification: Mutation Testing Baseline (PIT / Stryker)

**Feature Branch**: `085-quality-mutation-testing-baseline`
**Created**: 2026-05-01
**Status**: Stub (scaffolded by 067 cross-domain follow-up planning)
**Wave**: fourth
**Domain**: tests
**Depends on**: none

## Problem Statement

Coverage is high (feature 003 rehydration) but coverage doesn't measure assertion strength. A test can execute a line and assert nothing meaningful.

## In Scope *(mandatory)*

- Adopt Stryker for the TS surface and PIT for the .NET surface.
- Run on a path-scoped CI lane (touching `Banana.Api`, `src/typescript/api`, or `tests/`).
- Establish a baseline mutation score per project; fail PRs that regress by >5%.
- Allowlist for known-low-value mutants.

## Out of Scope *(mandatory)*

- Native mutation testing (no good tooling for our C surface).

## Notes for the planner

- Run `speckit.specify` against this stub to expand User Scenarios, Functional Requirements, and Success Criteria before `speckit.plan`.
- Cross-layer dependencies (API or native changes) MUST be enumerated in the plan artifact and parity-governed (feature 047) when applicable.
- This stub was generated as part of the cross-domain planning batch documented in [`.specify/specs/067-cross-domain-followup-planning/spec.md`](../067-cross-domain-followup-planning/spec.md).
