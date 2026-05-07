# Feature Specification: Storybook Demos CI (GitHub Pages)

**Feature Branch**: `feature/115-storybook-demos-ci`
**Created**: 2026-05-02
**Status**: Draft (pending spike 113 and baseline 114)
**Input**: Scaffolded from spike spec `113-storybook-spike`; finalize after spec 114 is delivered.

> **Prerequisites**: Spec `113-storybook-spike` complete and spec `114-storybook-baseline` delivered (Storybook installed and building on `main`) before this spec is planned and tasked.

## In Scope *(mandatory)*

- A GitHub Actions workflow (`.github/workflows/storybook-deploy.yml`) that builds the Storybook static output on every merge to `main` and deploys it to GitHub Pages.
- A PR preview comment bot (via `actions/deploy-pages` or a third-party action) that posts a temporary Storybook preview link on each pull request touching `src/typescript/react`.
- A public Storybook demo URL added to the root `README.md` (under a `## Component Demos` section) pointing to the GitHub Pages deployment.
- A `storybook-static/` `.gitignore` entry to ensure the build output is never committed to the repo.
- A CI badge for the Storybook deployment status in `README.md`.

## Out of Scope *(mandatory)*

- Chromatic or paid visual regression services.
- Storybook for Electron, React Native, or `@banana/ui`.
- Any ASP.NET, native, or training pipeline changes.
- Self-hosted or non-GitHub Pages deployments.

## User Scenarios & Testing *(mandatory)*
## Problem Statement

Feature Specification: Storybook Demos CI (GitHub Pages) aims to improve system capability and user experience by implementing the feature described in the specification.


### User Story 1 - Public demo link in README (Priority: P1)

A developer visiting the GitHub repo sees a `## Component Demos` section in the README with a Storybook link they can click to browse UI components without cloning.

**Acceptance Scenarios**:

1. **Given** a merge to main, **When** the deploy workflow runs, **Then** the GitHub Pages URL is live and serves all component stories.
2. **Given** the README contains a Storybook badge/link, **When** a user clicks it, **Then** the Storybook renders correctly in the browser.

---

### User Story 2 - PR preview link (Priority: P2)

A reviewer can click a Storybook preview link in the PR comment to see component changes rendered in isolation before merging.

**Acceptance Scenarios**:

1. **Given** a PR touches `src/typescript/react`, **When** CI runs, **Then** a bot comments with a temporary Storybook preview URL.
2. **Given** the PR is closed or merged, **When** the preview URL is accessed, **Then** it returns a 404 or a clear "preview expired" message.

## Requirements *(mandatory)*

### Functional Requirements

- **FR-001**: `.github/workflows/storybook-deploy.yml` MUST run on `push` to `main`, build `storybook-static/`, and deploy to GitHub Pages using `actions/deploy-pages`.
- **FR-002**: The workflow MUST gate deployment on `bun run build-storybook` exit 0.
- **FR-003**: `README.md` MUST contain a `## Component Demos` section with the GitHub Pages Storybook URL and a deployment status badge.
- **FR-004**: `storybook-static/` MUST be listed in `.gitignore`.
- **FR-005**: PR preview deployment MUST be scoped to PRs that touch files under `src/typescript/react/**`.

## Success Criteria *(mandatory)*

### Validation Lane

```bash
# Deploy workflow exists
ls .github/workflows/storybook-deploy.yml
# gitignore entry present
grep "storybook-static" .gitignore
# README has demo section
grep "Component Demos" README.md
python scripts/validate-spec-tasks-parity.py .specify/specs/115-storybook-demos-ci
```

### Measurable Outcomes

- **SC-001**: GitHub Pages deployment URL is live after merge to `main`.
- **SC-002**: `README.md` contains the Storybook URL and badge.
- **SC-003**: `storybook-static/` is git-ignored.
- **SC-004**: The deploy workflow exits 0 on a clean build.

## Assumptions

- Spec 114 is complete and `bun run build-storybook` produces a valid `storybook-static/` directory.
- The repo has GitHub Pages enabled (GitHub Pages via GitHub Actions source).
- PR preview deployments use a free/included GitHub Actions mechanism (no paid service required).
