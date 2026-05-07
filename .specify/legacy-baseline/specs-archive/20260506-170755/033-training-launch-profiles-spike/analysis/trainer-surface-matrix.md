# Trainer Surface Matrix (G1)

| Trainer       | Script                                       | Profiles                  | Key flags                                                                  | Outputs                                      |
| ------------- | -------------------------------------------- | ------------------------- | -------------------------------------------------------------------------- | -------------------------------------------- |
| not-banana    | `scripts/train-not-banana-model.py`          | `ci`, `local`, `overnight`, `auto` | `--corpus`, `--output`, `--vocab-size`, `--training-profile`, `--session-mode`, `--max-sessions`, `--min-signal-score`, `--min-f1`, `--min-token-length`, `--allow-numeric-tokens` | model artifacts to `--output` dir            |
| (model-image) | `scripts/manage-not-banana-model-image.py`   | -                         | image build / channel pointer commands (sha256 + signed)                   | immutable model image + channel pointer JSON |

## CI / orchestration entry points

| Entry                                                              | Trigger              | Locked markers                                                                       |
| ------------------------------------------------------------------ | -------------------- | ------------------------------------------------------------------------------------ |
| `.github/workflows/train-not-banana-model.yml`                     | `workflow_dispatch`  | profile choices `ci/local/overnight/auto`; modes `incremental/single`                |
| `.github/workflows/orchestrate-autonomous-self-training-cycle.yml` | scheduled + dispatch | literal `--training-profile ci --session-mode single --max-sessions 1` (validated by `scripts/validate-ai-contracts.py`) |

## Observations

- Only one trainer exists today; the matrix is intentionally
  small. SPIKE 034 picks the next models and SPIKE 033's follow-up
  slice will add their entries.
- `--corpus` and `--output` are required positional inputs; F5
  needs reasonable defaults (corpus = `data/<model>/corpus.json`;
  output = `artifacts/training/<model>/<run>/`).
- `--training-profile auto` exists but F5 should pin a profile so
  the contract doc never disagrees with the launch behavior.
