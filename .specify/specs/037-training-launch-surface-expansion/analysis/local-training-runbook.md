# Local Training Runbook

This runbook describes the editor-first local model iteration loop.

## 1. Pick a launch entry (F5)

Use one of:

- `Train: not-banana (ci profile)`
- `Train: banana (ci profile)`
- `Train: ripeness (ci profile)`
- or `Train: all (ci profile)`

Each writes artifacts to:

- `artifacts/training/not-banana/local`
- `artifacts/training/banana/local`
- `artifacts/training/ripeness/local`

## 2. Review output metrics

Check each model's `metrics.json` in its artifact folder.

## 3. Promote a candidate image

Examples:

```bash
python scripts/manage-not-banana-model-image.py create --model-dir artifacts/training/not-banana/local --registry-dir data/not-banana --channel candidate
python scripts/manage-not-banana-model-image.py promote --registry-dir data/not-banana --from-channel candidate --to-channel stable --allow-unsigned --min-f1 0.7 --max-removed-tokens 999
```

```bash
python scripts/manage-banana-image.py create --model-dir artifacts/training/banana/local --registry-dir data/banana --channel candidate
python scripts/manage-banana-image.py promote --registry-dir data/banana --from-channel candidate --to-channel stable --allow-unsigned --min-f1 0.7 --min-signal-score 0.7
```

```bash
python scripts/manage-ripeness-image.py create --model-dir artifacts/training/ripeness/local --registry-dir data/ripeness --channel candidate
python scripts/manage-ripeness-image.py promote --registry-dir data/ripeness --from-channel candidate --to-channel stable --allow-unsigned --min-f1 0.7 --min-signal-score 0.7
```

## 4. Commit deterministic improvements

Commit only the intended model registry changes and related docs/scripts. Keep generated local artifact directories under `artifacts/training/*` out of source control.
