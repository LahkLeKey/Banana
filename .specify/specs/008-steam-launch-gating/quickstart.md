# Quickstart: Steam Launch Gating

## Purpose

Validate launch-gating planning assumptions across native engine launch, API verification contracts, and web remediation flows.

## Prerequisites

- Run from repository root.
- Bun available for API checks.
- Native build configured (`out/v3-native`).
- Access to Steam test identity and a non-linked/suspended test account set.

## Baseline Contract Smoke (API)

```bash
cd src/typescript/api
bun test src/routes/v1/launch-gate.contract.test.ts src/routes/v1/launch-gate.mode-policy.contract.test.ts
```

Expected result: 6 passing tests across launch-gate contract and mode-policy suites.

## Native Launch Gate Validation Matrix

Validate at least these scenarios:

1. `production-steam-package` + linked good-standing account -> allow.
2. `production-steam-package` + unlinked account -> deny `UNLINKED_ACCOUNT`.
3. `production-steam-package` + suspended account -> deny `SUSPENDED_OR_RESTRICTED`.
4. `production-steam-package` + stale assertion/session -> deny `STALE_SESSION`.
5. `production-steam-package` + Steam unavailable -> deny `STEAM_UNAVAILABLE`.
6. `production-steam-package` + API unavailable/offline -> deny `API_UNAVAILABLE` or `OFFLINE_UNVERIFIABLE`.
7. `development` mode without Steam context -> follow documented dev fallback policy and emit audit marker.

Validated command:

```bash
C:/Github/Banana/out/v3-native/engine/Debug/banana_runtime_launch_gate_matrix_test.exe
C:/Github/Banana/out/v3-native/engine/Debug/banana_runtime_launch_gate_staleness_test.exe
C:/Github/Banana/out/v3-native/engine/Debug/banana_runtime_launch_gate_mode_policy_test.exe
```

Expected result: all three native executables print `pass`.

## Web Remediation Validation

1. Trigger launch denial with `UNLINKED_ACCOUNT`.
2. Request remediation ticket and open returned `webFlowUrl`.
3. Complete registration/linking flow.
4. Retry launch and confirm fresh verification decision is required before allow.

Validated command:

```bash
cd src/typescript/api
bun test src/integration/launch-gate-remediation.integration.test.ts
```

Expected result: 2 passing remediation integration tests.

## Failure-Mode Retry Validation

- Confirm retry is user-initiated.
- Confirm backoff guidance is present for transient outages.
- Confirm each retry produces a fresh decision event and updated reason code when conditions change.

Validated command:

```bash
bash scripts/validate-steam-launch-gating.sh
```

Expected result: task counters and setup contract validation print successfully.

## Evidence Capture

Store validation evidence under feature and artifact paths:

- `.specify/specs/008-steam-launch-gating/checklists/`
- `artifacts/native/008-launch-gate-allow-deny-matrix.txt`
- `artifacts/api/008-launch-gate-contract-status.txt`
- `artifacts/api/008-remediation-flow.txt`
- `artifacts/native/008-failure-mode-retry.txt`

Current captured artifacts (2026-05-25):

- `artifacts/native/008-launch-gate-allow-deny-matrix.txt`
- `artifacts/api/008-launch-gate-contract-status.txt`
- `artifacts/api/008-remediation-flow.txt`
- `artifacts/native/008-failure-mode-retry.txt`
