# Feature Specification: Ripeness Corpus + Trainer

**Feature Branch**: `[036-ripeness-corpus-and-trainer]`
**Created**: 2026-04-26
**Status**: Draft -- ready to implement
**Type**: SLICE (implementation)
**Hard prerequisites**: SPIKE 034 + spec 035 (banana corpus +
trainer must land first so the trainer pattern + any extracted
helper module exists).
**Source**: `.specify/specs/034-model-suite-curation-spike/analysis/followup-readiness-ripeness-corpus.md`

## Goal

Add `data/ripeness/corpus.json` and
`scripts/train-ripeness-model.py` -- the first multi-class
trainer in the suite -- so the chatbot orchestrator (downstream)
can answer "how ripe?" alongside "is it a banana?".

## Functional requirements

- **FR-036-01**: `data/ripeness/corpus.json` conforms to the
  SPIKE 034 corpus schema; labels are exactly `unripe`, `ripe`,
  `overripe`, `spoiled`.
- **FR-036-02**: Minimum 8 samples per label; `splits` block
  ensures the eval set covers all four labels.
- **FR-036-03**: `scripts/train-ripeness-model.py` mirrors the
  banana / not-banana trainer CLI surface; quality gates use
  multi-class macro-F1 (`--min-f1` interpreted as macro-F1).
- **FR-036-04**: Trainer is **deterministic** -- same corpus +
  same flags -> bit-identical artifact.
- **FR-036-05**: Trainer emits a model artifact loadable by the
  same downstream loader (with the multi-class label set).
- **FR-036-06**: `scripts/manage-ripeness-image.py` mirrors the
  banana / not-banana manage scripts.

## Out of scope

- Wiring ripeness into the API ensemble pipeline.
- Image-based ripeness inference (text-only for parity).
- Cross-model attention.

## Success criteria

- `python scripts/train-ripeness-model.py --corpus
  data/ripeness/corpus.json --output artifacts/training/ripeness/local
  --training-profile ci --session-mode single --max-sessions 1`
  exits 0 and emits the expected artifact.
- Determinism re-run produces a byte-identical artifact.
- Macro-F1 in the artifact metadata >= the configured
  `--min-f1` gate.
- `python scripts/validate-ai-contracts.py` passes.
- `bash scripts/validate-spec-tasks-parity.sh --all` passes.

## Validation lane

```
bash scripts/validate-spec-tasks-parity.sh --all
python scripts/validate-ai-contracts.py
python scripts/train-ripeness-model.py --corpus data/ripeness/corpus.json --output artifacts/training/ripeness/local --training-profile ci --session-mode single --max-sessions 1
python scripts/train-ripeness-model.py --corpus data/ripeness/corpus.json --output artifacts/training/ripeness/local --training-profile ci --session-mode single --max-sessions 1 # determinism re-run
```

## Downstream

- Spec 037 adds the F5 entry `Train: ripeness (ci profile)`.
- Future orchestrator slice consumes ripeness alongside banana +
  not-banana.

## In-scope files

- `data/ripeness/corpus.json` (NEW).
- `scripts/train-ripeness-model.py` (NEW).
- `scripts/manage-ripeness-image.py` (NEW).
- `data/ripeness/images/` (NEW; `.gitkeep` until first promotion).
- `.specify/specs/036-ripeness-corpus-and-trainer/**` (NEW).
- `.specify/feature.json` (repoint during execution).
