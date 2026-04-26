# Feature Specification: CI Training Session Persistence

**Feature Branch**: `[039-ci-training-session-persistence]`
**Created**: 2026-04-26
**Status**: Draft -- ready for planning
**Type**: SLICE (implementation)
**Source**: user request to persist CI training sessions to repository data folders.

## Goal

Persist CI training-session outputs into versioned data folders so model training
can run in different environments (CI, local, and containers) while sharing a
single reproducible session history.

## Functional requirements

- **FR-039-01**: CI training flows for banana, not-banana, and ripeness MUST
  emit a normalized training-session record with model id, corpus id,
  thresholds, metrics, run fingerprint, and timestamp.
- **FR-039-02**: A repository data location under `data/` MUST store persisted
  session records so future runs can rehydrate from committed session state.
- **FR-039-03**: Persisted session records MUST be portable across machines and
  must not include host-specific absolute paths.
- **FR-039-04**: Session persistence MUST support append-only behavior; new CI
  sessions must not rewrite prior historical session records.
- **FR-039-05**: Session persistence MUST include schema validation and fail CI
  with actionable diagnostics when required fields are missing.
- **FR-039-06**: Existing training scripts MUST remain runnable in local mode
  without requiring CI-only environment variables.
- **FR-039-07**: The persisted session format MUST be documented with explicit
  fields and replay expectations for deterministic retraining.

## Out of scope

- Model algorithm changes for left/right/full brain internals.
- UI changes for interactive human training control.
- Production model serving rollout policy.

## Success criteria

- CI emits and stores at least one valid persisted session record per model
  lane in `data/` with schema validation passing.
- A local training run can consume persisted session history and produce
  deterministic output parity with CI for the same input corpus and profile.
- Training-session history remains readable after multiple CI runs without
  regression in older entries.
- Contract validators continue to pass for spec-task parity and AI contracts.

## Validation lane

```bash
python scripts/validate-ai-contracts.py
bash scripts/validate-spec-tasks-parity.sh --all
python scripts/train-banana-model.py --corpus data/banana/corpus.json --output artifacts/training/banana/local --training-profile ci --session-mode single --max-sessions 1
python scripts/train-not-banana-model.py --corpus data/not-banana/corpus.json --output artifacts/training/not-banana/local --training-profile ci --session-mode single --max-sessions 1
python scripts/train-ripeness-model.py --corpus data/ripeness/corpus.json --output artifacts/training/ripeness/local --training-profile ci --session-mode single --max-sessions 1
```

## Downstream

- Provides durable provenance for model updates and cross-environment reruns.
- Unblocks human-in-the-loop training UX by giving the UI stable session data.

## In-scope files

- `.github/workflows/train-not-banana-model.yml` (UPDATE)
- Potentially `.github/workflows/train-banana-model.yml` and
  `.github/workflows/train-ripeness-model.yml` (UPDATE)
- `scripts/train-banana-model.py` (UPDATE)
- `scripts/train-not-banana-model.py` (UPDATE)
- `scripts/train-ripeness-model.py` (UPDATE)
- `data/**` session history paths (UPDATE/NEW)
- `.specify/specs/039-ci-training-session-persistence/**` (NEW)
