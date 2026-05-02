# Feature Specification: Threat Model + STRIDE Review

**Feature Branch**: `098-security-threat-model-and-stride`
**Created**: 2026-05-01
**Status**: Stub (scaffolded by 087 additional planning batch)
**Wave**: second
**Domain**: security
**Depends on**: none

## Problem Statement

There is no documented threat model. New features ship without a structured security review against STRIDE categories.

## In Scope *(mandatory)*

- Author the canonical threat model in `.specify/wiki/human-reference/security/threat-model.md`.
- Cover trust boundaries between React/Electron/Mobile, both APIs, native pipeline, and Postgres.
- STRIDE checklist added to the PR template for security-sensitive changes.
- Quarterly review: the model is re-evaluated against shipped changes.

## Out of Scope *(mandatory)*

- Pen-testing engagement (handed to feature 099).

## Notes for the planner

- Run `speckit.specify` against this stub to expand User Scenarios, Functional Requirements, and Success Criteria before `speckit.plan`.
- Cross-layer dependencies (API or native changes) MUST be enumerated in the plan artifact and parity-governed (feature 047) when applicable.
- This stub was generated as part of the additional cross-domain planning batch documented in [`.specify/specs/087-additional-followup-planning/spec.md`](../087-additional-followup-planning/spec.md).
