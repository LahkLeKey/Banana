# Feature Specification: Storybook Integration Spike

**Feature Branch**: `feature/113-storybook-spike`
**Created**: 2026-05-02
**Status**: Spike
**Input**: User description: "storybook integrations for demos do a spike and scaffold follow up specs"

> **This is a spike.** The goal is to answer open technical questions about Storybook compatibility with the current Bun + Vite + React 19 + shadcn stack, then produce a concrete implementation spec (`114-storybook-baseline`) and a demo-publishing spec (`115-storybook-demos-ci`) as follow-ups. No production code is expected to land from this spike.

## In Scope *(mandatory)*

- Verify that `@storybook/react-vite` (Storybook 8.x) installs and initializes cleanly with Bun as the package manager inside `src/typescript/react`.
- Verify that the five shadcn components from spec 048 (`Button`, `Input`, `Textarea`, `Card`, `Alert`) can each have a minimal Story written against them with no TypeScript errors.
- Verify that `@storybook/test` (play functions) works in the Bun + happy-dom environment.
- Verify that `bun run storybook` starts the Storybook dev server on a non-conflicting port (`:6006`) without interfering with the Vite dev server on `:5173`.
- Verify that `bun run build-storybook` produces a static `storybook-static/` output that can be served as a GitHub Pages site.
- Document all blockers, workarounds, and incompatibilities discovered in `.specify/specs/113-storybook-spike/research.md`.
- Scaffold follow-up specs `114-storybook-baseline` and `115-storybook-demos-ci` as stub directories with `spec.md` files, drawing from spike findings.

## Out of Scope *(mandatory)*

- Adding Storybook to the production CI pipeline (that is spec 115).
- Writing a full set of stories for every component (that is spec 114).
- Publishing the Storybook to GitHub Pages or deploying to any host (that is spec 115).
- Storybook for Electron, React Native, or `@banana/ui` — those are out of scope for this spike.
- Any ASP.NET, native, or training pipeline changes.

## User Scenarios & Testing *(mandatory)*
## Problem Statement

Feature Specification: Storybook Integration Spike aims to improve system capability and user experience by implementing the feature described in the specification.


### User Story 1 - Confirm Bun + Vite + React 19 + shadcn compatibility (Priority: P1)

A developer runs `bun add -d @storybook/react-vite` and `bunx storybook init` in `src/typescript/react` and can start the Storybook server without peer-dependency conflicts or Bun-specific install failures.

**Why this priority**: If Storybook cannot install cleanly under Bun, the rest of the spike is moot.

**Independent Test**: `bun run storybook` starts on `:6006` and the browser renders at least one story without console errors.

**Acceptance Scenarios**:

1. **Given** the spike installs Storybook deps, **When** `bun run storybook` runs, **Then** the Storybook dev server starts and the default stories render without errors.
2. **Given** a peer-dependency conflict, **When** it is encountered, **Then** the spike documents the exact conflict and the minimal workaround (override, resolution, or lockfile patch).

---

### User Story 2 - Minimal stories for all five shadcn primitives (Priority: P1)

A contributor can open Storybook and interactively browse the five shadcn primitives (`Button`, `Input`, `Textarea`, `Card`, `Alert`) with at least a `Default` story and one interaction variant each.

**Why this priority**: Proves the component contract is story-writable and validates the shadcn prop shapes are usable standalone.

**Independent Test**: `bunx storybook build` exits 0 and the static output contains story entries for all five components.

**Acceptance Scenarios**:

1. **Given** stories exist for all five primitives, **When** `bun run build-storybook` runs, **Then** it exits 0 and `storybook-static/` contains the expected story manifest.
2. **Given** a play function is added to `Button.stories.tsx`, **When** Storybook runs it, **Then** the play function passes without timeout.

---

### User Story 3 - Follow-up specs scaffolded from spike findings (Priority: P1)

After the spike, `.specify/specs/114-storybook-baseline/spec.md` and `.specify/specs/115-storybook-demos-ci/spec.md` exist as concrete, actionable specs informed by the compatibility findings.

**Why this priority**: A spike with no follow-up plan delivers no durable value.

**Independent Test**: Both follow-up spec files exist and pass `validate-spec-tasks-parity.py` structure checks (spec.md present; tasks.md not required for spike-scaffolded stubs).

**Acceptance Scenarios**:

1. **Given** the spike completes, **When** the scaffolding step runs, **Then** `114-storybook-baseline/spec.md` and `115-storybook-demos-ci/spec.md` both exist with In Scope, Out of Scope, and Success Criteria sections.
2. **Given** blockers were found, **When** the follow-up specs are written, **Then** each blocker is captured as either a constraint or a pre-condition in the relevant spec.

---

### Edge Cases

- Storybook 8.x requires Node resolution for some plugins: document if `bun --bun` flag resolves it or if a wrapper is needed.
- `@storybook/react-vite` Vite plugin conflicts with `@vitejs/plugin-react`: document the coexistence pattern.
- `happy-dom` vs `jsdom` environment differences for Storybook play tests: document which global polyfills Storybook expects.
- Storybook auto-generates `.storybook/main.ts` that may conflict with project `tsconfig.json` paths: document the adjustment.

## Requirements *(mandatory)*

### Functional Requirements

- **FR-001**: The spike MUST attempt `bun add -d @storybook/react-vite storybook` inside `src/typescript/react` in a throwaway branch and document the exact install output (success or failure with error text).
- **FR-002**: The spike MUST write five minimal story files (`Button.stories.tsx`, `Input.stories.tsx`, `Textarea.stories.tsx`, `Card.stories.tsx`, `Alert.stories.tsx`) under `src/typescript/react/src/components/ui/` and verify `bunx tsc --noEmit` passes with them present.
- **FR-003**: The spike MUST record whether `bun run storybook` (`:6006`) starts without conflicting with `bun run dev` (`:5173`) in a parallel terminal.
- **FR-004**: The spike MUST record whether `bun run build-storybook` produces a valid `storybook-static/` directory that can be served by `bunx serve storybook-static`.
- **FR-005**: The spike MUST produce `.specify/specs/113-storybook-spike/research.md` documenting: install method, Bun compatibility notes, peer conflicts, Vite plugin coexistence, TypeScript path resolution, play-function runtime, build output structure, and a go/no-go recommendation for spec 114.
- **FR-006**: The spike MUST scaffold `.specify/specs/114-storybook-baseline/spec.md` as a concrete implementation spec with In Scope, Out of Scope, User Stories, Requirements, and Success Criteria sections informed by FR-001–FR-004 findings.
- **FR-007**: The spike MUST scaffold `.specify/specs/115-storybook-demos-ci/spec.md` as a concrete CI publishing spec covering GitHub Pages deployment of the static Storybook build.
- **FR-008**: All spike artefacts (story files, `.storybook/` config) MUST be removed from `src/typescript/react` before the spike branch is merged so no half-baked Storybook config lands in production code.
- **FR-009**: The spike MUST NOT add Storybook deps to `src/typescript/react/package.json` on `main` — changes stay on the spike branch until spec 114 is approved.

### Key Entities

- **Spike Branch**: `feature/113-storybook-spike` — throwaway branch for install experimentation; not merged to main.
- **`research.md`**: The canonical spike findings document at `.specify/specs/113-storybook-spike/research.md`.
- **Follow-up spec stubs**: `114-storybook-baseline/spec.md` and `115-storybook-demos-ci/spec.md` — created on `main` as planning artifacts independent of the spike branch.
- **`.storybook/`**: Storybook config directory created by `storybook init`; lives in `src/typescript/react/.storybook/` on the spike branch only.

## Success Criteria *(mandatory)*

### Validation Lane

```bash
# Spike research doc exists
ls .specify/specs/113-storybook-spike/research.md
# Follow-up specs scaffolded
ls .specify/specs/114-storybook-baseline/spec.md
ls .specify/specs/115-storybook-demos-ci/spec.md
# Parity check passes for all three specs (tasks.md not required for spikes/stubs)
python scripts/validate-spec-tasks-parity.py --include-spikes .specify/specs/113-storybook-spike
```

### Measurable Outcomes

- **SC-001**: `research.md` exists and contains sections covering: install method, compatibility verdict (go/no-go), known blockers, and follow-up spec links.
- **SC-002**: `.specify/specs/114-storybook-baseline/spec.md` and `.specify/specs/115-storybook-demos-ci/spec.md` both exist with all mandatory spec sections.
- **SC-003**: On the spike branch, `bunx storybook build` exits 0 (or the research.md documents exactly why it doesn't and what the workaround is).
- **SC-004**: Spike branch leaves `src/typescript/react/package.json` and `main` branch unchanged — no Storybook deps committed to `main`.
- **SC-005**: `python scripts/validate-spec-tasks-parity.py --include-spikes .specify/specs/113-storybook-spike` exits 0 (spike spec recognized and not flagged as gap).

## Assumptions

- Storybook 8.x is the current stable release and supports `@storybook/react-vite` with Vite 5.
- Bun can install Storybook's peer deps without requiring a Node shim, or any such shim is minimal and documentable.
- The spike branch is short-lived (1–2 days of investigation) and its production artefacts are cleaned up before any merge.
- Follow-up specs 114 and 115 will be planned and tasked separately after spike findings are reviewed.
