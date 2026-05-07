# Implementation Plan: Banana Grade-School Explanation Quality (253)

## Overview

Create an automated rubric to score explanation outputs for factual completeness and readability.

## Key Files

- `data/banana/explanation-rubric.json`
- `data/banana/explanation-prompts.json`
- `scripts/score-banana-explanations.py`
- `.github/workflows/orchestrate-neuro-git-event-training.yml`

## Steps

1. Define rubric with required fact keys.
2. Add prompt set for explanation generation.
3. Implement scorer for fact coverage + readability + length band.
4. Wire strict-mode gate into training workflow.

## Validation

1. Scorer outputs deterministic JSON report.
2. Fixtures show pass and fail for missing-fact scenarios.
3. CI gate fails with actionable fact-level diagnostics.
