# Implementation Plan: 247-ripeness-holdout-realism-sweep

**Branch**: `247-ripeness-holdout-realism-sweep` | **Date**: 2026-05-05 | **Spec**: `.specify/specs/247-ripeness-holdout-realism-sweep/spec.md`
**Input**: Feature specification from `.specify/specs/247-ripeness-holdout-realism-sweep/spec.md`

## Summary

Harden ripeness evaluation by replacing tiny canonical holdouts with deterministic, balanced, and boundary-heavy eval IDs plus automated split validation.

## Technical Context

**Language/Version**: Python 3.11
**Primary Dependencies**: stdlib (`json`, `argparse`, `pathlib`), existing ripeness trainer helpers
**Storage**: `data/ripeness/corpus.json`, `artifacts/training/ripeness/local/metrics.json`
**Testing**: CLI validation scripts + deterministic training rerun
**Target Platform**: Windows + Git Bash local workflow and CI Linux runners
**Project Type**: Data/training quality hardening slice
**Performance Goals**: Validation script < 1s on current corpus size
**Constraints**: Deterministic outputs; workspace-relative paths; no model-architecture changes
**Scale/Scope**: Ripeness lane only

## Constitution Check

- Preserve deterministic training contract.
- Preserve Spec Kit validation command style (`--spec`, workspace-relative paths).
- Keep changes scoped to ripeness/training scripts and spec docs.

## Project Structure

```text
.specify/specs/247-ripeness-holdout-realism-sweep/
├── spec.md
├── plan.md
└── tasks.md

data/ripeness/
└── corpus.json

scripts/
├── train-ripeness-model.py
└── check-ripeness-eval-split.py
```

## Complexity Tracking

| Violation | Why Needed | Simpler Alternative Rejected Because |
|-----------|------------|-------------------------------------|
| None expected | N/A | N/A |
