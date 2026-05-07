# Tasks: Playwright E2E Screenshot Suite (112)

**Input**: Design spec at `.specify/specs/112-playwright-e2e-screenshot-suite/spec.md`
**Prerequisites**: spec.md, shadcn baseline (048) delivered, React app builds with `bun run build`

## Phase 1: Playwright Setup

- [x] T001 Add `@playwright/test` as a dev dependency in [src/typescript/react/package.json](../../../src/typescript/react/package.json) and add a `screenshots` script entry: `bunx playwright test --project=screenshots`.
- [x] T002 Create `tests/e2e/playwright/playwright.config.ts` defining a `screenshots` project with Chromium, `baseURL` from `PLAYWRIGHT_BASE_URL` env (default `http://localhost:5173`), and a screenshot output directory of `../../../docs/screenshots`.
- [x] T003 Create `tests/e2e/playwright/screenshots.config.ts` exporting a typed `ScreenshotManifest` array covering: ensemble idle, ensemble submitting, ensemble success, ensemble error, chat empty, chat with messages, ripeness idle, ripeness result, error boundary fallback (9 captures minimum → target 12).
- [x] T004 Create `docs/screenshots/` directory with a `.gitkeep` placeholder so the directory is committed before any screenshots exist.

## Phase 2: Playwright Spec Files

- [x] T005 Create `tests/e2e/playwright/specs/ensemble.spec.ts` capturing ensemble idle state, ensemble submitting state (mocked slow response), ensemble success state (mocked response), and ensemble error state (mocked rejection) using `page.route()` intercepts.
- [x] T006 Create `tests/e2e/playwright/specs/chat.spec.ts` capturing chat panel empty state and chat panel with one message sent (mocked `/api/chat` response).
- [x] T007 Create `tests/e2e/playwright/specs/ripeness.spec.ts` capturing ripeness idle and ripeness result states via `page.route()` mock.
- [x] T008 Create `tests/e2e/playwright/specs/error-boundary.spec.ts` capturing the `ErrorBoundary` fallback card by injecting a React render error through `page.evaluate()`.
- [x] T009 Create a shared `tests/e2e/playwright/helpers/mock-api.ts` module with typed `routeEnsemble()`, `routeChat()`, `routeRipeness()` helpers so all specs share one mock-API surface.

## Phase 3: README Gallery Update Script

- [x] T010 Insert `<!-- GALLERY_START -->` and `<!-- GALLERY_END -->` markers into [README.md](../../../README.md) in a new `## UI Gallery` section (manual one-time insertion, placed after the intro paragraph and before `## Compose Run Profiles`).
- [x] T011 Create `scripts/update-readme-screenshots.sh` that: reads all `docs/screenshots/*.png` sorted by filename, generates `![<label>](docs/screenshots/<file>)` lines grouped by panel prefix, and replaces the content between `<!-- GALLERY_START -->` and `<!-- GALLERY_END -->` in `README.md`. Script must be idempotent (no diff on second run with unchanged screenshots).
- [x] T012 Add `#!/usr/bin/env bash` and `set -euo pipefail` to `scripts/update-readme-screenshots.sh` and enforce LF line endings in `.gitattributes`.

## Phase 4: CI Job

- [x] T013 Add a `screenshot-suite` job to `.github/workflows/compose-ci.yml` (or create `.github/workflows/screenshot-ci.yml`) that: checks out the repo, sets up Bun, installs deps (`bun install --cwd src/typescript/react`), installs Playwright Chromium (`bunx playwright install chromium --with-deps`), builds the React app (`bun run build --cwd src/typescript/react`), serves via `bunx serve` or `bun run preview`, waits for the server (`bunx wait-on http://localhost:5173`), runs `bunx playwright test --project=screenshots`, calls `bash scripts/update-readme-screenshots.sh`, and uploads `docs/screenshots` as a CI artifact named `ui-screenshots`.
- [x] T014 Gate the `screenshot-suite` job on `push` to `main` only (not PRs) so screenshot commits do not create CI recursion loops.

## Phase 5: Polish & Validation

- [x] T015 Run `bunx tsc --noEmit` from `tests/e2e/playwright` (or include playwright types in a `tsconfig.json`) and ensure no type errors.
- [x] T016 Run `bunx playwright test --project=screenshots --reporter=list` locally against `bun run preview` and confirm all 10+ captures exit 0.
- [x] T017 Run `bash scripts/update-readme-screenshots.sh` and confirm `README.md` diff shows only the gallery section updated, then run it again and confirm zero diff (idempotency).
- [x] T018 Run `grep -c "GALLERY_START" README.md` and confirm output is `1`; run `grep -c "docs/screenshots" README.md` and confirm count >= 6.
- [x] T019 Run `python scripts/validate-spec-tasks-parity.py .specify/specs/112-playwright-e2e-screenshot-suite` and confirm `OK`.
- [x] T020 Run `bunx playwright test --project=screenshots` and confirm `SC-001`: at least 10 screenshots captured, all exit 0.
