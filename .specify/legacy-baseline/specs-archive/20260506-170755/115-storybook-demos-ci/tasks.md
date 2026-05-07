# Tasks: Storybook Demos CI (115)

**Input**: Spec at `.specify/specs/115-storybook-demos-ci/spec.md`
**Prerequisites**: spec 114 delivered; `bun run build-storybook` produces a valid `storybook-static/` directory

## Phase 1: Deployment Workflow

- [x] T001 Create [.github/workflows/storybook-deploy.yml](../../../.github/workflows/storybook-deploy.yml) to build `storybook-static/` on pushes to `main` and deploy it to GitHub Pages using `actions/deploy-pages`.
- [x] T002 Gate deployment on `bun run build-storybook --cwd src/typescript/react` exiting 0.
- [x] T003 Limit PR preview behavior to changes under `src/typescript/react/**` so unrelated PRs do not publish Storybook previews.

## Phase 2: Repo Wiring

- [x] T004 Add `storybook-static/` to [.gitignore](../../../.gitignore).
- [x] T005 Add a `## Component Demos` section to [README.md](../../../README.md) with the GitHub Pages Storybook URL and a deploy-status badge.
- [x] T006 Add PR preview comment wiring in `.github/workflows/storybook-deploy.yml` so qualifying PRs receive a Storybook preview link.

## Phase 3: Validation

- [x] T007 Run `ls .github/workflows/storybook-deploy.yml` and confirm the workflow exists.
- [x] T008 Run `grep "storybook-static" .gitignore` and confirm the ignore entry is present.
- [x] T009 Run `grep "Component Demos" README.md` and confirm the README demo section is present.
- [x] T010 Run `python scripts/validate-spec-tasks-parity.py .specify/specs/115-storybook-demos-ci` and confirm `OK`.
