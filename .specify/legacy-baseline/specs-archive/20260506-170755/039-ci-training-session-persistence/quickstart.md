# Quickstart: CI Training Session Persistence

## 1. Validate contracts

- python scripts/validate-ai-contracts.py
- bash scripts/validate-spec-tasks-parity.sh --all

## 2. Run deterministic training per lane

- python scripts/train-banana-model.py --corpus data/banana/corpus.json --output artifacts/training/banana/local --training-profile ci --session-mode single --max-sessions 1
- python scripts/train-not-banana-model.py --corpus data/not-banana/corpus.json --output artifacts/training/not-banana/local --training-profile ci --session-mode single --max-sessions 1
- python scripts/train-ripeness-model.py --corpus data/ripeness/corpus.json --output artifacts/training/ripeness/local --training-profile ci --session-mode single --max-sessions 1

## 3. Verify persisted session outputs

- Confirm new append-only entries under data/<lane>/sessions/.
- Confirm records include run_fingerprint, thresholds, metrics, and relative artifact paths.

## 4. CI verification

- Push branch and confirm training workflow validates persisted session schema before downstream steps.

## 5. Local replay and drift checks

- Re-run a lane trainer command and inspect `history_drift` in JSON stdout.
- Confirm history file growth with:

```bash
wc -l data/not-banana/sessions/history.jsonl
tail -n 2 data/not-banana/sessions/history.jsonl
```

- Validate persisted session records explicitly:

```bash
python scripts/validate-training-session-schema.py
```
