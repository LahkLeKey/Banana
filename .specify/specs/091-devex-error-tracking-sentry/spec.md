# Feature Specification: Error Tracking (Sentry / OpenTelemetry Errors)

**Feature Branch**: `091-devex-error-tracking-sentry`
**Created**: 2026-05-01
**Status**: Stub (scaffolded by 087 additional planning batch)
**Wave**: third
**Domain**: devex / observability
**Depends on**: #061

## Problem Statement

Runtime errors in React, Electron, both APIs, and the React-Native client surface only as logs. There is no aggregated, deduplicated error inbox.

## In Scope *(mandatory)*

- Wire Sentry SDK (or OTel logs + Grafana errors) into all five runtimes.
- Source-map upload from React + Electron CI builds.
- Release tagging tied to git SHA + model registry id (feature 079).
- Operator dashboard (064) links to error inbox; alerts wired to PagerDuty/Slack.

## Out of Scope *(mandatory)*

- Performance monitoring (handed to feature 092).

## Notes for the planner

- Run `speckit.specify` against this stub to expand User Scenarios, Functional Requirements, and Success Criteria before `speckit.plan`.
- Cross-layer dependencies (API or native changes) MUST be enumerated in the plan artifact and parity-governed (feature 047) when applicable.
- This stub was generated as part of the additional cross-domain planning batch documented in [`.specify/specs/087-additional-followup-planning/spec.md`](../087-additional-followup-planning/spec.md).
