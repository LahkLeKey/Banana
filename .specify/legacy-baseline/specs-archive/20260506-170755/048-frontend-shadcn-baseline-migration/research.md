# Research: Frontend shadcn Baseline Migration (048)

## Decision: Manual shadcn install over CLI

**Context**: shadcn/ui is normally added via `npx shadcn add <component>`, which writes files into `components/ui/` and patches `tailwind.config`.

**Decision**: Add the dependencies (`class-variance-authority`, `clsx`, `tailwind-merge`, `lucide-react`, `@radix-ui/react-slot`) directly to `package.json` and copy minimal component sources by hand. Skip Node CLI invocation.

**Why**:
- Repository uses Bun as the canonical package manager; running a Node CLI muddles install behavior and lockfile parity.
- Only ~5 primitives are needed for the baseline; manual copies are smaller surface area than a CLI integration.
- Keeps the migration self-contained and reproducible inside Docker (`docker/react.Dockerfile`).

**Alternatives rejected**: `npx shadcn` CLI (breaks Bun-first install path), full Radix wrapping without shadcn styling (loses the "good baseline" intent).

## Decision: `useReducer` discriminated-union state over external store

**Context**: The remaining blank-page bug is a state-management regression where partial state derivations can short-circuit the React tree to `null`.

**Decision**: Use a single typed reducer (`SubmitState = { kind: 'idle' | 'submitting' | 'success' | 'error', ... }`) per flow, exposed as `useSubmitMachine`. No Zustand/Jotai/Redux.

**Why**:
- React 19's `useReducer` + discriminated unions give exhaustive-switch guarantees with `never`-checks at compile time.
- Avoids new runtime dependencies; aligns with existing app's "no global store" pattern.
- Two flows (ensemble, chat) — well below the threshold where an external store helps.

**Alternatives rejected**: Zustand (added dep), XState (over-modeled for this scope), one mega-`useState` (the current pattern that produced the bug).

## Decision: Single top-level `ErrorBoundary`

**Context**: React render errors propagate up; without a boundary, the root unmounts and `<div id="root">` becomes empty — exactly the "blank page" symptom.

**Decision**: One class-based `ErrorBoundary` wrapping `<App />` in `main.tsx`, rendering a shadcn `Card` + `Button` retry fallback.

**Why**: Single guarantee point; simplest invariant ("root never empty"). Per-panel boundaries can be added later when a panel-isolation requirement appears.

**Alternatives rejected**: `react-error-boundary` package (extra dep for a 30-line class), per-panel boundaries (premature).

## Decision: Keep `@banana/ui` as the shared contract surface

**Context**: `@banana/ui` already exposes `BananaBadge`, `RipenessLabel`, `EscalationPanel`, `ErrorText`, `RetryButton`, plus all shared types.

**Decision**: Do not rewrite or rebrand `@banana/ui`. shadcn primitives live only inside the React app at `src/typescript/react/src/components/ui/`. The React app composes shadcn primitives + `@banana/ui` widgets together.

**Why**: Cross-app contract stability (Electron/RN consume the same package). Migration scope stays bounded.

**Alternatives rejected**: Move shadcn into `@banana/ui` (forces RN/Electron impact, out of scope).

## Decision: Preserve all spike-042 guardrails

**Context**: Spike 042 added `preventNativeSubmitNavigation`, `canSubmitEnsemble`, `normalizeEnsembleSample`, location-snapshot classification, Ctrl/Cmd+Enter handler, `type="button"`.

**Decision**: All 042 helpers remain and are wired through the new shadcn-rendered submit handlers.

**Why**: They cover the navigation-class blank page; the reducer covers the render-class blank page. Both classes need durable defenses.
