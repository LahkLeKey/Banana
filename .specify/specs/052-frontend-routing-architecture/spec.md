# Feature Specification: Frontend Routing & Multi-Page Architecture

**Feature Branch**: `052-frontend-routing-architecture`
**Created**: 2026-05-01
**Status**: Stub (scaffolded by feature 051 frontend-robustness spike)
**Wave**: first
**Domain**: react / electron
**Depends on**: none (pure-frontend; can ship from current main)

## Problem Statement

Single-page App.tsx couples chat, ensemble verdict, ripeness, escalation, and history into one component tree. Adding any new screen requires editing the same file, and route-level code-splitting is impossible.

## In Scope *(mandatory)*

- Adopt a typed file-based router (TanStack Router or React Router 7 framework mode) for the React app.
- Split the existing App.tsx surface into 4-5 routes (verdict, chat, history, escalation, settings).
- Code-split each route via Vite dynamic import; preserve current shadcn baseline and resilience wiring.
- Wire Electron BrowserWindow navigation to the new router (history mode + deep-link).

## Out of Scope *(mandatory)*

- Replacing Vite or migrating to Next.js / Remix.
- Server-side rendering.
- Adding new business features beyond surfacing the existing ones.

## Notes for the planner

- Run `speckit.specify` against this stub to expand User Scenarios, Functional Requirements, and Success Criteria before `speckit.plan`.
- Cross-layer dependencies (API or native changes) MUST be enumerated in the plan artifact and parity-governed (feature 047) when applicable.
- This stub was generated as part of the 15-feature follow-up inventory in [`.specify/specs/051-frontend-robustness-spike/spec.md`](../051-frontend-robustness-spike/spec.md).
