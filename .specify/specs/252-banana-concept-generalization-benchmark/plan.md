# Implementation Plan: Banana Concept Generalization Benchmark (252)

## Overview

Create a deterministic evaluation lane focused on context diversity and concept transfer quality.

## Key Files

- `data/banana/generalization-benchmark.json`
- `scripts/score-banana-generalization-benchmark.py`
- `.github/workflows/orchestrate-neuro-git-event-training.yml`

## Steps

1. Define benchmark JSON with concept lane + context family metadata.
2. Implement scoring script with per-lane and per-context outputs.
3. Add regression thresholds with non-zero exit on failure.
4. Publish summary artifact in training workflow.

## Validation

1. Script runs locally and emits deterministic JSON summary.
2. Intentional degraded fixture fails with clear diagnostics.
3. Workflow step surfaces lane/context failures.
