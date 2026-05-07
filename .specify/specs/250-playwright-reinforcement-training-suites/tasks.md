# Tasks: Playwright Reinforcement Training Suites (250)

## Phase 1: Shared Infrastructure

- [x] T001 Create `tests/e2e/playwright/helpers/training-feedback.ts` with `appendFeedbackEntry`, `makeBananaEntry`, `makeRipenessEntry`, and `DISAGREEMENT_THRESHOLD`.
  Files: `tests/e2e/playwright/helpers/training-feedback.ts`

- [x] T002 Create `data/ripeness/feedback/` directory and empty `inbox.json` (`[]`) so the ripeness suite has a write target.
  Files: `data/ripeness/feedback/inbox.json`

## Phase 2: Banana / Not-Banana Suite

- [x] T003 Create `tests/e2e/playwright/specs/banana-reinforcement.spec.ts` with 14 labeled prompts (5 banana, 5 not-banana, 4 boundary edge cases).
  Files: `tests/e2e/playwright/specs/banana-reinforcement.spec.ts`

- [x] T004 Wire each banana suite test to write a feedback entry to `data/not-banana/feedback/inbox.json` via the `training-feedback` helper.
  Files: `tests/e2e/playwright/specs/banana-reinforcement.spec.ts`

- [x] T005 Add post-suite disagreement rate assertion (`DISAGREEMENT_THRESHOLD = 0.4`) to the banana suite.
  Files: `tests/e2e/playwright/specs/banana-reinforcement.spec.ts`

## Phase 3: Ripeness Suite

- [x] T006 Create `tests/e2e/playwright/specs/ripeness-reinforcement.spec.ts` with 12 labeled prompts (3 per label class, minimum 1 boundary per class from `challenge-set.json`).
  Files: `tests/e2e/playwright/specs/ripeness-reinforcement.spec.ts`

- [x] T007 Wire each ripeness suite test to call the ripeness API via `page.request` and write entries to `data/ripeness/feedback/inbox.json`.
  Files: `tests/e2e/playwright/specs/ripeness-reinforcement.spec.ts`

- [x] T008 Add per-label disagreement rate assertion to the ripeness suite.
  Files: `tests/e2e/playwright/specs/ripeness-reinforcement.spec.ts`

## Phase 4: Orchestrator Script

- [x] T009 Create `scripts/run-playwright-training-suites.sh` accepting `--mode live|mock` and `--apply` flags; emit a JSON session summary to `artifacts/playwright-training/`.
  Files: `scripts/run-playwright-training-suites.sh`

- [x] T010 Add mock-mode API stubs to `tests/e2e/playwright/helpers/mock-api.ts` for the ripeness endpoint (`/api/ripeness`) so suites can run fully offline in CI.
  Files: `tests/e2e/playwright/helpers/mock-api.ts`

## Phase 5: CI Workflow

- [x] T011 Create `.github/workflows/playwright-reinforcement.yml` with `push`+`workflow_dispatch` triggers, `build-react` → `run-suites` jobs, and artifact upload for the session summary.
  Files: `.github/workflows/playwright-reinforcement.yml`

## Validation

- [x] T012 Run `bun run --cwd src/typescript/react tsc --noEmit` — zero errors.
  Commands: `bun run --cwd src/typescript/react tsc --noEmit`

- [x] T013 Run `bash scripts/run-playwright-training-suites.sh --mode mock` — exits 0, both inbox files populated with correctly-shaped entries.
  Commands: `bash scripts/run-playwright-training-suites.sh --mode mock`
