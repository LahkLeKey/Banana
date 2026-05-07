# Implementation Plan: 249-training-metrics-suspicion-guard

**Branch**: `249-training-metrics-suspicion-guard` | **Date**: 2026-05-05 | **Spec**: `.specify/specs/249-training-metrics-suspicion-guard/spec.md`
**Input**: Feature specification from `.specify/specs/249-training-metrics-suspicion-guard/spec.md`

## Summary

Build an automated checker that blocks suspicious perfect metrics when evaluation evidence is too small or too trivial, then integrate it into validation lanes.

## Technical Context

**Language/Version**: Python 3.11
**Primary Dependencies**: stdlib JSON/argparse/pathlib, existing training artifact schema
**Storage**: `artifacts/training/*/local/metrics.json`
**Testing**: fixture-based script tests + workflow dry-run validation
**Target Platform**: Local + CI
**Project Type**: Validation/quality-gate automation
**Performance Goals**: full-lane check < 2s for three lanes
**Constraints**: zero false positives for non-perfect but healthy results; deterministic diagnostics
**Scale/Scope**: training lanes (`banana`, `not-banana`, `ripeness`)

## Constitution Check

- Keep checker read-only against artifacts.
- Keep diagnostics actionable and lane-specific.
- Preserve existing validator contracts and command styles.

## Project Structure

```text
.specify/specs/249-training-metrics-suspicion-guard/
├── spec.md
├── plan.md
└── tasks.md

scripts/
├── check-training-metrics-suspicion.py
└── validate-ai-contracts.py

.github/workflows/
└── [affected validation workflows]
```

## Complexity Tracking

| Violation | Why Needed | Simpler Alternative Rejected Because |
|-----------|------------|-------------------------------------|
| None expected | N/A | N/A |
