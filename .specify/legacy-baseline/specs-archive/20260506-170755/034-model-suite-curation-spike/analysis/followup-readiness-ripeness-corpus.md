# Follow-up Readiness -- Ripeness Corpus + Trainer Slice

**Trigger**: Banana corpus slice lands cleanly (so the trainer
helper module, if extracted, exists).

## Goal

Add `data/ripeness/corpus.json` + `scripts/train-ripeness-model.py`
as the first multi-class trainer in the suite.

## Functional requirements

- **FR-ripe-01**: `data/ripeness/corpus.json` conforms to the
  SPIKE 034 corpus schema; labels are exactly
  `unripe` / `ripe` / `overripe` / `spoiled`; minimum 8 samples
  per label; `splits` block ensures eval covers all four labels.
- **FR-ripe-02**: `scripts/train-ripeness-model.py` mirrors the
  CLI surface of the banana / not-banana trainers; gates use
  multi-class macro-F1.
- **FR-ripe-03**: Trainer emits an artifact loadable by the API.
- **FR-ripe-04**: F5 entry `Train: ripeness (ci profile)` lands
  in `.vscode/launch.json`.
- **FR-ripe-05**: `python scripts/validate-ai-contracts.py`
  passes.

## Out of scope

- Wiring the ripeness model into the API pipeline (separate
  slice).
- Manage-image script lands in a follow-up.
- Image-based ripeness inference (text-only for now to stay
  consistent with the existing trainer pattern).

## Validation lane

```
bash scripts/validate-spec-tasks-parity.sh --all
python scripts/validate-ai-contracts.py
python scripts/train-ripeness-model.py --corpus data/ripeness/corpus.json --output artifacts/training/ripeness/local --training-profile ci --session-mode single --max-sessions 1
```

## Sizing

<=20 tasks. Hard prerequisites: SPIKE 034 + banana corpus slice.
