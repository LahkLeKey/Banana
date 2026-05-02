# scripts/neuro

Neuroscience-inspired helpers for the Banana autonomous self-training cycle (Spec Kit feature 050).

All helpers are **additive and opt-in** behind `--neuro-profile {off,ci,local}` on the trainers
(`scripts/train-{not-banana,banana,ripeness}-model.py`). Default is `off`, which preserves the
existing deterministic outputs bit-for-bit. When enabled, the trainer emits a sidecar
`neuro-trace.json` next to `metrics.json` describing what the neuro layer observed; trained
artifacts themselves remain deterministic until a follow-up wave promotes neuro signals into the
selection loop.

## Components (four-component contract)

| Module | Responsibility | Spec refs |
|---|---|---|
| [`replay-buffer.py`](replay-buffer.py) | Reservoir-sampled episodic memory of past sessions per model. Cap 2048. JSONL on disk, deterministic ordering. | FR-001, FR-011, SC-005 |
| [`surprise-sampler.py`](surprise-sampler.py) | Predictive-coding surprise weighting over the corpus, floored by `0.7 * uniform + 0.3 * surprise`. | FR-003 |
| [`reward-modulator.py`](reward-modulator.py) | Dopamine-style scalar in `[0.25, 4.0]` from rolling validation-accuracy delta. | FR-004 |
| [`consolidation-pass.py`](consolidation-pass.py) | Sleep-style EWC consolidation: writes `fisher.json` + `consolidation-report.json` (cold-start safe). | FR-002 |

Wave B adds the workflow drivers (`open-evidence-pr.sh`, `scope-models-from-event.py`) and the two
new GitHub Actions workflows. Wave C adds `forgetting-guard.py`.

## On-disk layout

```
artifacts/training/<model>/
  local/
    metrics.json              # existing
    vocabulary.json           # existing
    sessions.json             # existing
    neuro-trace.json          # NEW — written when --neuro-profile != off
  replay/
    buffer.jsonl              # NEW — capped reservoir
  consolidation/<utc-date>/
    fisher.json               # NEW
    consolidation-report.json # NEW
```

Paths are workspace-relative and stable across runs (deterministic timestamps where applicable).

## Determinism + safety

- All modules accept an explicit `--seed`; defaults are stable per-model integers.
- No module mutates corpora or existing artifacts.
- Cold-start paths (no buffer, no prior fisher) return the identity scalar / empty report rather
  than failing.
- All emitted JSON sorts keys and uses `\n` line endings to keep diffs reviewable.
