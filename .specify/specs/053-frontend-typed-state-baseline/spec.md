# Feature Specification: Typed Client State Baseline (TanStack Query + Zustand)

**Feature Branch**: `053-typed-state-baseline`
**Created**: 2026-05-01
**Status**: Implemented (v1 — provider + first hook + UI store)
**Wave**: first
**Domain**: react
**Depends on**: none (pure-frontend; ships from current main)

## Problem Statement

All async state in `src/typescript/react` is local React + ad-hoc `fetch`. There is no cache, no background refresh, no optimistic UI, and no shared client store. Every new screen reinvents these patterns and the resilience-bootstrap queue has to be reasoned about by hand at every call site.

## In Scope *(mandatory)*

- Adopt **TanStack Query v5** as the server-state surface for the React app.
- Adopt **Zustand** as the client UI-state surface (theme override, panel toggles, future settings).
- Ship a single `QueryProvider` mounted in `main.tsx` that future hooks consume.
- Ship one migration example (`useBananaSummary`) so subsequent slices have a pattern.
- Tests cover the provider defaults and the store contract (initial state, setters, reset).

## Out of Scope *(mandatory)*

- Migrating the entire `App.tsx` fetch surface in one PR (subsequent slices migrate `predictRipeness`, `fetchEnsembleVerdictWithEmbedding`, `createChatSession`, `sendChatMessage`).
- Replacing the existing `resilience-bootstrap` queue (it remains the durable submit layer).
- Introducing Redux or RTK.
- Persisting the Zustand store to localStorage (deferred to feature 059 theming + 063 auth).

## Functional Requirements

- **FR-001**: A `QueryProvider` MUST wrap `<App />` inside `<ErrorBoundary>` in `main.tsx`.
- **FR-002**: `createBananaQueryClient()` MUST default to `staleTime=60_000`, `gcTime=300_000`, `retry=2`, `refetchOnWindowFocus=false`, `mutations.retry=0`.
- **FR-003**: `useUiStore` MUST expose `themeOverride` (`system|light|dark`), `escalationPanelOpen`, their setters, and a `reset()`.
- **FR-004**: `useBananaSummary(baseUrl, enabled?)` MUST wrap `fetchBananaSummary` and only enable the query when `baseUrl` is truthy.
- **FR-005**: All existing React tests MUST continue to pass (`bun test` in `src/typescript/react`).

## Success Criteria

- **SC-001**: `bun test` exit 0 in `src/typescript/react` (55+ tests).
- **SC-002**: New deps (`@tanstack/react-query`, `zustand`) install cleanly and the dedupe-react postinstall script keeps a single `react`/`react-dom` copy.
- **SC-003**: The `useBananaSummary` hook is consumable from any descendant of `<App />` without additional setup.

## Validation evidence

- `bun test`: 55 pass, 0 fail (147 expect calls).
- New tests:
  - `src/lib/queryClient.test.ts` (1 test) — provider defaults.
  - `src/lib/uiStore.test.ts` (4 tests) — store contract.

## Follow-up handoff

Subsequent migrations should land as their own PRs and add one test file per migrated endpoint. Feature 058 (i18n) will plug into `useUiStore` for locale persistence; feature 059 (theming) will read `themeOverride`; feature 063 (auth) will add a `useAuthStore` peer.
