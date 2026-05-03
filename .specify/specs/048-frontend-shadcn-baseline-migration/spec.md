# Feature Specification: Frontend shadcn Baseline Migration

**Feature Branch**: `feature/048-frontend-shadcn-baseline-migration`
**Created**: 2026-04-26
**Status**: Draft
**Input**: User description: "we still have a state management bug for a blank page, lets just migrate our frontend to shadcn so we have a good baseline, make a new spec"
## Problem Statement

Feature Specification: Frontend shadcn Baseline Migration aims to improve system capability and user experience by implementing the feature described in the specification.


## In Scope *(mandatory)*

- Replace the current ad-hoc Tailwind primitives in `src/typescript/react` with a shadcn/ui-based baseline (button, input, textarea, card, form, alert, toast, dialog/sheet, scroll-area, separator) wired into the existing App shell.
- Introduce a deterministic, observable client state container for the ensemble + chat flows so submit transitions cannot leave the app in a blank/unrendered state.
- Add a top-level error boundary + suspense fallback that always render visible content (no blank `<div />`) when a render or state-derivation error occurs.
- Preserve the existing `@banana/ui` shared contracts (`ChatMessage`, `EnsembleVerdict`, `Ripeness`, training types) and API base URL contract (`VITE_BANANA_API_BASE_URL`).
- Keep Bun as the package manager and Vite as the dev/build tool; add only the dependencies shadcn requires (Radix primitives, `class-variance-authority`, `clsx`, `tailwind-merge`, `lucide-react`).
- Carry forward the spike 042 guardrails (form `onSubmit` preventDefault, button `type="button"`, location-snapshot classification, Ctrl/Cmd+Enter submit) inside the new shadcn components.
- Provide a regression test suite that proves the ensemble submit and chat send paths never render an empty document body and never lose visible state on error.

## Out of Scope *(mandatory)*

- Migrating the React Native (`src/typescript/react-native`) or Electron (`src/typescript/electron`) surfaces to shadcn — those keep their current shells and only consume `@banana/ui` types.
- Changing the ASP.NET API contract, Fastify chat contract, native interop, or training pipeline.
- Visual redesign beyond what shadcn defaults dictate (no new IA, no new routes, no new analytics).
- Theming/dark-mode work beyond shadcn's default `globals.css` token block.
- Replacing the existing `@banana/ui` package with shadcn primitives; `@banana/ui` remains the shared contract surface, shadcn lives inside the React app.

## User Scenarios & Testing *(mandatory)*

### User Story 1 - Predict ensemble never produces a blank page (Priority: P1)

A user types a banana sample, clicks **Predict ensemble**, and always sees either the prior verdict, a loading state, the new verdict, or an inline error. They never see a blank page, and the URL/route never changes as a side effect of submitting.

**Why this priority**: This is the regression that motivated the migration. Without it the baseline is not "good".

**Independent Test**: Render the migrated `App`, click submit, fail the network request, force a render-time error in the verdict panel, and assert that the document body always contains visible shadcn-rendered content and the location pathname/search/hash is unchanged.

**Acceptance Scenarios**:

1. **Given** a successful ensemble request, **When** the user clicks Predict ensemble, **Then** the verdict card updates in place and the page never unmounts to empty.
2. **Given** a failing ensemble request, **When** the request rejects, **Then** an inline shadcn `Alert` (or toast) is shown and the previous verdict remains visible.
3. **Given** a thrown render error inside the verdict panel, **When** React catches it, **Then** the top-level error boundary renders a shadcn `Card` with the error and a retry button instead of a blank document.

---

### User Story 2 - Shared shadcn primitives across ensemble + chat (Priority: P2)

The ensemble panel and the chat panel share a single set of shadcn primitives (`Button`, `Input`, `Textarea`, `Card`, `ScrollArea`, `Form`) so behavior (focus rings, disabled states, keyboard submit) is consistent and predictable.

**Why this priority**: Predictability across the two main flows is what makes this a "baseline" rather than a one-off fix.

**Independent Test**: DOM tests assert both panels use the same component identities and that disabled/loading states render identical class names from `cn()`.

**Acceptance Scenarios**:

1. **Given** the ensemble form is submitting, **When** the button is in its loading state, **Then** it renders the shadcn disabled styling and shows a spinner.
2. **Given** the chat composer is submitting, **When** the user presses Enter (without Shift), **Then** the same submit guardrails fire and the same disabled styling appears.

---

### User Story 3 - Deterministic client state for submit lifecycles (Priority: P2)

All submit flows route through a single typed reducer (or store) with explicit `idle | submitting | success | error` states, so a partial state can never make the React tree return `null` at the root.

**Why this priority**: The remaining blank-page bug after spike 042 is a state-management regression, not a navigation regression. Centralizing it is the durable fix.

**Independent Test**: Unit-test the reducer for every transition; integration-test that the App renders a non-empty root for every reachable state.

**Acceptance Scenarios**:

1. **Given** any transition triggered by a user action, **When** the reducer applies it, **Then** the resulting state has a defined `view` discriminator and the root component returns non-null JSX.
2. **Given** an unexpected state shape, **When** the root selector runs, **Then** it falls back to the error-boundary card instead of returning `null`/`undefined`.

---

### Edge Cases

- Network goes offline mid-submit: shadcn `Alert` is shown, prior verdict preserved, no blank page.
- Bun/Vite HMR reload during a pending submit: state hydrates back to `idle` with the last successful verdict still visible.
- API returns a 200 with malformed JSON: error boundary catches the parse error and renders the fallback card.
- User double-submits via mouse + keyboard: only one in-flight request is dispatched (idempotency handled by reducer guard).
- Slow network (>3s): loading skeleton remains visible; the page never unmounts to empty.

## Requirements *(mandatory)*

### Functional Requirements

- **FR-001**: The React app MUST install and configure shadcn/ui under `src/typescript/react/src/components/ui/` with Tailwind tokens defined in `src/index.css`.
- **FR-002**: The ensemble form MUST be rendered with shadcn `Form`, `Input`, `Button`, and `Card` components and MUST keep `type="button"`/`onSubmit preventDefault` guardrails from spike 042.
- **FR-003**: The chat composer MUST be rendered with shadcn `Textarea`, `Button`, and `ScrollArea` components and MUST share the submit guardrail helpers with the ensemble form.
- **FR-004**: The application MUST expose a single client state machine (reducer or store) covering submit lifecycle states `idle | submitting | success | error` for both ensemble and chat flows.
- **FR-005**: The root `App` component MUST always return non-null JSX for every reachable state of the state machine (verified by exhaustive switch + `never` check).
- **FR-006**: A top-level `ErrorBoundary` MUST wrap the App and render a shadcn `Card` fallback with retry on any thrown render error.
- **FR-007**: The migration MUST preserve all existing exports of `@banana/ui` and the `VITE_BANANA_API_BASE_URL` contract.
- **FR-008**: All existing passing tests in `src/typescript/react/src/App.test.tsx` MUST continue to pass; new tests MUST cover blank-page non-regression for success, error, and thrown-render paths.
- **FR-009**: The build MUST remain Bun + Vite; `bun install`, `bunx tsc --noEmit`, `bun test`, and `bun run build` MUST all succeed.
- **FR-010**: No machine-specific paths or Node-only assumptions MUST be introduced; the React container build (`docker/react.Dockerfile`) MUST continue to build.

### Key Entities

- **SubmitState**: Discriminated union `{ kind: 'idle' } | { kind: 'submitting', requestId } | { kind: 'success', verdict } | { kind: 'error', message, lastVerdict? }` shared by ensemble and chat reducers.
- **AppView**: Top-level discriminator the root selector maps state to (`'shell' | 'error-fallback'`) so the root never returns null.
- **UI Primitive Set**: The shadcn components installed under `components/ui/` (button, input, textarea, card, form, alert, scroll-area, separator, dialog, toast).

## Success Criteria *(mandatory)*

### Measurable Outcomes

- **SC-001**: Zero blank-page reproductions in the ensemble + chat regression suite across success, error, network-failure, and thrown-render scenarios (10/10 runs).
- **SC-002**: 100% of submit interactions in the React app go through the centralized state machine (verified by lint rule or grep test for direct `setVerdict` outside the reducer).
- **SC-003**: `bunx tsc --noEmit` reports 0 errors and `bun test` reports >= 13 prior tests + new shadcn regression tests all green.
- **SC-004**: shadcn primitive coverage: ensemble form and chat composer each render at least 4 shared shadcn components from `components/ui/`.
- **SC-005**: Bundle-size delta from migration stays under +150 KB gzipped for the React app's main chunk.

## Assumptions

- The React app at `src/typescript/react` remains the primary baseline; Electron and React Native consume `@banana/ui` types only.
- shadcn/ui's default Tailwind 3 setup is compatible with the existing `tailwind.config.ts` after adding the tokens block.
- The persistent state-management blank-page bug originates in submit-lifecycle state derivation in `App.tsx`, not in routing or navigation (the spike 042 guardrails already cover navigation paths).
- Bun continues to handle Radix peer-dependency installs without additional resolution overrides.
- No backend or contract changes are required to land the baseline.
