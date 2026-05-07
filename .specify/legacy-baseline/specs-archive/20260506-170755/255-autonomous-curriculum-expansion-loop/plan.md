# Implementation Plan: Autonomous Curriculum Expansion Loop (255)

## Overview

Add a gap-driven sample proposal subsystem that closes concept coverage deficits automatically while preserving safety and traceability.

## Key Files

- `scripts/mine-banana-knowledge-gaps.py`
- `scripts/generate-banana-curriculum-candidates.py`
- `data/banana/curriculum/staging.json`
- `scripts/validate-training-session-schema.py`
- `scripts/run-playwright-training-suites.sh`

## Steps

1. Mine deficits from ontology coverage + disagreement artifacts.
2. Generate candidate samples with concept/context metadata.
3. Stage candidates in dedicated curriculum file.
4. Add staged-ingest mode to autonomous runner with hard caps.

## Validation

1. Gap miner report is deterministic and ranked.
2. Candidate generator produces schema-valid staging entries.
3. Autonomous runner ingests staged entries when gates pass.
4. Training artifacts include staged-ingest counts and outcome deltas.
