# Implementation Plan: 248-ripeness-boundary-corpus-expansion

**Branch**: `248-ripeness-boundary-corpus-expansion` | **Date**: 2026-05-05 | **Spec**: `.specify/specs/248-ripeness-boundary-corpus-expansion/spec.md`
**Input**: Feature specification from `.specify/specs/248-ripeness-boundary-corpus-expansion/spec.md`

## Summary

Expand ripeness corpus and add a dedicated boundary challenge set to systematically measure and improve adjacent-class discrimination (especially ripe vs overripe).

## Technical Context

**Language/Version**: Python 3.11
**Primary Dependencies**: stdlib JSON tooling, existing training artifact format
**Storage**: `data/ripeness/*.json`, `artifacts/training/ripeness/local/*`
**Testing**: deterministic training run + challenge scorer CLI + validator scripts
**Target Platform**: Local dev and CI runners
**Project Type**: Corpus + evaluation expansion slice
**Performance Goals**: challenge scorer < 2s on <=100 prompts
**Constraints**: Keep deterministic seed behavior; balanced per-label growth
**Scale/Scope**: Ripeness data and scripts only

## Constitution Check

- Keep scope isolated to ripeness lane.
- Keep training and metrics deterministic.
- Keep spec-task parity and boundary validation lanes green.

## Project Structure

```text
.specify/specs/248-ripeness-boundary-corpus-expansion/
├── spec.md
├── plan.md
└── tasks.md

data/ripeness/
├── corpus.json
└── challenge-set.json

scripts/
├── train-ripeness-model.py
└── score-ripeness-challenge-set.py
```

## Complexity Tracking

| Violation | Why Needed | Simpler Alternative Rejected Because |
|-----------|------------|-------------------------------------|
| None expected | N/A | N/A |
