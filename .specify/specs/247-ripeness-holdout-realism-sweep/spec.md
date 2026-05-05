# Feature Specification: Ripeness Holdout Realism Sweep

**Feature Branch**: `[247-ripeness-holdout-realism-sweep]`
**Created**: 2026-05-05
**Status**: Draft -- ready for planning
**Type**: SLICE (quality hardening)
**Source**: Follow-up from suspiciously perfect ripeness metrics (0 fp/fn at holdout_size 4)

## Goal

Make ripeness evaluation resistant to trivial perfect scores by enforcing a larger, balanced, boundary-rich eval split.

## In Scope

- Expand ripeness eval split policy from minimal canonical examples to balanced boundary coverage.
- Add deterministic split governance so eval composition is reviewable and auditable in repo.
- Add validation that blocks regressions to tiny/easy eval splits.

## Functional Requirements

- **FR-247-01**: `data/ripeness/corpus.json` MUST define `splits.eval_ids` with at least 12 samples and at least 3 per label (`unripe`, `ripe`, `overripe`, `spoiled`).
- **FR-247-02**: At least half of eval IDs MUST be boundary-oriented examples (adjacent-class ambiguity, e.g., unripe-vs-ripe, ripe-vs-overripe).
- **FR-247-03**: Introduce a validator script that fails when eval split violates size, label balance, or boundary-ratio rules.
- **FR-247-04**: Ripeness trainer output metadata MUST include eval composition summary (`holdout_size`, per-label counts, boundary count).
- **FR-247-05**: Validation and training commands in docs/tasks MUST run from workspace-relative paths only.

## Out of Scope

- Changing model family or scoring algorithm.
- Image-based ripeness inference.
- API/UI integration changes.

## Success Criteria

- `python scripts/train-ripeness-model.py --corpus data/ripeness/corpus.json --output artifacts/training/ripeness/local --training-profile ci --session-mode single --max-sessions 1` exits 0 with `holdout_size >= 12`.
- New split validator exits 0 for compliant corpus and non-zero for intentionally small/imbalanced fixture.
- Ripeness metrics are no longer trivially perfect on tiny holdout; confusion matrix demonstrates non-trivial coverage or explicitly documented boundary misses.
- `python scripts/validate-ai-contracts.py` passes.
- `bash .specify/scripts/bash/validate-spec-boundaries.sh --spec .specify/specs/247-ripeness-holdout-realism-sweep/spec.md` passes.

## Validation Lane

```bash
python scripts/train-ripeness-model.py --corpus data/ripeness/corpus.json --output artifacts/training/ripeness/local --training-profile ci --session-mode single --max-sessions 1
python scripts/check-ripeness-eval-split.py --corpus data/ripeness/corpus.json
python scripts/validate-ai-contracts.py
bash .specify/scripts/bash/validate-spec-boundaries.sh --spec .specify/specs/247-ripeness-holdout-realism-sweep/spec.md
```

## In-scope Files

- `data/ripeness/corpus.json`
- `scripts/train-ripeness-model.py`
- `scripts/check-ripeness-eval-split.py` (NEW)
- `.specify/specs/247-ripeness-holdout-realism-sweep/**`

## Assumptions

- Ripeness labels remain exactly `unripe`, `ripe`, `overripe`, `spoiled`.
- Existing deterministic seed contract in trainer stays unchanged.
- Boundary metadata can be represented via existing `source` field patterns.
