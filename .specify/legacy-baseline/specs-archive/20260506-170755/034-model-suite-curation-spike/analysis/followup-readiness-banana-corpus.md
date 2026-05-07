# Follow-up Readiness -- Banana Corpus + Trainer Slice

**Trigger**: SPIKE 034 commit lands.

## Goal

Add `data/banana/corpus.json` + `scripts/train-banana-model.py`
mirroring the not-banana shape so the orchestrator has both
positive and negative class models.

## Functional requirements

- **FR-banana-01**: `data/banana/corpus.json` conforms to the
  SPIKE 034 corpus schema (`schema_version=1`, `samples[]` with
  `id`/`label`/`text`); minimum 12 banana-positive + 6
  not-banana-negative samples (balanced eval).
- **FR-banana-02**: `scripts/train-banana-model.py` mirrors the
  not-banana CLI surface (`--corpus`, `--output`,
  `--training-profile`, `--session-mode`, `--max-sessions`,
  quality gates).
- **FR-banana-03**: Trainer emits an artifact loadable by the API
  via the same shape as the not-banana model.
- **FR-banana-04**: F5 entry `Train: banana (ci profile)` lands in
  `.vscode/launch.json` (per SPIKE 033 follow-up).
- **FR-banana-05**: `python scripts/validate-ai-contracts.py`
  passes (no perturbation of CI workflow markers).

## Out of scope

- Wiring the banana model into the API pipeline (separate slice).
- Manage-image script (`scripts/manage-banana-image.py`) lands in
  a follow-up after the first artifact is produced.

## Validation lane

```
bash scripts/validate-spec-tasks-parity.sh --all
python scripts/validate-ai-contracts.py
python scripts/train-banana-model.py --corpus data/banana/corpus.json --output artifacts/training/banana/local --training-profile ci --session-mode single --max-sessions 1
```

## Sizing

<=20 tasks. Hard prerequisite: SPIKE 034.
