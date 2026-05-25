# Quickstart: Deterministic Persistent World Orchestration

## Purpose

Validate deterministic area generation, authoritative baseline + delta replay, cross-player consistency, and fault recovery behavior for feature 009.

## Prerequisites

- Run from repository root.
- Native build configured in `out/v3-native`.
- API dependencies installed for `src/typescript/api`.
- Test environment includes deterministic seed fixtures and at least two player personas.

## Baseline API Smoke

```bash
cd src/typescript/api
bun test
```

Expected result: orchestration contract tests (to be implemented in tasks) pass for identity normalization, versioning, replay ordering, and merge determinism.

## Deterministic Validation Commands

Run the following from repository root to generate evidence artifacts deterministically:

```bash
# Native baseline + unexplored parity
./out/v3-native/engine/Debug/banana_runtime_terrain_generation_revisit_parity_test.exe
./out/v3-native/engine/Debug/banana_runtime_terrain_unexplored_parity_test.exe

# Native seam continuity
./out/v3-native/engine/Debug/banana_terrain_chunk_seam_continuity_test.exe

# Native deterministic retry flow
./out/v3-native/engine/Debug/banana_runtime_terrain_retry_determinism_test.exe

# API authoritative replay parity
bun test src/typescript/api/src/routes/world.persistent.contract.test.ts \
	src/typescript/api/src/integration/persistent-world-revisit-parity.integration.test.ts

# API cross-player and conflict/stale merge consistency
bun test src/typescript/api/src/integration/persistent-world-cross-player-parity.integration.test.ts \
	src/typescript/api/src/integration/persistent-world-conflict-parity.integration.test.ts \
	src/typescript/api/src/integration/persistent-world-stale-update.integration.test.ts

# API retry/failure orchestration
bun test src/typescript/api/src/integration/persistent-world-failure-retry.integration.test.ts
```

All commands above must pass before publishing artifacts.

## Deterministic Generation Validation Matrix

Run repeated generation for sampled unexplored areas under equivalent inputs:

1. Same (`world_id`, `lane_id`, `chunk_x`, `chunk_y`, `partition_epoch`) across independent runs.
2. Verify `terrain_signature` parity.
3. Verify `mesh_vector_signature` parity.
4. Verify seam continuity contract at adjacent chunk boundaries.

Pass criterion target: aligns with SC-001.

## Revisit and Cross-Player Consistency Validation

1. Player A generates/explores area and commits no deltas; Player A revisits in a later session and verifies parity.
2. Player A modifies area and commits deltas; Player B arrives later and must receive authoritative post-merge state.
3. Repeat with overlapping modifications submitted in different arrival orders; canonical output must remain identical.

Pass criterion targets: SC-002, SC-003, SC-004.

## Failure-Mode and Deterministic Retry Validation

Inject failures in each class:

1. Recoverable generation timeout.
2. Recoverable persistence timeout/unavailable dependency.
3. Non-recoverable contract mismatch or corrupt delta payload.

For recoverable faults:

- Retry with identical contract fingerprint.
- Verify no duplicate area identity creation.
- Verify canonical output parity with non-faulted run.

For non-recoverable faults:

- Ensure operation is blocked and operator context is emitted.

Pass criterion target: SC-005.

## Evidence Capture

Store evidence in feature-local and shared artifact locations:

- `.specify/specs/009-persistent-world-orchestration/checklists/`
- `artifacts/native/009-deterministic-baseline-parity.txt`
- `artifacts/native/009-chunk-seam-continuity.txt`
- `artifacts/api/009-authoritative-replay-parity.txt`
- `artifacts/api/009-cross-player-merge-consistency.txt`
- `artifacts/api/009-failure-retry-determinism.txt`

Populate evidence files with timestamped command output and explicit exit codes.

## Operational Notes

- Keep `BANANA_PG_CONNECTION` explicit for persistence-backed validation.
- Keep `BANANA_NATIVE_PATH` explicit for API/native integration paths.
- Use equivalent normalized inputs for all parity reruns to avoid false nondeterminism.
