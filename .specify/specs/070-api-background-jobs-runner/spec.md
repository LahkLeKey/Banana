# Feature Specification: Background Job Runner (Durable Queue)

**Feature Branch**: `070-api-background-jobs-runner`
**Created**: 2026-05-01
**Status**: Stub (scaffolded by 067 cross-domain follow-up planning)
**Wave**: second
**Domain**: api
**Depends on**: none

## Problem Statement

Long-running training triggers and PR-opener calls run inline on request handlers. Failures lose work, and operators cannot see what's pending.

## In Scope *(mandatory)*

- Adopt a durable job queue (BullMQ on Redis, or PostgreSQL-backed via pg-boss).
- Move triggered-training, evidence-PR opener, and consolidation hand-off into jobs.
- Add a small admin surface listing pending / failed / completed jobs.
- Idempotency keys on all enqueue paths.

## Out of Scope *(mandatory)*

- Replacing the workflow orchestrator (feature 049) — the queue runs inside the API tier.

## Notes for the planner

- Run `speckit.specify` against this stub to expand User Scenarios, Functional Requirements, and Success Criteria before `speckit.plan`.
- Cross-layer dependencies (API or native changes) MUST be enumerated in the plan artifact and parity-governed (feature 047) when applicable.
- This stub was generated as part of the cross-domain planning batch documented in [`.specify/specs/067-cross-domain-followup-planning/spec.md`](../067-cross-domain-followup-planning/spec.md).
