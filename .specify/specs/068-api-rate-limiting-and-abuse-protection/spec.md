# Feature Specification: API Rate Limiting & Abuse Protection

**Feature Branch**: `068-api-rate-limiting-and-abuse-protection`
**Created**: 2026-05-01
**Status**: Stub (scaffolded by 067 cross-domain follow-up planning)
**Wave**: second
**Domain**: api
**Depends on**: #063

## Problem Statement

Neither API surface has rate limiting. A noisy client or a runaway training-trigger can saturate the pipeline. Once auth ships (feature 063) per-principal limits become the natural primitive.

## In Scope *(mandatory)*

- Add token-bucket rate limiting to Fastify+Prisma TS API and ASP.NET pipeline.
- Tier limits by route class (read / write / training-trigger / admin).
- Surface 429 + Retry-After uniformly; consumers respect headers.
- Mirror limits across both surfaces (parity governance).

## Out of Scope *(mandatory)*

- Bot-detection / fingerprinting heuristics.
- DDoS protection at the edge (out of repo scope).

## Notes for the planner

- Run `speckit.specify` against this stub to expand User Scenarios, Functional Requirements, and Success Criteria before `speckit.plan`.
- Cross-layer dependencies (API or native changes) MUST be enumerated in the plan artifact and parity-governed (feature 047) when applicable.
- This stub was generated as part of the cross-domain planning batch documented in [`.specify/specs/067-cross-domain-followup-planning/spec.md`](../067-cross-domain-followup-planning/spec.md).
