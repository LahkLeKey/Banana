# Determinism Refresh Notes

## Determinism changes in trainer

- Replaced runtime timestamps with fixed `generated_at_utc = 1970-01-01T00:00:00Z`.
- Added `run_fingerprint` generated from normalized run inputs.
- Added deterministic tie-breaker in session selection (`-session_index`).
- Preserved seeded split/training behavior and sorted vocabulary emission.

## Classification safety adjustment

- Changed neutral score behavior to classify as `not-banana` when score is exactly zero (`score > 0` required for `banana`) to reduce sparse-token false positives.

## Evidence commands

```bash
python scripts/train-not-banana-model.py --corpus data/not-banana/corpus.json --output artifacts/training/not-banana/local-a --training-profile ci --session-mode single --max-sessions 1
python scripts/train-not-banana-model.py --corpus data/not-banana/corpus.json --output artifacts/training/not-banana/local-b --training-profile ci --session-mode single --max-sessions 1
find artifacts/training/not-banana/local-a -type f -print0 | sort -z | xargs -0 sha256sum | awk '{print $1}' > artifacts/training/not-banana/local-a.hashes
find artifacts/training/not-banana/local-b -type f -print0 | sort -z | xargs -0 sha256sum | awk '{print $1}' > artifacts/training/not-banana/local-b.hashes
diff -u artifacts/training/not-banana/local-a.hashes artifacts/training/not-banana/local-b.hashes
```
