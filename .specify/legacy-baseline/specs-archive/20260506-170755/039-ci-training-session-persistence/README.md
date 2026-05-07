# 039 CI Training Session Persistence

Status: COMPLETE

## Summary

Persists deterministic CI training-session history into repository data folders so banana, not-banana, and ripeness training can be replayed across environments.

## Execution Tracker

- [x] Added implementation planning artifacts (`plan.md`, `research.md`, `data-model.md`, `contracts/`, `quickstart.md`, `tasks.md`).
- [x] Added persisted session contract draft in `contracts/training-session-contract.md`.
- [x] Added shared serializer/session writer behavior in all three trainer scripts.
- [x] Added persisted-session schema validator utility in `scripts/validate-training-session-schema.py`.
- [x] Added CI workflow validation steps for persisted session schema in `.github/workflows/train-not-banana-model.yml`.
- [x] Persisted append-only session records per lane under `data/<lane>/sessions/`.
- [x] Added replay/drift diagnostics in trainer outputs with local replay quickstart instructions.
- [x] Completed close-out validation (`validate-ai-contracts`, `validate-spec-tasks-parity --all`, and all 3 ci-profile trainer runs + session schema validation).

## Baseline validation lane

```bash
python scripts/validate-ai-contracts.py
bash scripts/validate-spec-tasks-parity.sh --all
python scripts/train-banana-model.py --corpus data/banana/corpus.json --output artifacts/training/banana/local --training-profile ci --session-mode single --max-sessions 1
python scripts/train-not-banana-model.py --corpus data/not-banana/corpus.json --output artifacts/training/not-banana/local --training-profile ci --session-mode single --max-sessions 1
python scripts/train-ripeness-model.py --corpus data/ripeness/corpus.json --output artifacts/training/ripeness/local --training-profile ci --session-mode single --max-sessions 1
```
