# 038 Not-Banana Corpus Refresh

Status: COMPLETE

## Summary

Refreshes legacy low-quality not-banana corpus content with curated, diverse examples and makes deterministic not-banana retraining artifacts safe for source-controlled promotion.

## Execution Tracker

- [x] Replaced legacy generated corpus entries with curated samples in `data/not-banana/corpus.json`.
- [x] Added duplicate and near-duplicate text enforcement via `scripts/check-corpus-schema.py`.
- [x] Updated `scripts/train-not-banana-model.py` for deterministic metadata/output and stable run fingerprinting while preserving CLI flags.
- [x] Verified deterministic double-run output equality.
- [x] Validated image compatibility and promoted refreshed candidate image to stable.
- [x] Ran parity and AI-contract validators.

## Local loop

```bash
python scripts/check-corpus-schema.py data/not-banana/corpus.json
python scripts/train-not-banana-model.py --corpus data/not-banana/corpus.json --output artifacts/training/not-banana/local-a --training-profile ci --session-mode single --max-sessions 1
python scripts/train-not-banana-model.py --corpus data/not-banana/corpus.json --output artifacts/training/not-banana/local-b --training-profile ci --session-mode single --max-sessions 1
python scripts/manage-not-banana-model-image.py create --model-dir artifacts/training/not-banana/local-a --registry-dir data/not-banana --channel candidate
python scripts/manage-not-banana-model-image.py promote --registry-dir data/not-banana --from-channel candidate --to-channel stable --allow-unsigned --min-f1 0.7 --max-removed-tokens 999
```

## Rollback

- Point `data/not-banana/channels/stable.json` back to a previous image id.
- Confirm with:

```bash
python scripts/manage-not-banana-model-image.py status --registry-dir data/not-banana --json
```
