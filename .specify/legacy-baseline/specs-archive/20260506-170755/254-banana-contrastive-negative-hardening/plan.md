# Implementation Plan: Banana Contrastive Negative Hardening (254)

## Overview

Integrate hard negatives and disagreement replays into training/evaluation to improve concept boundaries.

## Key Files

- `data/not-banana/hard-negatives.json`
- `scripts/build-contrastive-replay-pack.py`
- `scripts/train-not-banana-model.py`
- `scripts/score-contrastive-negatives.py`

## Steps

1. Define hard-negative categories and seed set.
2. Build replay pack from reinforcement disagreement entries.
3. Merge replay samples into training flow with provenance tags.
4. Add contrastive precision gate and artifact summary.

## Validation

1. Replay builder emits deterministic pack from inbox input.
2. Contrastive scorer reports per-category precision.
3. Gate blocks run when floor threshold is violated.
