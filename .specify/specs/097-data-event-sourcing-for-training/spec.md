# Feature Specification: Event Sourcing for Training Lifecycle

**Feature Branch**: `097-data-event-sourcing-for-training`
**Created**: 2026-05-01
**Status**: Stub (scaffolded by 087 additional planning batch)
**Wave**: fifth
**Domain**: data / training
**Depends on**: #070, #079

## Problem Statement

The training lifecycle (corpus add -> trigger -> evidence PR -> promotion) is reconstructable only by reading scattered logs. An event-sourced view enables replay + debugging + compliance.

## In Scope *(mandatory)*

- Define the canonical event schema covering corpus, training, evidence, and promotion.
- Persist events in an append-only Postgres table (Outbox pattern).
- Project read models for the operator dashboard + audit log.
- Replay tooling: rebuild any model release from the event log alone.

## Out of Scope *(mandatory)*

- Event-sourcing the verdict pipeline itself.

## Notes for the planner

- Run `speckit.specify` against this stub to expand User Scenarios, Functional Requirements, and Success Criteria before `speckit.plan`.
- Cross-layer dependencies (API or native changes) MUST be enumerated in the plan artifact and parity-governed (feature 047) when applicable.
- This stub was generated as part of the additional cross-domain planning batch documented in [`.specify/specs/087-additional-followup-planning/spec.md`](../087-additional-followup-planning/spec.md).

## Status: Implemented
