# Feature Specification: End-to-End Test Harness (Playwright across React/Electron)

**Feature Branch**: `060-frontend-e2e-playwright-harness`
**Created**: 2026-05-01
**Status**: Stub (scaffolded by feature 051 frontend-robustness spike)
**Wave**: third
**Domain**: react / electron / qa
**Depends on**: #052

## Problem Statement

There is no e2e lane today; regressions in the routing, streaming, or auth flows would only be caught in production.

## In Scope *(mandatory)*

- Stand up Playwright with web + Electron driver.
- Author smoke flows: open verdict route, submit ensemble, observe streamed chat, escalate, drain offline queue.
- Wire @axe-core/playwright into the same lane (handoff with 057).
- Run e2e on PRs that touch React, Electron, or shared/ui paths via path-scoped GitHub Actions trigger.

## Out of Scope *(mandatory)*

- Mobile e2e (covered by 066).
- Visual regression diffing (handled by 056 follow-on).

## Notes for the planner

- Run `speckit.specify` against this stub to expand User Scenarios, Functional Requirements, and Success Criteria before `speckit.plan`.
- Cross-layer dependencies (API or native changes) MUST be enumerated in the plan artifact and parity-governed (feature 047) when applicable.
- This stub was generated as part of the 15-feature follow-up inventory in [`.specify/specs/051-frontend-robustness-spike/spec.md`](../051-frontend-robustness-spike/spec.md).
