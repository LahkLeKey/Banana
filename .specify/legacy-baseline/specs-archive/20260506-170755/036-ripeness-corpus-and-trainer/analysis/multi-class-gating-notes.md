# Multi-Class Gating Notes

## F1 interpretation

`--min-f1` is interpreted as **macro-F1** across labels:

- `unripe`
- `ripe`
- `overripe`
- `spoiled`

Macro-F1 is the unweighted average of per-label F1 values and is more robust than micro-F1 for catching per-class collapse.

## Threshold rationale

- Baseline threshold: `--min-f1 0.7`
- Signal threshold: `--min-signal-score 0.7`

This matches the existing binary trainer gate defaults and keeps early model promotion disciplined while remaining reachable for seed corpora.

## Determinism notes

Determinism follows the 035 pattern:

- Fixed `generated_at_utc` constant in output artifacts.
- Stable run fingerprint from normalized CLI/corpus inputs.
- Stable token ranking and stable session tie-breaking.
- Stable split behavior:
  - `splits.eval_ids` if present (fully deterministic)
  - otherwise seeded per-label shuffles.
