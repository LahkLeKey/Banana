# Feature Specification: Postgres Migration Discipline (Prisma + EF Parity)

**Feature Branch**: `095-data-postgres-migration-discipline`
**Created**: 2026-05-01
**Status**: Stub (scaffolded by 087 additional planning batch)
**Wave**: second
**Domain**: api / data
**Depends on**: none

## Problem Statement

Prisma migrations and EF Core migrations evolve independently against the same database. There is no canonical schema source and no backward-compat policy.

## In Scope *(mandatory)*

- Pick Prisma as the canonical schema authority; EF generates code-first models from the same DB.
- Add a CI lane that diffs the live schema against both ORMs after migration.
- Backward-compatibility policy: every migration must be reversible or paired with a code-only follow-up.
- Document the migration-review checklist in the wiki.

## Out of Scope *(mandatory)*

- Replacing EF Core (the parity continues; only schema authority is unified).

## Notes for the planner

- Run `speckit.specify` against this stub to expand User Scenarios, Functional Requirements, and Success Criteria before `speckit.plan`.
- Cross-layer dependencies (API or native changes) MUST be enumerated in the plan artifact and parity-governed (feature 047) when applicable.
- This stub was generated as part of the additional cross-domain planning batch documented in [`.specify/specs/087-additional-followup-planning/spec.md`](../087-additional-followup-planning/spec.md).
