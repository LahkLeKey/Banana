# Trainer and Image Convention (G4)

## Decision: per-model script

**Picked**: `scripts/train-<model>.py` -- one trainer script per
model. Names: `train-not-banana-model.py` (existing),
`train-banana-model.py`, `train-ripeness-model.py`.

Rejected alternative: a single parametrized
`scripts/train-model.py --model <name>`. Reasons:
- Each model has different label sets, gating thresholds, and
  output shapes; lumping them dilutes the CLI surface.
- The CI workflow contract markers
  (`scripts/validate-ai-contracts.py`) reference per-trainer
  filenames; a parametrized entry would force a contract change.
- Per-script files are easier to land in dependency-ordered
  slices.

## Shared helper module

Trainers SHOULD share corpus-loading, profile resolution, and
artifact-emission logic. The follow-up slices may extract
`scripts/_lib/banana_trainer.py` (or similar) without changing
the CLI surface; this is a refactor opportunity, not a SPIKE
deliverable.

## Model image management

**Picked**: per-model `scripts/manage-<model>-image.py` mirroring
`manage-not-banana-model-image.py`. Same reasoning -- the CLI
surface stays small + per-model + easy to grep.

When the third manage-*.py lands, evaluate extracting a shared
helper (`scripts/_lib/model_image.py`); not a SPIKE concern.

## Output paths (consistent with SPIKE 033)

- Local F5: `artifacts/training/<model>/<run-id>/`.
- CI workflow: `artifacts/<workflow-step-output>/<model>/` (already
  conventional for the not-banana workflow; new trainers reuse it).
- Promoted images: `data/<model>/images/<sha>.tar` + channel
  pointer JSON (as not-banana does today).
