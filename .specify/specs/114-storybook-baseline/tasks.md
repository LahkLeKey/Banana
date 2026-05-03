# Tasks: Storybook Baseline (114)

**Input**: Spec at `.specify/specs/114-storybook-baseline/spec.md`
**Prerequisites**: spike `113-storybook-spike` complete with a go recommendation; Bun + Vite React app remains the delivery target

## Phase 1: Install and Configure Storybook

- [ ] T001 Add Storybook dependencies to [src/typescript/react/package.json](../../../src/typescript/react/package.json): `@storybook/react-vite`, `storybook`, and any minimal peer dependencies validated by spike 113.
- [ ] T002 Add `storybook` and `build-storybook` scripts to [src/typescript/react/package.json](../../../src/typescript/react/package.json) targeting port `6006` and static output `storybook-static/`.
- [ ] T003 Create [src/typescript/react/.storybook/main.ts](../../../src/typescript/react/.storybook/main.ts) and [src/typescript/react/.storybook/preview.ts](../../../src/typescript/react/.storybook/preview.ts) using the Bun-compatible Vite configuration established by spike 113.

## Phase 2: Story Authoring

- [ ] T004 Create `Button.stories.tsx` for [src/typescript/react/src/components/ui/button.tsx](../../../src/typescript/react/src/components/ui/button.tsx) with `Default`, variant, and play-function stories.
- [ ] T005 Create `Input.stories.tsx` for [src/typescript/react/src/components/ui/input.tsx](../../../src/typescript/react/src/components/ui/input.tsx).
- [ ] T006 Create `Textarea.stories.tsx` for [src/typescript/react/src/components/ui/textarea.tsx](../../../src/typescript/react/src/components/ui/textarea.tsx).
- [ ] T007 Create `Card.stories.tsx` for [src/typescript/react/src/components/ui/card.tsx](../../../src/typescript/react/src/components/ui/card.tsx).
- [ ] T008 Create `Alert.stories.tsx` for [src/typescript/react/src/components/ui/alert.tsx](../../../src/typescript/react/src/components/ui/alert.tsx).
- [ ] T009 Create `EnsemblePanel.stories.tsx`, `ChatPanel.stories.tsx`, and `ErrorBoundary.stories.tsx` under `src/typescript/react/src/` or the nearest component-owned story location.

## Phase 3: Compatibility and Docs

- [ ] T010 Run `bunx tsc --noEmit --cwd src/typescript/react` and resolve Storybook-related type issues.
- [ ] T011 Run `bun test --cwd src/typescript/react` and confirm existing React tests still pass.
- [ ] T012 Run `bun run build --cwd src/typescript/react` and confirm the Vite app still builds cleanly with Storybook deps installed.
- [ ] T013 Run `bun run build-storybook --cwd src/typescript/react` and confirm `storybook-static/` is produced.
- [ ] T014 Create `docs/storybook.md` documenting local Storybook usage, expected port `6006`, and the `build-storybook` flow.

## Phase 4: Validation

- [ ] T015 Run `python scripts/validate-spec-tasks-parity.py .specify/specs/114-storybook-baseline` and confirm `OK`.
