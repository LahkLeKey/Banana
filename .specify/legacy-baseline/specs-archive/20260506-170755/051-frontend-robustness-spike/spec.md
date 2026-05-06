# Feature Specification: Frontend Robustness Uplift Spike

**Feature Branch**: `051-frontend-robustness-spike`
**Created**: 2026-05-01
**Status**: Spike — research only
**Input**: User description: "do a spike on how we could deliver a more robust frontend experience, our frontend is pretty limited given the infrastructure of the project. then scaffold follow up specs for implementation and QA. Plan at least 15 specs that we could follow up on."
## Problem Statement

Feature Specification: Frontend Robustness Uplift Spike aims to improve system capability and user experience by implementing the feature described in the specification.


## In Scope *(mandatory)*

- A research spike (no code shipped) that surveys how Banana's React, Electron, React-Native, and shared-UI surfaces can be uplifted to match the depth of the API/native/training infrastructure.
- An honest inventory of the current frontend surface (one-page App.tsx, 5 shared components, no router, no state-machine, minimal a11y/i18n/observability).
- A prioritized list of **15 follow-up Spec Kit features** (#052–#066) covering routing, state management, streaming, PWA/offline, storybook, accessibility, i18n, theming, e2e, observability, performance budgets, auth/roles, operator dashboard, neuro-trace viewer, and mobile parity.
- Scaffold a `spec.md` stub for each follow-up feature so the planner agent can `speckit.specify` / `speckit.plan` them in dependency order.

## Out of Scope *(mandatory)*

- Implementing any of the 15 follow-up features in this PR (they each get their own branch + PR).
- Changing the API contract, native pipeline, or training cycle (the spike is frontend-only).
- Replacing React, Electron, or React-Native runtimes — uplift is additive.
- Adopting any framework that conflicts with the Bun + Vite + Tailwind + shadcn baseline already shipped in feature 048.

## Background

The Banana frontend currently consists of:

- One React app (`src/typescript/react`) with a single `App.tsx` page rendering ensemble verdict + chat + ripeness in a long form-style layout. shadcn primitives (Alert, Button, Card, Input, Textarea) are wired in.
- A shared `@banana/ui` package with 5 components (BananaBadge, ChatMessageBubble, ErrorText, EscalationPanel, RetryButton, RipenessLabel) plus tokens.
- An Electron shell (`src/typescript/electron`) with main/preload/ipc/notifications/storage/tray and a smoke harness — no rendered desktop polish.
- A React-Native app (`src/typescript/react-native`) with three screens (Capture, History, Verdict) and a thin api/copy/resilience-bootstrap lib.
- Resilience primitives (retry policy, queue) used only in App.tsx.

The backend infrastructure is dramatically deeper than the UX surface: ASP.NET pipeline + native interop, Fastify+Prisma TS API, three trainers with neuro-self-training cycle, validation governance, parity gates, compose-orchestrated runtimes, AI-contract guards. The frontend is where the gap is most visible.

## Spike findings — 9 uplift themes

See [`spike-report.md`](./spike-report.md) for the full survey. The spike identifies nine themes that map cleanly to the 15 follow-up features:

| Theme | Follow-up features |
|---|---|
| Application architecture | 052 routing, 053 state |
| Real-time UX | 054 streaming, 065 neuro-trace viewer |
| Resilience & offline | 055 PWA, 060 e2e |
| Design system depth | 056 storybook, 059 theming |
| Quality gates | 057 a11y, 061 observability, 062 perf budget |
| Internationalization | 058 i18n |
| Authn/Authz UX | 063 auth, 064 operator dashboard |
| Mobile parity | 066 mobile parity |

## Follow-up spec inventory (15 features)

| # | Feature | Domain | Sequence |
|---|---|---|---|
| 052 | Frontend routing + multi-page architecture | react / electron | first wave |
| 053 | Typed state baseline (TanStack Query + Zustand) | react | first wave |
| 054 | Streaming verdict + chat (SSE/WebSocket) | react / api | second wave |
| 055 | PWA + offline service worker shell | react | second wave |
| 056 | Storybook + component documentation | shared/ui | second wave |
| 057 | Accessibility WCAG 2.2 AA audit + CI gate | shared/ui / react | third wave |
| 058 | i18n + localization scaffold | shared/ui / react | third wave |
| 059 | Dark mode + theming via design tokens | shared/ui | third wave |
| 060 | E2E test harness (Playwright across react/electron) | react / electron / qa | third wave |
| 061 | Frontend observability (telemetry, web vitals, error reporting) | react / electron / api | fourth wave |
| 062 | Performance budget + bundle analysis CI gate | workflow | fourth wave |
| 063 | Auth + session/role-aware UI | react / api | fourth wave |
| 064 | Operator dashboard (metrics + training runs) | react / api | fifth wave |
| 065 | Real-time training-run visualization (neuro-trace viewer) | react / api | fifth wave |
| 066 | Mobile parity sprint (RN screen depth + share/upload) | react-native | fifth wave |

## Functional Requirements *(mandatory)*

- **FR-001**: Spike report MUST document the current frontend surface inventory with file references.
- **FR-002**: Spike report MUST cite at least one external authoritative source per uplift theme.
- **FR-003**: Spike report MUST identify which themes have hard prerequisites on the API/native layers and which can ship purely from the frontend domain.
- **FR-004**: Each of the 15 follow-up features MUST have a stub `spec.md` under `.specify/specs/<NNN>-<name>/spec.md` containing at minimum a title, branch name, In Scope, Out of Scope, and a one-paragraph problem statement.
- **FR-005**: Follow-up stubs MUST declare the dependency wave they belong to (first/second/third/fourth/fifth) so they can be sequenced.
- **FR-006**: This feature MUST NOT mutate any runtime code, workflows, or trainer outputs (spike-only contract).

## Success Criteria *(mandatory)*

- **SC-001**: PR containing this spike + 15 stubs lands on `main` without modifying any file outside `.specify/specs/051-066-*` or `docs/release-notes/051-frontend-robustness.md`.
- **SC-002**: `validate-ai-contracts.py` exit 0 after the new files land.
- **SC-003**: Each follow-up stub is discoverable by `speckit.specify --feature <NNN>` (i.e., directory + spec.md exists, branch slug is consistent).
- **SC-004**: At least 5 of the 15 follow-ups are tagged "first/second wave" so a sprint can start immediately without additional sequencing work.

## Risks

| Risk | Mitigation |
|---|---|
| Stubs become abandoned shelfware | Dependency-wave tagging + explicit ownership in plan artifact when each is picked up |
| Frontend uplift drifts from API contract | Each feature that changes the API surface (054, 061, 063, 064, 065) declares it in the stub's "depends on" line |
| shadcn baseline regressions during expansion | All UI work continues to build on feature 048 baseline; component additions go through 056 storybook gate once shipped |
