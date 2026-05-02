# Release Notes — 050 Neuroscience-Inspired Self-Training Enhancements

## TL;DR

Adds a biologically-inspired layer (replay, EWC-style consolidation, surprise
weighting, dopamine-style learning-rate modulation, anti-forgetting guard) on
top of Banana's existing autonomous self-training cycle. New git-event and
sleep-schedule workflows retrain only the affected models. Fully **opt-in** —
default behavior is byte-for-byte unchanged.

## What's new

### Trainers

- New flag on all three trainers: `--neuro-profile {off,ci,local}`
  - `scripts/train-banana-model.py`
  - `scripts/train-not-banana-model.py`
  - `scripts/train-ripeness-model.py`
- Default `off` preserves bit-for-bit deterministic outputs.
- When enabled, trainer writes `neuro-trace.json` next to `metrics.json`.

### Helpers (`scripts/neuro/`)

| File | Purpose |
|---|---|
| `replay-buffer.py` | Reservoir-sampled JSONL, cap 2048 |
| `surprise-sampler.py` | Predictive-coding surprise weights, 0.7 uniform floor |
| `reward-modulator.py` | Dopamine-style scalar clamped to `[0.25, 4.0]` |
| `consolidation-pass.py` | Sleep-style EWC Fisher snapshot |
| `forgetting-guard.py` | Anti-forgetting check (1.0 pp regression bound) |
| `scope-models-from-event.py` | Routes git events -> training matrix |
| `open-evidence-pr.sh` | Wraps the triaged-item PR opener for evidence PRs |

### Workflows

- `.github/workflows/orchestrate-neuro-git-event-training.yml` — fires on
  PR-merge to `main`, scoped pushes, and manual dispatch.
- `.github/workflows/orchestrate-neuro-consolidation.yml` — nightly NREM
  cron + weekly REM cron.

### Tests

- 47 new unit tests under `tests/scripts/test_neuro_*.py`.
- Pre-flight (`validate-workflow-dependencies.sh`) and AI-contract
  (`validate-ai-contracts.py`) validators stay green.

## Trigger map

| Trigger | Effect |
|---|---|
| PR merged on main | Retrain models matching `train:<model>` labels or changed paths |
| Push on main (scoped paths) | Retrain models with changed corpora/trainers |
| Cron `30 6 * * *` | NREM consolidation across all 3 models |
| Cron `30 7 * * 0` | REM (deep) consolidation across all 3 models |
| Manual dispatch | Operator selects model + neuro profile |

## Rollback

- Drop `--neuro-profile` (or pass `off`) on the trainers.
- Disable the new workflows in repo settings; the existing
  `train-not-banana-model.yml` and autonomous-self-training cycle continue
  unchanged.
- All neuro outputs live under `artifacts/training/<model>/{replay,consolidation}`
  and `evidence-<trigger>.json`; deleting them has no effect on the
  existing deterministic artifacts.

## Spec & traceability

- Spec: [`.specify/specs/050-neuro-self-training-enhancements/spec.md`](../../.specify/specs/050-neuro-self-training-enhancements/spec.md)
- Plan: [`.specify/specs/050-neuro-self-training-enhancements/plan.md`](../../.specify/specs/050-neuro-self-training-enhancements/plan.md)
- Tasks: [`.specify/specs/050-neuro-self-training-enhancements/tasks.md`](../../.specify/specs/050-neuro-self-training-enhancements/tasks.md)
- QA report: [`.specify/specs/050-neuro-self-training-enhancements/qa-report.md`](../../.specify/specs/050-neuro-self-training-enhancements/qa-report.md)

## Deferred (post-v1)

- STDP / spiking-network experiments
- Generative replay with a learned generator (current v1 uses a real-sample
  reservoir)
- JEPA-style world models
- Promotion of neuro signals from observation-only to selection-loop
  influence (current v1 leaves vocabulary selection deterministic)
