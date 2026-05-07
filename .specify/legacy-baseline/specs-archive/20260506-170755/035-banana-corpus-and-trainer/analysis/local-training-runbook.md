# Local Training Runbook

This runbook captures the minimal local loop for deterministic banana model improvements.

## 1. Validate corpus shape

```bash
python scripts/check-corpus-schema.py data/banana/corpus.json
```

## 2. Train locally (deterministic)

```bash
python scripts/train-banana-model.py --corpus data/banana/corpus.json --output artifacts/training/banana/local --training-profile ci --session-mode single --max-sessions 1
```

## 3. Confirm deterministic re-run

```bash
python scripts/train-banana-model.py --corpus data/banana/corpus.json --output artifacts/training/banana/local-rerun --training-profile ci --session-mode single --max-sessions 1
```

Compare checksums for all emitted files:

```bash
find artifacts/training/banana/local -type f -print0 | sort -z | xargs -0 sha256sum > artifacts/training/banana/local.sha256
find artifacts/training/banana/local-rerun -type f -print0 | sort -z | xargs -0 sha256sum > artifacts/training/banana/local-rerun.sha256
diff -u artifacts/training/banana/local.sha256 artifacts/training/banana/local-rerun.sha256
```

## 4. Create and promote immutable image

```bash
python scripts/manage-banana-image.py create --model-dir artifacts/training/banana/local --registry-dir data/banana --channel candidate
python scripts/manage-banana-image.py promote --registry-dir data/banana --from-channel candidate --to-channel stable --allow-unsigned
```

## 5. Commit model updates

Stage corpus, trainer changes, and the new `data/banana/images/*` + `data/banana/channels/*.json` files, then commit.
