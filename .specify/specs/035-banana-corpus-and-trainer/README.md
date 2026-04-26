# 035 Banana Corpus + Trainer

Status: IN PROGRESS (awaiting global AI-contract cleanup outside this slice)

## Summary

Implements the first banana-positive deterministic text trainer and image-management flow.

## Execution Tracker

- [x] Added `data/banana/corpus.json` with banana/not-banana samples.
- [x] Added `scripts/train-banana-model.py` with deterministic artifact output.
- [x] Added `scripts/manage-banana-image.py` for create/promote/verify/status/snapshot.
- [x] Added schema lint utility: `scripts/check-corpus-schema.py`.
- [x] Ran deterministic double-train check and recorded evidence.
- [x] Promoted first banana image and committed channel pointers.
- [ ] Final parity + AI contract validation (parity pass, AI contract currently fails on pre-existing non-035 issues).

## Local command baseline

```bash
python scripts/train-banana-model.py --corpus data/banana/corpus.json --output artifacts/training/banana/local --training-profile ci --session-mode single --max-sessions 1
```

## Rollback

- Roll channel back by pointing `data/banana/channels/stable.json` to a previous image id.
- Confirm with:

```bash
python scripts/manage-banana-image.py status --registry-dir data/banana --json
```
