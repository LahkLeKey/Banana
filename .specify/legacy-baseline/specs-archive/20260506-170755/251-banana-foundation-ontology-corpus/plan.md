# Implementation Plan: Banana Foundation Ontology Corpus (251)

## Overview

Create a shared ontology contract for foundational banana knowledge and wire it into corpus validation + training summaries.

## Key Files

- `data/banana/ontology-foundation.json`
- `data/banana/corpus.json`
- `scripts/check-corpus-schema.py`
- `scripts/train-banana-model.py`

## Steps

1. Introduce ontology file with canonical concept IDs.
2. Add `concepts` metadata to banana corpus samples.
3. Extend schema/validator checks for concept IDs and counts.
4. Emit coverage summary in trainer output JSON.

## Validation

1. Schema validator passes with concept-tagged corpus.
2. Coverage gate fails when any lane drops below threshold.
3. Trainer writes deterministic `concept_coverage_summary` block.
