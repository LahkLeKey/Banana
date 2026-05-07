# Tasks: Frontend Shared Design System SPIKE

**Branch**: `021-frontend-shared-design-system-spike` | **Created**: 2026-04-26
**Type**: SPIKE -- investigation only, no production code changes.

## Phase 1 -- Setup

- [x] T001 Repoint `.specify/feature.json` ->
  `021-frontend-shared-design-system-spike`.
- [x] T002 Author `README.md` execution tracker.

## Phase 2 -- Component inventory

- [x] T003 Inventory current `shared/ui` exports under
  `src/typescript/shared/ui/src/` (components, props, styling
  primitives used). Record in `analysis/component-inventory.md`.
- [x] T004 Map each component to actual consumption by React,
  Electron, and React Native. Note styling overrides each channel
  applies. Append to `analysis/component-inventory.md`.

## Phase 3 -- Token + theming decision

- [x] T005 Propose the minimum-viable token set
  (color / typography / spacing / radius / motion) needed to unblock
  SPIKE 020's follow-up slices. Record rationale in
  `analysis/token-contract.md`.
- [x] T006 Decide how tokens flow into each channel
  (CSS variables for web/desktop; TS object for React Native; or a
  shared TS export consumed by both stacks). Record in
  `analysis/theming-strategy.md` with explicit handling of the React
  Native styling primitive.

## Phase 4 -- Readiness packets

- [x] T007 Author `analysis/followup-readiness-tokens.md` (slice 026):
  in-scope files, validation commands
  (`bun run --cwd src/typescript/shared/ui tsc --noEmit`),
  estimated task count (<=15).
- [x] T008 Author `analysis/followup-readiness-components-v2.md`
  (slice 027): in-scope files, validation commands
  (`bun run --cwd src/typescript/shared/ui tsc --noEmit`,
  vitest where applicable), estimated task count (<=20). MUST
  declare slice 026 as a hard prerequisite.
- [x] T009 Author `analysis/followup-readiness-channel-adoption.md`
  (slice 028): in-scope files, per-channel validation commands,
  estimated task count (<=20). MUST declare slice 027 as a hard
  prerequisite.
- [x] T010 Author `analysis/deferred-registry.md` -- any items pushed
  past slice 028 (e.g. Storybook adoption, Figma sync, dark-mode
  rollout) with explicit triggers.

## Phase 5 -- Close-out

- [x] T011 Update `README.md` to COMPLETE; mark all tasks `[x]`;
  cross-link slices 026, 027, 028 from this SPIKE's deliverables.

## Out of scope

- Writing any token, component, or per-channel adoption code (slices
  026-028 own that).
- Customer journey or verdict copy (SPIKE 020).
- Offline / persistence (SPIKE 022).
- New design tooling adoption unless concluded as the only viable
  path.
