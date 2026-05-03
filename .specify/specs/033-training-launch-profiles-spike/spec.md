## Problem Statement

## In Scope

- Feature implementation and integration
- Testing and validation of new behavior
- Documentation of feature usage and design decisions

Feature Specification: Training Launch Profiles SPIKE aims to improve system capability and user experience by implementing the feature described in the specification.

# Feature Specification: Training Launch Profiles SPIKE

**Feature Branch**: `[033-training-launch-profiles-spike]`
**Created**: 2026-04-26
**Status**: Draft -- ready to investigate
**Type**: SPIKE (investigate -> plan a small family of bounded follow-ups)
**Triggered by**: Trainers (`scripts/train-not-banana-model.py` and
the autonomous self-training cycle workflow) are CLI-only today.
Contributors who want to iterate on a model locally have to
remember `--training-profile`, `--session-mode`, `--max-sessions`,
plus any future per-model flags. A predictable F5 launch family
would close the inner-loop gap mirrored by SPIKE 032 for the
frontends.

## Problem

Today the training story is split:

- `scripts/train-not-banana-model.py` -- local + CI trainer with
  `ci`, `local`, `overnight` profiles.
- `.github/workflows/train-not-banana-model.yml` -- CI invocation.
- `.github/workflows/orchestrate-autonomous-self-training-cycle.yml`
  -- scheduled self-training driver.

There is no host-side F5 entry to invoke a single profile, no way
to step a trainer in `debugpy`, and no convention for adding new
trainers (banana-classifier, ripeness, escalation explainer) when
they appear. The forthcoming chatbot orchestration (see SPIKE 034)
will grow the trainer surface; standardizing the launch contract
now keeps that growth cheap.

## Investigation goals

- **G1**: Inventory current trainers + their CLI surface. Output:
  trainer-surface matrix (script x profile x flags x outputs).
- **G2**: Decide the F5 contract:
  - One configuration per trainer x profile, or
  - One configuration per trainer with a VS Code input variable
    for the profile.
- **G3**: Decide the debugger contract: `debugpy` + Python
  interpreter from `.venv/Scripts/python` on Windows /
  `.venv/bin/python` on Ubuntu WSL2.
- **G4**: Decide the artifacts contract: where trainer output
  lands (today: `data/<model>/`, model image via
  `scripts/manage-not-banana-model-image.py`) and whether F5
  launches should write to a separate `artifacts/training/<run>/`
  to avoid clobbering committed seed models.

## Concrete deliverable in this SPIKE

Initial entries `Train: not-banana (ci profile)` and
`Train: not-banana (local profile)` are stubbed in the slice 032
`launch.json` under the `training` presentation group so that the
F5 surface exists today. SPIKE 033 expands the surface (debugpy,
input-variable profile selection, additional trainers) and writes
the canonical contract doc.

## Out of Scope

- Implementing new trainers (banana-classifier, ripeness, etc.) --
  those are slice work, scheduled after SPIKE 034 picks the model
  suite.
- Distributed / multi-GPU launch (defer with explicit trigger when
  a trainer outgrows a single host).
- Hyperparameter sweeps (defer; a trainer-specific concern).
- Modifying the CI workflow contract (markers
  `--training-profile ci --session-mode single --max-sessions 1`
  must remain literal -- enforced by
  `scripts/validate-ai-contracts.py`).

## Deliverables

- `analysis/trainer-surface-matrix.md` -- G1 output.
- `analysis/launch-contract-decision.md` -- G2 + G3 decisions
  (per-profile entries vs input-variable; debugpy vs node-style).
- `analysis/artifacts-contract.md` -- G4 decision (where local F5
  trainer runs write outputs).
- `analysis/followup-readiness.md` -- bounded scope for the slice
  that lands the expanded F5 entries + any new trainers from
  SPIKE 034.

## Success Criteria

- F5 entry runs `train-not-banana-model.py --training-profile ci
  --session-mode single --max-sessions 1` end-to-end on a fresh
  `.venv` and produces a committed-shape model image.
- Debugger entry hits a breakpoint inside the trainer.
- AI-contract validator (`scripts/validate-ai-contracts.py`) still
  passes -- the CI workflow markers are not perturbed.
- Spec/tasks parity passes.

## Validation lane

```
bash scripts/validate-spec-tasks-parity.sh --all
python scripts/validate-ai-contracts.py
# Manual: F5 -> "Train: not-banana (ci profile)" -> green run.
```

## Downstream

- Slice that adds debugpy + input-variable profile selection +
  F5 entries for any new trainers from SPIKE 034.

## In-scope files

- `.specify/specs/033-training-launch-profiles-spike/**` (NEW).
- `.vscode/launch.json` (READ -- the SPIKE 032 stubs are the
  starting point; expansion lands in the follow-up slice).
- `scripts/train-not-banana-model.py`,
  `.github/workflows/train-not-banana-model.yml`,
  `.github/workflows/orchestrate-autonomous-self-training-cycle.yml`
  (READ ONLY -- audit input).
- `.specify/feature.json` (repoint during execution).
