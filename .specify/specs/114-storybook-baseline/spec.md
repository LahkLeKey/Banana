# Feature Specification: Storybook Baseline

**Feature Branch**: `feature/114-storybook-baseline`
**Created**: 2026-05-02
**Status**: Draft (pending spike 113 findings)
**Input**: Scaffolded from spike spec `113-storybook-spike`; finalize after `research.md` is complete.

> **Prerequisite**: Spike `113-storybook-spike` must be complete and `research.md` reviewed before this spec is planned and tasked. Constraints and technical choices below may be updated based on spike findings.

## In Scope *(mandatory)*

- Install and configure Storybook 8.x (`@storybook/react-vite`) in `src/typescript/react` on `main` with Bun as the package manager.
- Write a canonical story for each shadcn primitive: `Button`, `Input`, `Textarea`, `Card`, `Alert` — with at least `Default`, one variant, and one interactive play-function story per component.
- Write stories for the two primary App panels (`EnsemblePanel.stories.tsx`, `ChatPanel.stories.tsx`) and the `ErrorBoundary` fallback.
- Add `bun run storybook` (dev, port `:6006`) and `bun run build-storybook` (static output to `storybook-static/`) scripts to `src/typescript/react/package.json`.
- Ensure `bunx tsc --noEmit`, `bun test`, and `bun run build` all pass with Storybook deps installed.
- Document the Storybook local dev workflow in `docs/storybook.md`.

## Out of Scope *(mandatory)*

- Deploying or publishing the Storybook to any host (that is spec 115).
- Storybook for Electron, React Native, or `@banana/ui`.
- Visual regression snapshots or Chromatic integration.
- Any ASP.NET, native, or training pipeline changes.

## User Scenarios & Testing *(mandatory)*

### User Story 1 - Developer browses components in isolation (Priority: P1)

A contributor runs `bun run storybook` and can interactively browse all shadcn primitives and panel components with controls and play functions without starting the full Compose stack.

**Acceptance Scenarios**:

1. **Given** `bun run storybook` starts, **When** a developer opens `:6006`, **Then** all component stories render with no console errors.
2. **Given** a play function on `Button`, **When** Storybook runs it, **Then** the interaction passes.

---

### User Story 2 - `bun run build-storybook` produces deployable static output (Priority: P1)

CI or a developer can build a static Storybook that can be deployed to GitHub Pages or served locally.

**Acceptance Scenarios**:

1. **Given** `bun run build-storybook` runs, **When** it completes, **Then** `storybook-static/` exists and `bunx serve storybook-static` renders all stories in a browser.

## Requirements *(mandatory)*

### Functional Requirements

- **FR-001**: `@storybook/react-vite` and required peer deps MUST be installable via `bun install` without manual lockfile overrides (or with documented minimal overrides from spike findings).
- **FR-002**: Stories MUST exist for: `Button`, `Input`, `Textarea`, `Card`, `Alert`, `EnsemblePanel`, `ChatPanel`, `ErrorBoundary`.
- **FR-003**: `bun run build-storybook` MUST exit 0 and produce a valid `storybook-static/` directory.
- **FR-004**: `bunx tsc --noEmit`, `bun test`, and `bun run build` MUST all continue to pass with Storybook deps added.
- **FR-005**: `docs/storybook.md` MUST document how to start and build Storybook locally.

## Success Criteria *(mandatory)*

### Validation Lane

```bash
bun run build-storybook --cwd src/typescript/react
bunx tsc --noEmit --cwd src/typescript/react
bun test --cwd src/typescript/react
python scripts/validate-spec-tasks-parity.py .specify/specs/114-storybook-baseline
```

### Measurable Outcomes

- **SC-001**: `bun run build-storybook` exits 0.
- **SC-002**: Stories exist for all 8 required components/panels.
- **SC-003**: Zero TypeScript errors after adding Storybook.
- **SC-004**: All existing `bun test` tests continue to pass.

## Assumptions

- Spike 113 confirms Storybook 8.x is compatible with Bun + Vite 5 + React 19 (go recommendation).
- Any peer-dep workarounds found in the spike are documented and applied here.
- `storybook-static/` is git-ignored (too large to commit); deployment is handled by spec 115.
