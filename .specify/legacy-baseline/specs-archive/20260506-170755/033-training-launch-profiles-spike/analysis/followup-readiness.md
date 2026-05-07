# Follow-up Readiness -- Training Launch Surface Expansion

**Trigger**: SPIKE 034 commits to a model suite (banana,
not-banana, ripeness) and the corresponding trainer scripts
land.

## Scope (when triggered)

- Add per-profile F5 entries for each new trainer
  (`Train: banana (ci profile)`, `Train: ripeness (ci profile)`,
  ... mirror the not-banana set).
- Standardize `--output ${workspaceFolder}/artifacts/training/<model>/<run>/`
  arguments per the artifacts contract.
- Add `tasks.json` entries that mirror the F5 set so the same
  invocations are reachable from the command palette without the
  debugger.
- Document the launch surface in
  `.github/instructions/api.instructions.md` (or a new
  `training.instructions.md` if the surface grows).

## Out of scope (still deferred)

- Distributed / multi-GPU launch.
- Hyperparameter sweeps.
- Editing CI workflows (markers must remain literal -- enforced
  by `scripts/validate-ai-contracts.py`).

## Prerequisites

- SPIKE 034 picks the suite + corpus schema.
- The trainers for banana + ripeness exist (their slices land
  before this follow-up).
