# Determinism Notes

The banana trainer removes or normalizes non-deterministic inputs so two runs with the same corpus and CLI flags produce byte-identical artifacts.

## Sources removed

- Runtime timestamp drift:
  - Replaced dynamic timestamps with fixed `generated_at_utc = 1970-01-01T00:00:00Z` in all emitted JSON artifacts.
- Unstable session selection ties:
  - Added deterministic tie-breaker on `session_index` when F1, signal score, and accuracy are equal.
- Run metadata drift:
  - Added `run_fingerprint` derived only from normalized CLI arguments + corpus path + sample count.

## Sources already deterministic

- Train/holdout split:
  - Uses fixed seed base `20260426 + session_index`.
- Vocabulary order:
  - Sorted by absolute weight desc, support desc, token asc.
- JSON key order:
  - Python insertion-order dicts are written consistently from fixed construction order.

## Verification command

```bash
python scripts/train-banana-model.py --corpus data/banana/corpus.json --output artifacts/training/banana/run-a --training-profile ci --session-mode single --max-sessions 1
python scripts/train-banana-model.py --corpus data/banana/corpus.json --output artifacts/training/banana/run-b --training-profile ci --session-mode single --max-sessions 1
```
