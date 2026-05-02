# Feature Specification: A/B Harness for Live Model Comparison

**Feature Branch**: `080-training-ab-harness-live-comparison`
**Created**: 2026-05-01
**Status**: Stub (scaffolded by 067 cross-domain follow-up planning)
**Wave**: fifth
**Domain**: api / training
**Depends on**: #079, #061

## Problem Statement

There is no way to ship two trained variants and compare them on real traffic. Promotion decisions are made on holdout accuracy alone.

## In Scope *(mandatory)*

- Add a traffic-splitting layer in both API surfaces (sticky on session id).
- Record per-variant verdicts + outcomes in the audit log.
- Operator dashboard surfaces variant comparison (depends on feature 064).
- Tear-down contract: variants automatically retire after a configurable window unless promoted.

## Out of Scope *(mandatory)*

- Bandit / contextual-bandit allocation in v1 (fixed splits only).

## Notes for the planner

- Run `speckit.specify` against this stub to expand User Scenarios, Functional Requirements, and Success Criteria before `speckit.plan`.
- Cross-layer dependencies (API or native changes) MUST be enumerated in the plan artifact and parity-governed (feature 047) when applicable.
- This stub was generated as part of the cross-domain planning batch documented in [`.specify/specs/067-cross-domain-followup-planning/spec.md`](../067-cross-domain-followup-planning/spec.md).
