# Hypersphere K-Means Evidence Index

This directory tracks deterministic and rollout evidence artifacts for feature 035.

## Determinism Captures

- Native deterministic run logs (repeat-run payload hash comparisons)
- Stable ordering assertions for centers/radii/scores/spectral proxies
- ABI envelope conformance snapshots

## Rollout Captures

- `BANANA_NETCODE_HYPERSPHERE_KMEANS_ENABLED=true` validation logs
- Cohort marker captures for `BANANA_NETCODE_HYPERSPHERE_KMEANS_COHORT`
- Rollback drill evidence with rollout-disabled API response contracts

## Naming Guidance

Use timestamped folders:

- `determinism/YYYYMMDD-HHMMSS/`
- `rollout/YYYYMMDD-HHMMSS/`

Include command transcripts and test outputs in each capture.
