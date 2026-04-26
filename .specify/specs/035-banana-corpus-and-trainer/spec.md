# Feature Specification: Banana Corpus + Trainer

**Feature Branch**: `[035-banana-corpus-and-trainer]`
**Created**: 2026-04-26
**Status**: Draft -- ready to implement
**Type**: SLICE (implementation)
**Hard prerequisite**: SPIKE 034 (model suite curation).
**Source**: `.specify/specs/034-model-suite-curation-spike/analysis/followup-readiness-banana-corpus.md`

## Goal

Add `data/banana/corpus.json` and `scripts/train-banana-model.py`
so the chatbot orchestrator (downstream) has a positive-class
banana model artifact built from a deterministic trainer that can
be invoked locally (F5 or CLI) and committed to source control as
a model image.

## Functional requirements

- **FR-035-01**: `data/banana/corpus.json` conforms to the SPIKE
  034 corpus schema (`schema_version=1`, `samples[]` with `id`,
  `label`, `text`, optional `source`).
- **FR-035-02**: Corpus has at minimum 12 banana-positive +
  6 not-banana-negative samples; labels are exactly `banana`
  or `not-banana` (mirrors not-banana corpus shape).
- **FR-035-03**: `scripts/train-banana-model.py` exposes the same
  CLI surface as `train-not-banana-model.py`: `--corpus`,
  `--output`, `--vocab-size`, `--training-profile`,
  `--session-mode`, `--max-sessions`, `--min-signal-score`,
  `--min-f1`, `--min-token-length`, `--allow-numeric-tokens`.
- **FR-035-04**: Trainer is **deterministic** -- same corpus +
  same flags -> bit-identical artifact (seeded RNG; sorted vocab
  emission). Re-running with `--training-profile ci --session-mode
  single --max-sessions 1` twice MUST produce identical files.
- **FR-035-05**: Trainer emits an artifact in the same shape the
  not-banana trainer emits (so downstream loaders can be reused).
- **FR-035-06**: `scripts/manage-banana-image.py` mirrors
  `manage-not-banana-model-image.py` (sha256-named immutable
  image + channel pointer JSON; HMAC signing optional).
- **FR-035-07**: README + analysis docs explain how to run a
  local training session and commit the resulting image to source
  control.

## Out of scope

- Wiring the banana model into the API ensemble pipeline
  (separate slice; preserves API contract until orchestrator
  spec lands).
- Image-based banana classification (text-only for parity with
  the existing not-banana trainer).
- Cross-model evaluation benchmarks.

## Success criteria

- `python scripts/train-banana-model.py --corpus data/banana/corpus.json
  --output artifacts/training/banana/local --training-profile ci
  --session-mode single --max-sessions 1` exits 0 and emits the
  expected artifact.
- Re-running the command produces a byte-identical artifact (FR-04
  determinism).
- `python scripts/manage-banana-image.py promote --image
  artifacts/training/banana/local/<artifact>` produces an
  immutable `data/banana/images/<sha>.tar` + channel pointer.
- `python scripts/validate-ai-contracts.py` passes.
- `bash scripts/validate-spec-tasks-parity.sh --all` passes.

## Validation lane

```
bash scripts/validate-spec-tasks-parity.sh --all
python scripts/validate-ai-contracts.py
python scripts/train-banana-model.py --corpus data/banana/corpus.json --output artifacts/training/banana/local --training-profile ci --session-mode single --max-sessions 1
python scripts/train-banana-model.py --corpus data/banana/corpus.json --output artifacts/training/banana/local --training-profile ci --session-mode single --max-sessions 1 # determinism re-run
```

## Downstream

- Spec 036 (ripeness corpus + trainer) reuses the trainer pattern
  introduced here.
- Spec 037 (training launch surface expansion) adds the F5 entry
  `Train: banana (ci profile)`.
- Future orchestrator slice consumes the banana model alongside
  not-banana per the SPIKE 034 orchestrator boundary.

## In-scope files

- `data/banana/corpus.json` (NEW).
- `scripts/train-banana-model.py` (NEW).
- `scripts/manage-banana-image.py` (NEW).
- `data/banana/images/` (NEW directory; `.gitkeep` until first
  promotion).
- `.specify/specs/035-banana-corpus-and-trainer/**` (NEW).
- `.specify/feature.json` (repoint during execution).
