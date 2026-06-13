# Hypersphere K-Means Evidence Index

This directory tracks deterministic and rollout evidence artifacts for feature 035.

## Determinism Captures

- Native deterministic run logs (repeat-run payload hash comparisons)
- Stable ordering assertions for centers/radii/scores/spectral proxies
- ABI envelope conformance snapshots

## Rollout Captures

- `BANANA_NETCODE_K3H4_ENABLED=true` validation logs
- Cohort marker captures for `BANANA_NETCODE_K3H4_COHORT`
- Rollback drill evidence with rollout-disabled API response contracts

## Naming Guidance

Use timestamped folders:

- `determinism/YYYYMMDD-HHMMSS/`
- `rollout/YYYYMMDD-HHMMSS/`

Include command transcripts and test outputs in each capture.

## Validation Reference Set (Feature 035)

- Native deterministic and ABI conformance:
	- `ctest -C Debug --test-dir out/v3-native -R "netcode|hypersphere|k3h4" --output-on-failure`
	- Save logs under `determinism/<timestamp>/ctest.log`.
- API contract reliability:
	- `bun test src/routes/netcode.contract.test.ts src/routes/netcode.integration.test.ts src/services/nativeNetcode.fail-fast.test.ts`
	- Save logs under `rollout/<timestamp>/api-contract.log`.
- React presentation-only consumer checks:
	- `bun test src/domain/notebook/useNetcodeSession.test.ts src/domain/notebook/network-domain.test.ts`
	- Save logs under `rollout/<timestamp>/react-consumer.log`.
