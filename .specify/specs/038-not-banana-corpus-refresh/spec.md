# Feature Specification: Not-Banana Corpus Refresh

**Feature Branch**: `[038-not-banana-corpus-refresh]`
**Created**: 2026-04-26
**Status**: Draft -- ready to implement
**Type**: SLICE (implementation)
**Source**: user request to replace legacy generated not-banana corpus quality.
## Problem Statement

## In Scope

- Feature implementation and integration
- Testing and validation of new behavior
- Documentation of feature usage and design decisions

Feature Specification: Not-Banana Corpus Refresh aims to improve system capability and user experience by implementing the feature described in the specification.


## Goal

Replace low-signal legacy/generated not-banana corpus content with a curated,
traceable corpus and deterministic training outputs so not-banana model
improvements are meaningful, reproducible, and safe to commit.

## Functional requirements

- **FR-038-01**: `data/not-banana/corpus.json` conforms to corpus schema v1
  (`schema_version=1`, `description`, `samples[]` with `id`, `label`, `text`,
  optional `source`).
- **FR-038-02**: Corpus replaces low-quality generated lines with curated,
  human-readable examples and includes both banana positives and hard
  not-banana negatives (minimum: 20 total samples, at least 8 per class).
- **FR-038-03**: Corpus must avoid duplicated/near-duplicated text entries and
  include diverse negative domains (tools, electronics, household, logistics,
  weather, documents).
- **FR-038-04**: `scripts/train-not-banana-model.py` emits deterministic
  artifacts for repeated runs with identical inputs/flags (stable timestamps/
  metadata, seeded behavior, stable ordering).
- **FR-038-05**: Existing CLI contract for
  `scripts/train-not-banana-model.py` remains unchanged.
- **FR-038-06**: `scripts/manage-not-banana-model-image.py` promotion flow
  remains compatible with refreshed training artifacts.
- **FR-038-07**: Add slice docs describing curation rules and the exact local
  rerun/promotion loop for committable improvements.

## Out of scope

- API pipeline wiring changes.
- Cross-model orchestration/routing policy.
- Image-based not-banana detection.

## Success criteria

- `python scripts/check-corpus-schema.py data/not-banana/corpus.json` exits 0.
- `python scripts/train-not-banana-model.py --corpus data/not-banana/corpus.json --output artifacts/training/not-banana/local --training-profile ci --session-mode single --max-sessions 1` exits 0.
- Re-running the same command produces byte-identical model artifacts.
- `python scripts/manage-not-banana-model-image.py create --model-dir artifacts/training/not-banana/local --registry-dir data/not-banana --channel candidate` exits 0.
- `python scripts/validate-ai-contracts.py` passes.
- `bash scripts/validate-spec-tasks-parity.sh --all` passes.

## Validation lane

```bash
bash scripts/validate-spec-tasks-parity.sh --all
python scripts/validate-ai-contracts.py
python scripts/check-corpus-schema.py data/not-banana/corpus.json
python scripts/train-not-banana-model.py --corpus data/not-banana/corpus.json --output artifacts/training/not-banana/local-a --training-profile ci --session-mode single --max-sessions 1
python scripts/train-not-banana-model.py --corpus data/not-banana/corpus.json --output artifacts/training/not-banana/local-b --training-profile ci --session-mode single --max-sessions 1
```

## Downstream

- Reduces training noise before launch-surface expansion and local training sessions.
- Enables deterministic, reviewable not-banana model updates in source control.

## In-scope files

- `data/not-banana/corpus.json` (UPDATE).
- `scripts/train-not-banana-model.py` (UPDATE).
- `.specify/specs/038-not-banana-corpus-refresh/**` (NEW).
- Optional: `data/not-banana/images/*` and channel pointer updates when promoting.
