# Implementation Plan: Frontend shadcn Baseline Migration

**Branch**: `feature/048-frontend-shadcn-baseline-migration` | **Date**: 2026-04-26 | **Spec**: [spec.md](./spec.md)

## Summary

Adopt shadcn/ui primitives in `src/typescript/react` and route every submit lifecycle through a single typed reducer + top-level error boundary so the ensemble and chat panels can never collapse to a blank document. The migration is bounded to the React app; `@banana/ui` remains the shared contract surface, and Electron/React Native are unchanged.

## Technical Context

**Language/Version**: TypeScript 5.5 + React 19
**Primary Dependencies**: shadcn/ui (Radix primitives, `class-variance-authority`, `clsx`, `tailwind-merge`, `lucide-react`), Tailwind 3.4, Vite 5, Bun 1.x
**Storage**: N/A (client-only); persistent verdict history retained via existing `@banana/resilience`
**Testing**: `bun test` (happy-dom) + `@testing-library/react`
**Target Platform**: Browser (Chromium baseline) + Vite dev server, container build via `docker/react.Dockerfile`
**Project Type**: Single React app at `src/typescript/react`
**Performance Goals**: Time-to-interactive parity with current baseline; main bundle delta < +150 KB gzipped
**Constraints**: No backend or `@banana/ui` contract changes; preserve `VITE_BANANA_API_BASE_URL`; preserve all 13 existing App tests
**Scale/Scope**: One React app, two primary panels (ensemble, chat), one ripeness panel, ~5 shadcn primitives.

## Constitution Check

- **Bun + Vite preserved** — yes (no package-manager change).
- **`@banana/ui` shared contracts preserved** — yes (only consumed; no shape changes).
- **`VITE_BANANA_API_BASE_URL` contract** — yes (untouched).
- **Container reproducibility** — yes (no host-only paths added; `docker/react.Dockerfile` continues to install via Bun).
- **One-window dev preserved** — yes (Vite stays on `:5173`).

## Project Structure

```text
.specify/specs/048-frontend-shadcn-baseline-migration/
├── plan.md
├── research.md
├── data-model.md
├── quickstart.md
├── contracts/
│   └── shadcn-baseline-contract.md
├── checklists/
│   └── requirements.md
└── tasks.md
```

```text
src/typescript/react/
├── package.json            # add shadcn deps
├── tailwind.config.ts      # add shadcn token block
├── src/
│   ├── index.css           # add shadcn CSS variables
│   ├── main.tsx            # wrap App with ErrorBoundary
│   ├── App.tsx             # refactor ensemble + chat panels
│   ├── App.test.tsx        # extended regression tests
│   ├── lib/
│   │   ├── utils.ts        # cn() helper
│   │   └── submitMachine.ts# reducer + useSubmitMachine hook
│   └── components/
│       ├── ErrorBoundary.tsx
│       └── ui/
│           ├── button.tsx
│           ├── input.tsx
│           ├── textarea.tsx
│           ├── card.tsx
│           └── alert.tsx
```

**Structure Decision**: single-app refactor inside `src/typescript/react`; no new packages, no contract changes.

## Phase 0 — Research (see `research.md`)

Key decisions:
- Adopt shadcn primitives manually (not via `npx shadcn` CLI) to keep Bun-first install and avoid Node CLI churn.
- Use a discriminated-union reducer instead of a state library; React 19's `useReducer` is sufficient for the two flows in scope.
- Wrap only at the top level with one `ErrorBoundary`; per-panel boundaries are deferred.

## Phase 1 — Design

- **Data model** (`data-model.md`): `SubmitState`, `SubmitAction`, `submitReducer`, `useSubmitMachine`.
- **Contracts** (`contracts/shadcn-baseline-contract.md`): public component props for each `components/ui/` primitive, error-boundary contract, reducer transition table.
- **Quickstart** (`quickstart.md`): bun install, run, test commands.

## Phase 2 — Tasks

Tasks live in `tasks.md` (generated separately). Sequence:

1. Setup: deps, tailwind tokens, css variables, `cn()` util.
2. Foundational: `components/ui/{button,input,textarea,card,alert}.tsx`, `ErrorBoundary`, `submitMachine`.
3. US1 (P1): wrap App, add blank-page non-regression tests.
4. US2 (P2): convert ensemble + chat + ripeness panels to shadcn primitives.
5. US3 (P2): route ensemble + chat submit through reducer.
6. Polish: tests pass, `tsc --noEmit` clean, parity validation.

## Complexity Tracking

| Violation | Why Needed | Simpler Alternative Rejected Because |
|-----------|------------|--------------------------------------|
| Adding 5 new UI primitives | shadcn baseline mandates a shared component set | Reusing inline Tailwind classes leaves divergence and is the source of the current state-management tangle |
| Top-level `ErrorBoundary` | React surfaces render errors as blank trees | Per-panel try/catch cannot catch render-time errors |
