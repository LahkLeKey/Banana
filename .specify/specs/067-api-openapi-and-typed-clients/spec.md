# Feature Specification: API OpenAPI Generation + Typed Clients

**Feature Branch**: `067-api-openapi-and-typed-clients`
**Created**: 2026-05-01
**Status**: Stub (scaffolded by 067 cross-domain follow-up planning)
**Wave**: first
**Domain**: api / react / electron
**Depends on**: none

## Problem Statement

Both API surfaces (Fastify+Prisma TS and ASP.NET) hand-roll request/response types, and clients duplicate them. There is no machine-checkable contract beyond the parity governance (feature 047) which compares semantics, not schemas.

## In Scope *(mandatory)*

- Emit OpenAPI 3.1 spec from both API surfaces during their build.
- Generate typed TS clients for React, Electron, and React-Native consumers.
- Add a CI lane that diffs the two emitted specs (parity check at the schema level).
- Wire generated types into the TanStack Query hooks introduced by feature 053.

## Out of Scope *(mandatory)*

- Replacing the parity-governance lane (it stays, schema diff is additive).
- GraphQL or gRPC migration.

## Notes for the planner

- Run `speckit.specify` against this stub to expand User Scenarios, Functional Requirements, and Success Criteria before `speckit.plan`.
- Cross-layer dependencies (API or native changes) MUST be enumerated in the plan artifact and parity-governed (feature 047) when applicable.
- This stub was generated as part of the cross-domain planning batch documented in [`.specify/specs/067-cross-domain-followup-planning/spec.md`](../067-cross-domain-followup-planning/spec.md).
