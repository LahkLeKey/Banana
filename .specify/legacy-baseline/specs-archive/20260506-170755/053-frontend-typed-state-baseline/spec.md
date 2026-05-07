# Feature Specification: Typed Client State Baseline (TanStack Query + Zustand)

**Feature Branch**: `053-frontend-typed-state-baseline`
**Created**: 2026-05-01
**Status**: Stub (scaffolded by feature 051 frontend-robustness spike)
**Wave**: first
**Domain**: react
**Depends on**: none (pure-frontend; can ship from current main)

## Success Criteria

- Feature is implemented according to the In Scope section
- All tasks in tasks.md are completed and verified
- Code changes are tested and pass CI/CD gates
- Documentation is updated to reflect new behavior


## Problem Statement

All async state is local React + ad-hoc fetch. There is no cache, no background refresh, no optimistic UI, and no shared client store. Every new feature reinvents these patterns.

## In Scope *(mandatory)*

- Adopt TanStack Query v5 for all server state (verdict, chat, ripeness, history, future operator endpoints).
- Adopt Zustand for client UI state (panels open, selected history entry, theme override).
- Generate strict TS types from the Fastify+Prisma OpenAPI surface and feed them into the query hooks.
- Migrate App.tsx fetch calls to query hooks while preserving resilience-bootstrap retry semantics.

## Out of Scope *(mandatory)*

- Replacing the existing resilience-bootstrap queue.
- Introducing Redux or RTK.
- Server-state for non-Banana endpoints.

## Notes for the planner

- Run `speckit.specify` against this stub to expand User Scenarios, Functional Requirements, and Success Criteria before `speckit.plan`.
- Cross-layer dependencies (API or native changes) MUST be enumerated in the plan artifact and parity-governed (feature 047) when applicable.
- This stub was generated as part of the 15-feature follow-up inventory in [`.specify/specs/051-frontend-robustness-spike/spec.md`](../051-frontend-robustness-spike/spec.md).
