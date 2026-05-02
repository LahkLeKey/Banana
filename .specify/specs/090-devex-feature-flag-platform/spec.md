# Feature Specification: Feature Flag Platform (OpenFeature)

**Feature Branch**: `090-devex-feature-flag-platform`
**Created**: 2026-05-01
**Status**: Stub (scaffolded by 087 additional planning batch)
**Wave**: third
**Domain**: devex / api / react
**Depends on**: none

## Problem Statement

There is no feature-flag primitive. Risky changes either gate behind env vars or merge as big-bang releases.

## In Scope *(mandatory)*

- Adopt OpenFeature SDK across React, Electron, TS API, and ASP.NET API.
- Provider: start with a Postgres-backed flag store; allow swap to LaunchDarkly / Flagsmith later.
- Operator dashboard (feature 064) surfaces flag state + per-flag rollout %.
- Document evaluation contract: server-side decisions only for security-sensitive flags.

## Out of Scope *(mandatory)*

- A/B experimentation (handed to feature 080).

## Notes for the planner

- Run `speckit.specify` against this stub to expand User Scenarios, Functional Requirements, and Success Criteria before `speckit.plan`.
- Cross-layer dependencies (API or native changes) MUST be enumerated in the plan artifact and parity-governed (feature 047) when applicable.
- This stub was generated as part of the additional cross-domain planning batch documented in [`.specify/specs/087-additional-followup-planning/spec.md`](../087-additional-followup-planning/spec.md).

## Status: Implemented
