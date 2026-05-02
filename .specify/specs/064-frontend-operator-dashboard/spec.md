# Feature Specification: Operator Dashboard (Metrics + Training Runs)

**Feature Branch**: `064-frontend-operator-dashboard`
**Created**: 2026-05-01
**Status**: Stub (scaffolded by feature 051 frontend-robustness spike)
**Wave**: fifth
**Domain**: react / api
**Depends on**: #063, #061

## Problem Statement

Backend ships a deep training + governance surface (features 047, 049, 050) that has no human window. Operators read JSON in artifact directories.

## In Scope *(mandatory)*

- Add an `/operator` route group gated by the `operator` role (depends on 063).
- Surface training-run history, neuro-cycle status, parity-gate results, workflow-recovery counters.
- Use TanStack Query against new read-only endpoints in the Fastify+Prisma TS API.
- Stream live run progress when feature 054 is available (deferred enhancement).

## Out of Scope *(mandatory)*

- Mutating training configuration from the UI (read-only in v1).
- Per-customer dashboards.

## Notes for the planner

- Run `speckit.specify` against this stub to expand User Scenarios, Functional Requirements, and Success Criteria before `speckit.plan`.
- Cross-layer dependencies (API or native changes) MUST be enumerated in the plan artifact and parity-governed (feature 047) when applicable.
- This stub was generated as part of the 15-feature follow-up inventory in [`.specify/specs/051-frontend-robustness-spike/spec.md`](../051-frontend-robustness-spike/spec.md).
