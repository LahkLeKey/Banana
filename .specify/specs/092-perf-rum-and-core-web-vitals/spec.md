# Feature Specification: Real-User Monitoring + Core Web Vitals Budget

**Feature Branch**: `092-perf-rum-and-core-web-vitals`
**Created**: 2026-05-01
**Status**: Stub (scaffolded by 087 additional planning batch)
**Wave**: third
**Domain**: frontend / observability
**Depends on**: #061

## Success Criteria

- Feature is implemented according to the In Scope section
- All tasks in tasks.md are completed and verified
- Code changes are tested and pass CI/CD gates
- Documentation is updated to reflect new behavior


## Problem Statement

Frontend perf is measured ad-hoc on developer machines. There is no production CWV budget or RUM data feeding back to the team.

## In Scope *(mandatory)*

- Add web-vitals + RUM beacon (Sentry RUM or self-hosted).
- Per-route LCP/INP/CLS budgets; CI lane fails on >10% lab regression.
- Operator dashboard (064) surfaces p75 vitals over time.
- Document the perf-budget contract for new routes.

## Out of Scope *(mandatory)*

- Synthetic monitoring (handed to feature 093).

## Notes for the planner

- Run `speckit.specify` against this stub to expand User Scenarios, Functional Requirements, and Success Criteria before `speckit.plan`.
- Cross-layer dependencies (API or native changes) MUST be enumerated in the plan artifact and parity-governed (feature 047) when applicable.
- This stub was generated as part of the additional cross-domain planning batch documented in [`.specify/specs/087-additional-followup-planning/spec.md`](../087-additional-followup-planning/spec.md).

## Status: Implemented
