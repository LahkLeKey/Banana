# Feature Specification: Streaming Verdict & Chat (SSE/WebSocket)

**Feature Branch**: `054-frontend-streaming-verdict-and-chat`
**Created**: 2026-05-01
**Status**: Stub (scaffolded by feature 051 frontend-robustness spike)
**Wave**: second
**Domain**: react / api
**Depends on**: #053

## Success Criteria

- Feature is implemented according to the In Scope section
- All tasks in tasks.md are completed and verified
- Code changes are tested and pass CI/CD gates
- Documentation is updated to reflect new behavior


## Problem Statement

Verdict and chat both block on a full HTTP roundtrip; users see a long spinner. The neuro layer (feature 050) emits trace events that have nowhere to surface.

## In Scope *(mandatory)*

- Add SSE endpoints in Fastify+Prisma TS API for chat tokens and ensemble verdict progress.
- Mirror the streaming contract in the ASP.NET pipeline so both API surfaces stay parity-aligned (feature 047 governance).
- Consume streams in React via TanStack Query streamed-query pattern.
- Define backpressure + reconnect policy using existing resilience primitives.

## Out of Scope *(mandatory)*

- Bidirectional WebSocket for chat (SSE one-way is sufficient for v1).
- Streaming for non-customer endpoints (admin/operator).

## Notes for the planner

- Run `speckit.specify` against this stub to expand User Scenarios, Functional Requirements, and Success Criteria before `speckit.plan`.
- Cross-layer dependencies (API or native changes) MUST be enumerated in the plan artifact and parity-governed (feature 047) when applicable.
- This stub was generated as part of the 15-feature follow-up inventory in [`.specify/specs/051-frontend-robustness-spike/spec.md`](../051-frontend-robustness-spike/spec.md).
