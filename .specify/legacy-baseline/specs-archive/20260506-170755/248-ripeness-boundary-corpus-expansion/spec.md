# Feature Specification: Ripeness Boundary Corpus Expansion

**Feature Branch**: `[248-ripeness-boundary-corpus-expansion]`
**Created**: 2026-05-05
**Status**: Draft -- ready for planning
**Type**: SLICE (data quality expansion)
**Hard prerequisites**: 247-ripeness-holdout-realism-sweep baseline rules in place
**Source**: Follow-up from overripe-vs-ripe boundary miss during ripeness hardening

## Goal

Widen ripeness corpus boundary coverage so adjacent ripeness classes are distinguishable under realistic wording variation.

## In Scope

- Add balanced, high-ambiguity ripeness examples across adjacent classes.
- Add explicit challenge set for boundary probes reused across training sessions.
- Improve class recall parity for `ripe` and `overripe` without reintroducing trivial eval behavior.

## Functional Requirements

- **FR-248-01**: Increase corpus floor to at least 12 samples per ripeness label.
- **FR-248-02**: Add minimum 6 boundary-pair contrasts for `unripe<->ripe` and 6 for `ripe<->overripe`.
- **FR-248-03**: Add `data/ripeness/challenge-set.json` with at least 24 labeled prompts covering lexical overlap (sweet, yellow, soft, brown, sour, mold).
- **FR-248-04**: Add a scorer utility that evaluates challenge-set accuracy and per-label recall.
- **FR-248-05**: Trainer output MUST include challenge-set summary alongside holdout metrics.

## Out of Scope

- Ensemble/API integration.
- New labels beyond `unripe`, `ripe`, `overripe`, `spoiled`.
- Vision model changes.

## Success Criteria

- Holdout macro-F1 >= configured minimum and per-label recall >= 0.75.
- Challenge-set overall accuracy >= 0.80 and `overripe` recall >= 0.75.
- At least one prior false-boundary pattern (overripe phrasing predicted as ripe) is corrected in challenge-set results.
- `python scripts/validate-ai-contracts.py` passes.

## Validation Lane

```bash
python scripts/train-ripeness-model.py --corpus data/ripeness/corpus.json --output artifacts/training/ripeness/local --training-profile ci --session-mode single --max-sessions 1
python scripts/score-ripeness-challenge-set.py --model artifacts/training/ripeness/local --challenge data/ripeness/challenge-set.json
python scripts/validate-ai-contracts.py
bash .specify/scripts/bash/validate-spec-boundaries.sh --spec .specify/specs/248-ripeness-boundary-corpus-expansion/spec.md
```

## In-scope Files

- `data/ripeness/corpus.json`
- `data/ripeness/challenge-set.json` (NEW)
- `scripts/score-ripeness-challenge-set.py` (NEW)
- `scripts/train-ripeness-model.py`
- `.specify/specs/248-ripeness-boundary-corpus-expansion/**`

## Assumptions

- The ripeness lane remains text-only in this slice.
- Existing artifact schema can be extended without breaking downstream loaders.
- Challenge set stays repo-local JSON and is versioned with corpus updates.
