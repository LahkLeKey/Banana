# 036 Ripeness Corpus + Trainer

Status: COMPLETE

## Summary

Implements deterministic multi-class ripeness training (`unripe`, `ripe`, `overripe`, `spoiled`) and image management for local/CI model iteration.

## Execution Tracker

- [x] Added `data/ripeness/corpus.json` with >=8 samples per label and eval coverage via `splits.eval_ids`.
- [x] Added `scripts/train-ripeness-model.py` with macro-F1 gating and deterministic output metadata.
- [x] Added `scripts/manage-ripeness-image.py` for create/promote/verify/status/snapshot.
- [x] Ran deterministic double-train check and recorded evidence.
- [x] Promoted first ripeness image and committed channel pointers.
- [x] Final parity + AI contract validation.

## Baseline command

```bash
python scripts/train-ripeness-model.py --corpus data/ripeness/corpus.json --output artifacts/training/ripeness/local --training-profile ci --session-mode single --max-sessions 1
```

## Rollback

- Roll channel back by editing `data/ripeness/channels/stable.json` to a previous image id.
- Confirm with:

```bash
python scripts/manage-ripeness-image.py status --registry-dir data/ripeness --json
```
