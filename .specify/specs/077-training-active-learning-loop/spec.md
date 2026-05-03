# Feature Specification: Active Learning Loop with Human-in-the-Loop

**Feature Branch**: `077-training-active-learning-loop`
**Created**: 2026-05-01
**Status**: Stub (scaffolded by 067 cross-domain follow-up planning)
**Wave**: fourth
**Domain**: training / api / react
**Depends on**: #050, #064

## Success Criteria

- Feature is implemented according to the In Scope section
- All tasks in tasks.md are completed and verified
- Code changes are tested and pass CI/CD gates
- Documentation is updated to reflect new behavior


## Problem Statement

The neuro REM pass surfaces high-surprise items but there is no UI to label them. Operators must inspect JSON files.

## In Scope *(mandatory)*

- Add an API surface for queued label requests + label submission.
- Add an `/operator/label-queue` UI route (depends on feature 064).
- Feed accepted labels back into `data/<model>/corpus.json` via signed evidence PRs.
- Track inter-rater agreement when more than one operator participates.

## Out of Scope *(mandatory)*

- Crowd-sourcing or external annotator integration.

## Notes for the planner

- Run `speckit.specify` against this stub to expand User Scenarios, Functional Requirements, and Success Criteria before `speckit.plan`.
- Cross-layer dependencies (API or native changes) MUST be enumerated in the plan artifact and parity-governed (feature 047) when applicable.
- This stub was generated as part of the cross-domain planning batch documented in [`.specify/specs/067-cross-domain-followup-planning/spec.md`](../067-cross-domain-followup-planning/spec.md).

## Status: Implemented
