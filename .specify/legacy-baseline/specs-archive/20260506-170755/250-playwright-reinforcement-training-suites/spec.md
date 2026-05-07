# Feature Specification: Playwright Reinforcement Training Suites (250)

**Feature Branch**: `250-playwright-reinforcement-training-suites`
**Created**: 2026-05-05
**Status**: In Progress

## Problem Statement

Training feedback today is applied manually via JSON files (`data/not-banana/feedback/inbox.json`) and a shell orchestrator. There is no automated pipeline that exercises the live UI like a human would — navigating to `/classify`, submitting real text prompts, observing the model's verdict, and writing the result back into the corpus as a labeled training signal.

The result is a slow, human-gated feedback loop that cannot run at scale. When a model regresses or a new edge case surfaces in production, there is no way to automatically generate corrective samples and feed them back into training without manual intervention.

## Root Cause

- No Playwright suite currently submits classify prompts and captures model verdicts.
- No bridge exists between a Playwright test run and the feedback inbox / corpus pipeline.
- Ripeness, banana, and not-banana models all need independent suites covering their critical edge cases.
- The feedback loop (`apply-not-banana-feedback.py` + `orchestrate-not-banana-feedback-loop.sh`) already works end-to-end — it just has no automated stimulus writer.

## In Scope

- E1: Playwright suite for **banana / not-banana** classifier — submits labeled prompts, captures API verdicts, writes disagreements as feedback entries.
- E2: Playwright suite for **ripeness** classifier — submits labeled prompts, captures ripeness verdict, writes misses as feedback entries.
- E3: A shared `training-feedback.ts` Playwright helper that writes structured feedback entries to `data/<model>/feedback/inbox.json` in the format expected by `apply-not-banana-feedback.py`.
- E4: A `scripts/run-playwright-training-suites.sh` orchestrator that runs the suites, then optionally triggers the existing feedback-apply pipeline.
- E5: CI workflow step (or standalone workflow) that runs the suites against a local dev server on push to main and surfaces disagreement counts.
- E6: Corpus-parity guard: if a suite session produces more than N new disagreements, it emits a warning and exits non-zero so CI flags it.

## Out of Scope

- Full autonomous corpus rewriting without human approval — all generated entries land in `inbox.json` with `status: "pending"` for human review before they are applied.
- Model weight hot-swapping during a test run.
- New model architectures.
- Mobile / React Native UI automation.

## Success Criteria

- SC-001: Running `bash scripts/run-playwright-training-suites.sh` against a running dev server produces a populated `inbox.json` with at least one entry per model.
- SC-002: Each entry in `inbox.json` has the correct schema (`id`, `label`, `text`, `status: "pending"`, `source: "playwright-reinforcement"`, `reviewed_by: []`).
- SC-003: The banana suite submits at least 10 labeled prompts and records the model's verdict alongside the expected label.
- SC-004: The ripeness suite submits at least 6 labeled prompts (one per label class per boundary category) and records verdict vs expected.
- SC-005: Disagreements (predicted ≠ expected) are flagged in test output and written to inbox as feedback candidates.
- SC-006: Agreements (predicted = expected) are written as positive reinforcement entries (status: "pending-positive").
- SC-007: CI step exits 0 when disagreement rate is below threshold (default 40%), non-zero when above.
- SC-008: `bun run --cwd src/typescript/react tsc --noEmit` exits 0 after all changes.

## Assumptions

- The live classify UI exposes `data-testid` attributes (`ensemble-form`, `ensemble-verdict-copy`, ripeness panel) sufficient for Playwright selectors — already confirmed by existing specs.
- The feedback inbox schema (`id`, `label`, `payload.text`, `status`, `source`, `reviewed_by`, `reviewed_at_utc`) is stable — defined by `apply-not-banana-feedback.py`.
- Ripeness predictions are surfaced by the API at a routable endpoint and readable from the page DOM or a direct API call from the Playwright context.
- The suites run against `http://localhost:5173` (or `PLAYWRIGHT_BASE_URL`) with the API mocked or live depending on the run mode.
- No new npm dependencies beyond `@playwright/test` (already installed).
