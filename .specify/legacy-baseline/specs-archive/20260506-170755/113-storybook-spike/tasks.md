# Tasks: Storybook Integration Spike (113)

**Input**: Spike spec at `.specify/specs/113-storybook-spike/spec.md`
**Prerequisites**: spec.md; shadcn baseline (048) delivered; React app on Bun + Vite 5 + React 19

> **Spike tasks only.** No production code lands on `main` from this spec. Story files and `.storybook/` config live on the spike branch and are cleaned up before close. Follow-up specs 114 and 115 are scaffolded as `main`-branch planning artifacts.

## Phase 1: Spike Branch Setup

- [x] T001 Create spike branch `feature/113-storybook-spike` from `main`.
- [x] T002 In `src/typescript/react`, run `bun add -d @storybook/react-vite storybook` and capture the full install output (success text or conflict errors) for `research.md`.
- [x] T003 Run `bunx storybook init --skip-install` (or `bunx storybook@latest init`) to generate `.storybook/main.ts` and `.storybook/preview.ts`; record any prompts and the generated config shape in `research.md`.
- [x] T004 Verify `bun run storybook` starts on `:6006` without crashing; note any Vite plugin coexistence issues with `@vitejs/plugin-react` in `research.md`.

## Phase 2: Minimal Story Authoring

- [x] T005 Write `src/typescript/react/src/components/ui/Button.stories.tsx` with a `Default`, `Disabled`, and `Loading` story (inline; no separate fixture files needed for the spike).
- [x] T006 Write `src/typescript/react/src/components/ui/Input.stories.tsx` with a `Default` and `WithValue` story.
- [x] T007 Write `src/typescript/react/src/components/ui/Textarea.stories.tsx` with a `Default` and `WithPlaceholder` story.
- [x] T008 Write `src/typescript/react/src/components/ui/Card.stories.tsx` with a `Default` story showing `CardHeader`, `CardTitle`, and `CardContent`.
- [x] T009 Write `src/typescript/react/src/components/ui/Alert.stories.tsx` with a `Default` and `Destructive` story.
- [x] T010 Run `bunx tsc --noEmit` from `src/typescript/react` with all story files present and document TypeScript errors (if any) and resolutions.

## Phase 3: Build and Compatibility Verification

- [x] T011 Run `bun run build-storybook` and record exit code, output, and `storybook-static/` directory size in `research.md`.
- [x] T012 Serve `storybook-static/` with `bunx serve storybook-static -p 6007` and confirm all five component stories render in a browser; document any rendering issues.
- [x] T013 Add one Storybook play function to `Button.stories.tsx` (click the default button, assert an `aria-label` or text updates) and verify `@storybook/test` works in the Bun runtime.
- [x] T014 Document Bun vs Node compatibility findings: peer dep conflicts, any `--bun` flag requirements, lockfile oddities, and any happy-dom polyfill gaps.

## Phase 4: Research Document

- [x] T015 Create `.specify/specs/113-storybook-spike/research.md` with sections: **Install Method**, **Bun Compatibility**, **Peer Conflicts**, **Vite Plugin Coexistence**, **TypeScript Path Resolution**, **Play Function Runtime**, **Build Output**, **Go/No-Go Recommendation**, **Follow-up Spec Links**.
- [x] T016 Populate each research section from T002–T014 findings; include exact commands run and exact error/success output snippets.

## Phase 5: Follow-up Spec Scaffolding (on `main`)

- [x] T017 Create `.specify/specs/114-storybook-baseline/spec.md` as a concrete implementation spec covering: full Storybook installation on `main`, story authoring for all shadcn primitives + key App panels, `bun run storybook` dev workflow, TypeScript integration, and `bun run build-storybook` producing a deployable static output. Scope and constraints MUST reflect spike findings from `research.md`.
- [x] T018 Create `.specify/specs/115-storybook-demos-ci/spec.md` as a CI publishing spec covering: GitHub Pages deployment of the Storybook static build on merge to `main`, a `storybook-deploy` workflow job, branch-preview Storybook links in PRs, and a public demo URL in the README.

## Phase 6: Cleanup and Validation

- [x] T019 Remove all story files and `.storybook/` config from `src/typescript/react` on the spike branch before creating any merge PR (spike artefacts stay on branch history only).
- [x] T020 Verify `src/typescript/react/package.json` has no Storybook deps on `main` after the follow-up spec stubs are committed.
- [x] T021 Run `ls .specify/specs/113-storybook-spike/research.md` and `ls .specify/specs/114-storybook-baseline/spec.md` and `ls .specify/specs/115-storybook-demos-ci/spec.md` to confirm all artefacts exist.
- [x] T022 Run `python scripts/validate-spec-tasks-parity.py --include-spikes .specify/specs/113-storybook-spike` and confirm `OK`.
