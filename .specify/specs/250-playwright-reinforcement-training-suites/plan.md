# Implementation Plan: Playwright Reinforcement Training Suites (250)

**Branch**: `250-playwright-reinforcement-training-suites` | **Date**: 2026-05-05 | **Spec**: [spec.md](spec.md)

## Overview

Wire Playwright e2e suites directly into the existing feedback + corpus pipeline so that UI-level classify interactions generate labeled training samples automatically, enabling a human-like reinforcement loop at scale.

## Architecture

```
Playwright suite
  └─ submits prompt via /classify UI
  └─ reads verdict from DOM or direct API call
  └─ compares expected vs predicted label
  └─ writes entry → data/<model>/feedback/inbox.json  (status: pending)
       └─ existing apply-not-banana-feedback.py
            └─ existing orchestrate-not-banana-feedback-loop.sh
                 └─ training run → PR → human review → merge
```

## Key Files

| File | Role |
|---|---|
| `tests/e2e/playwright/helpers/training-feedback.ts` | Shared helper — appends feedback entries to inbox.json |
| `tests/e2e/playwright/specs/banana-reinforcement.spec.ts` | Banana / not-banana reinforcement suite |
| `tests/e2e/playwright/specs/ripeness-reinforcement.spec.ts` | Ripeness reinforcement suite |
| `scripts/run-playwright-training-suites.sh` | Orchestrator — runs suites + optional feedback apply |
| `.github/workflows/playwright-reinforcement.yml` | CI workflow (push to main, manual dispatch) |

## Feedback Entry Schema

```json
{
  "id": "pw-banana-<timestamp>-<index>",
  "label": "BANANA" | "NOT_BANANA",
  "payload": { "text": "<prompt text>" },
  "status": "pending",
  "source": "playwright-reinforcement",
  "session_id": "<playwright-run-id>",
  "predicted_label": "<what the model returned>",
  "disagreement": true | false,
  "reviewed_by": [],
  "created_at_utc": "<ISO8601>"
}
```

For ripeness entries, `label` is one of `unripe|ripe|overripe|spoiled` and `predicted_label` is the ripeness verdict.

## Implementation Sequence

### Step 1 — `training-feedback.ts` helper
- `appendFeedbackEntry(inboxPath, entry)`: reads existing inbox array, deduplicates by canonical text, appends entry, writes back atomically.
- `makeBananaEntry(opts)` / `makeRipenessEntry(opts)`: construct properly-shaped entries with generated IDs.
- Export `DISAGREEMENT_THRESHOLD = 0.4` for the corpus-parity guard.

### Step 2 — Banana reinforcement suite
- 10+ prompts: 5 clear banana signals, 5 not-banana signals, plus 4 boundary edge cases from `data/banana/corpus.json` eval_ids.
- For each: navigate to `/classify`, fill `[data-testid='ensemble-form'] textarea`, click submit, wait for verdict, parse `[data-testid='ensemble-verdict-copy']`.
- Write entry (agreement or disagreement) to `data/not-banana/feedback/inbox.json`.
- After all prompts: assert disagreement rate < `DISAGREEMENT_THRESHOLD` (soft fail: log warn; hard fail: exit 1).

### Step 3 — Ripeness reinforcement suite
- 12+ prompts: 3 per label class (unripe/ripe/overripe/spoiled), with at least 1 boundary case per class drawn from `data/ripeness/challenge-set.json`.
- Call ripeness API directly via `page.request.get('/api/ripeness?text=...')` — avoids DOM coupling to ripeness panel selectors which may scroll out of view.
- Write entries to `data/ripeness/feedback/inbox.json` (create if absent).
- After all prompts: assert per-label disagreement rate < threshold.

### Step 4 — `run-playwright-training-suites.sh`
- Accepts `--mode live|mock` (default: mock), `--apply` flag to run feedback apply step after suites.
- In mock mode: stubs API routes so verdicts come from mock-api fixtures (useful for CI smoke).
- In live mode: expects `BANANA_API_BASE_URL` set; calls real API.
- Emits a `artifacts/playwright-training/session-<date>.json` summary with total prompts, agreements, disagreements per model.

### Step 5 — CI workflow
- Name: `playwright-reinforcement.yml`
- Trigger: `push` to `main`, `workflow_dispatch` with `mode` input.
- Jobs: `build-react` → `run-suites` (mock mode by default) → upload `artifacts/playwright-training/` report.
- Disagreement gate: fail job if rate exceeds `DISAGREEMENT_THRESHOLD`.

## Validation

1. `bun run --cwd src/typescript/react tsc --noEmit` — zero errors
2. `bash scripts/run-playwright-training-suites.sh --mode mock` — exits 0, inbox populated
3. Inspect `data/not-banana/feedback/inbox.json` and `data/ripeness/feedback/inbox.json` for properly-shaped entries
4. Run `python scripts/apply-not-banana-feedback.py --preview --feedback data/not-banana/feedback/inbox.json --corpus data/not-banana/corpus.json` — parses entries cleanly (status: pending entries are skipped by default)
