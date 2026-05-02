# Feature Specification: Monorepo Task Graph (Nx / Turbo / Bun workspaces)

**Feature Branch**: `087-devex-monorepo-task-graph`
**Created**: 2026-05-01
**Status**: Stub (scaffolded by 087 additional planning batch)
**Wave**: third
**Domain**: devex / infra
**Depends on**: none

## Problem Statement

Cross-workspace builds re-run unchanged work. There is no incremental task graph spanning Bun workspaces, dotnet projects, and CMake targets.

## In Scope *(mandatory)*

- Adopt a task-graph runner (Turborepo for TS, integrated with dotnet+CMake via wrapper scripts).
- Cache build + test outputs keyed on input hashes; share cache in CI via remote cache backend.
- Document the canonical command surface (`bun run check`, `bun run test`, etc.) in the wiki.
- Measure CI wall-clock improvement; target -40% on no-op PRs.

## Out of Scope *(mandatory)*

- Replacing existing `scripts/*.sh` entry points (the runner wraps them).

## Notes for the planner

- Run `speckit.specify` against this stub to expand User Scenarios, Functional Requirements, and Success Criteria before `speckit.plan`.
- Cross-layer dependencies (API or native changes) MUST be enumerated in the plan artifact and parity-governed (feature 047) when applicable.
- This stub was generated as part of the additional cross-domain planning batch documented in [`.specify/specs/087-additional-followup-planning/spec.md`](../087-additional-followup-planning/spec.md).
