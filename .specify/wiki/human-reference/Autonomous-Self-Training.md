# Autonomous Self-Training Cycle — Neuroscience Layer (feature 050)

> Canonical AI-consumable reference. Companion human page: `.wiki/Autonomous-Self-Training.md`.

## Purpose

Layer biologically-inspired mechanisms onto Banana's existing deterministic
self-training cycle so that models can refresh themselves on relevant git
events and on a sleep-like nightly/weekly schedule, without sacrificing the
bit-for-bit determinism the rest of the pipeline depends on.

## Trigger surfaces

| Surface | Workflow | Cadence |
|---|---|---|
| Git event (PR merged on main, push on main, manual dispatch) | `.github/workflows/orchestrate-neuro-git-event-training.yml` | per event, scoped by changed paths or `train:<model>` labels |
| Nightly sleep consolidation (NREM) | `.github/workflows/orchestrate-neuro-consolidation.yml` | cron `30 6 * * *` |
| Weekly deep consolidation (REM) | `.github/workflows/orchestrate-neuro-consolidation.yml` | cron `30 7 * * 0` |
| Manual | both workflows | `workflow_dispatch` |

Both workflows apply concurrency `neuro-{train,consolidate}-<model>` so a
given model never trains twice in parallel.

## Component contract

| Component | File | Spec ref |
|---|---|---|
| Episodic replay buffer (reservoir-sampled, cap 2048) | `scripts/neuro/replay-buffer.py` | FR-001, FR-011 |
| Surprise sampler (predictive coding, 0.7 uniform floor) | `scripts/neuro/surprise-sampler.py` | FR-003 |
| Reward modulator (dopamine-style scalar [0.25, 4.0]) | `scripts/neuro/reward-modulator.py` | FR-004 |
| Sleep consolidation (EWC Fisher snapshot) | `scripts/neuro/consolidation-pass.py` | FR-002 |
| Forgetting guard (1.0 pp regression bound) | `scripts/neuro/forgetting-guard.py` | SC-002 |
| Event scoping helper | `scripts/neuro/scope-models-from-event.py` | FR-010 |
| Evidence PR opener | `scripts/neuro/open-evidence-pr.sh` | FR-013 |
| Trainer sidecar | `scripts/neuro_trace.py` | FR-005 |

## Opt-in / opt-out

The neuro layer is gated by `--neuro-profile {off,ci,local}` on each trainer:

- `off` (default): trainer behavior is bit-for-bit identical to pre-feature-050.
  No `neuro-trace.json` sidecar is written.
- `ci`: enables the trainer sidecar with conservative window settings (window=3).
- `local`: same sidecar with longer rolling windows (window=5).

Rollback: pass `--neuro-profile off` (or omit the flag) to disable the layer
entirely. No corpora, vocabularies, or session histories are mutated by the
neuro layer in v1 — it is observation-only.

## On-disk layout

```
artifacts/training/<model>/
  local/
    metrics.json              # existing
    vocabulary.json           # existing
    sessions.json             # existing
    neuro-trace.json          # added when --neuro-profile != off
  replay/
    buffer.jsonl              # reservoir-sampled session memory
  consolidation/<utc-date>/
    fisher.json               # diagonal Fisher proxy
    consolidation-report.json # phase + replay buffer size + cold-start flag
  evidence-<trigger>.json     # written by open-evidence-pr.sh
```

## Scoping rules (`scope-models-from-event.py`)

| Event | Resolution |
|---|---|
| `workflow_dispatch` | `model=all` -> all 3, otherwise the named model |
| `push` on main | union of `data/<model>/` + `scripts/train-<model>-model.py` changed paths; shared `scripts/neuro/**` -> all 3 |
| `pull_request_target` (merged) | `train:<model>` PR labels override path scope; `train:all` -> all 3 |
| no scope match | `run=false`; matrix is skipped |

## Forgetting guard

Run as the final step of both new workflows. Compares the freshly emitted
`holdout_accuracy` against `data/<model>/anchor-metrics.json` (when present).
If the regression exceeds `--max-regression-pp 1.0` and `--enforce` is set,
exits non-zero and fails the workflow. When the anchor file is absent the
check downgrades to informational so cold-start environments still succeed.

## Pre-flight

Both new workflows run, before any training step:

- `bash scripts/validate-workflow-dependencies.sh` — every `bash|python|cp <path>`
  reference in orchestrate-*.yml resolves to a real file in the workspace.
- `python scripts/validate-ai-contracts.py` — full AI customization contract
  surface (prompts, agents, instructions, skills, workflows) is intact.

## Mechanism references

- Kirkpatrick et al., 2017 — *Overcoming catastrophic forgetting in neural
  networks* (EWC) — arXiv:1612.00796.
- Zenke, Poole & Ganguli, 2017 — *Continual learning through synaptic
  intelligence* — arXiv:1703.04200.
- Aljundi et al., 2018 — *Memory aware synapses* — arXiv:1711.09601.
- Golden, Tadros & Bazhenov, 2022 — sleep-replay continual-learning literature.
- Rao & Ballard, 1999; Friston, 2010 — predictive coding / free-energy.
- van de Ven, Siegelmann & Tolias, 2020 — *Brain-inspired replay for continual
  learning with artificial neural networks*.

See [`.specify/specs/050-neuro-self-training-enhancements/spike-report.md`](../../specs/050-neuro-self-training-enhancements/spike-report.md)
for the original mechanism survey.
