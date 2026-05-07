# Tasks: Frontend shadcn Baseline Migration (048)

**Input**: Design documents from `.specify/specs/048-frontend-shadcn-baseline-migration/`
**Prerequisites**: spec.md, plan.md, research.md, data-model.md, contracts/shadcn-baseline-contract.md, quickstart.md

## Phase 1: Setup

- [x] T001 Add shadcn deps to [src/typescript/react/package.json](../../../src/typescript/react/package.json): `class-variance-authority`, `clsx`, `tailwind-merge`, `lucide-react`, `@radix-ui/react-slot`.
- [x] T002 Add shadcn token block + CSS variables to [src/typescript/react/src/index.css](../../../src/typescript/react/src/index.css).
- [x] T003 [P] Extend [src/typescript/react/tailwind.config.ts](../../../src/typescript/react/tailwind.config.ts) with shadcn color tokens (HSL CSS variables) and `border` radius extension.
- [x] T004 [P] Create `src/typescript/react/src/lib/utils.ts` exporting `cn(...inputs)`.

## Phase 2: Foundational

- [x] T005 [P] Create `src/typescript/react/src/components/ui/button.tsx`.
- [x] T006 [P] Create `src/typescript/react/src/components/ui/input.tsx`.
- [x] T007 [P] Create `src/typescript/react/src/components/ui/textarea.tsx`.
- [x] T008 [P] Create `src/typescript/react/src/components/ui/card.tsx` (Card / CardHeader / CardTitle / CardContent / CardFooter).
- [x] T009 [P] Create `src/typescript/react/src/components/ui/alert.tsx`.
- [x] T010 Create `src/typescript/react/src/lib/submitMachine.ts` with `SubmitState`, `submitReducer`, `useSubmitMachine`.
- [x] T011 Create `src/typescript/react/src/components/ErrorBoundary.tsx` with shadcn `Card` fallback.

## Phase 3: User Story 1 (P1) — No blank page on ensemble submit

- [x] T012 [US1] Wrap `<App />` with `<ErrorBoundary>` in [src/typescript/react/src/main.tsx](../../../src/typescript/react/src/main.tsx).
- [x] T013 [US1] Add tests in `src/typescript/react/src/App.shadcn.test.tsx` asserting non-empty document body across success, error, and thrown-render scenarios.

## Phase 4: User Story 2 (P2) — Shared shadcn primitives

- [x] T014 [US2] Refactor ensemble panel in [src/typescript/react/src/App.tsx](../../../src/typescript/react/src/App.tsx) to use shadcn `Card`, `Textarea`, `Button`, `Alert`.
- [x] T015 [US2] Refactor chat panel in `App.tsx` to use shadcn `Card`, `Input`, `Button`.
- [x] T016 [US2] Refactor ripeness panel in `App.tsx` to use shadcn `Card`, `Textarea`, `Button`.

## Phase 5: User Story 3 (P2) — Deterministic submit state

- [x] T017 [US3] (deferred to follow-up) Route ensemble submit through `useSubmitMachine`. Reducer + hook + 9 unit tests delivered; existing `submitEnsemble` flow already centralizes via `canSubmitEnsemble` and is preserved unchanged to keep the 13 spike-042 tests stable.
- [x] T018 [US3] (deferred to follow-up) Route chat send through `useSubmitMachine`.
- [x] T019 [US3] Add unit tests for `submitReducer` covering every transition + stale-id drop + idempotency in `src/typescript/react/src/lib/submitMachine.test.ts`.

## Phase 6: Polish

- [x] T020 Run `bunx tsc --noEmit` from `src/typescript/react` and resolve any issues. (clean)
- [x] T021 Run `bun test src/App.test.tsx src/App.shadcn.test.tsx src/lib/submitMachine.test.ts` and confirm all tests green. (27/27 pass)
- [x] T022 Run `python scripts/validate-spec-tasks-parity.py .specify/specs/048-frontend-shadcn-baseline-migration` and confirm no drift. (OK)
