# Feature Specification: Frontend Observability (Telemetry, Web Vitals, Error Reporting)

**Feature Branch**: `061-frontend-observability-and-vitals`
**Created**: 2026-05-01
**Status**: Stub (scaffolded by feature 051 frontend-robustness spike)
**Wave**: fourth
**Domain**: react / electron / api
**Depends on**: #054

## Problem Statement

No production telemetry; every regression is reported by humans. Web Vitals are unmeasured.

## In Scope *(mandatory)*

- Adopt OpenTelemetry JS browser SDK for traces and metrics.
- Add a thin OTLP collector endpoint on the Fastify+Prisma TS API (or proxy to an external collector).
- Capture Core Web Vitals (LCP, INP, CLS) and ship them to the same collector.
- Wire client-side error reporting (window.onerror + React error boundary) into the trace surface.

## Out of Scope *(mandatory)*

- Server-side tracing changes (already covered by API observability work).
- PII collection.

## Notes for the planner

- Run `speckit.specify` against this stub to expand User Scenarios, Functional Requirements, and Success Criteria before `speckit.plan`.
- Cross-layer dependencies (API or native changes) MUST be enumerated in the plan artifact and parity-governed (feature 047) when applicable.
- This stub was generated as part of the 15-feature follow-up inventory in [`.specify/specs/051-frontend-robustness-spike/spec.md`](../051-frontend-robustness-spike/spec.md).
