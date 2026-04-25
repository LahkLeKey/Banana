# Banana E2E Scaffold

This folder provides the initial end-to-end harness scaffold for issue #367.

## Scope

- A dedicated test project: `Banana.E2eTests.csproj`
- Deterministic runner contracts for critical journeys (`CJ001`..`CJ007`)
- Environment-driven run settings contract
- Contract validator that can enforce bootstrap and teardown script presence

## Why this exists

Issue #367 requires an explicit E2E runner API before compose and workflow wiring are added in #368, #369, and #370.

## Current contract defaults

- Compose project name: `banana-container`
- Bootstrap script: `scripts/compose-e2e-bootstrap.sh`
- Teardown script: `scripts/compose-e2e-teardown.sh`
- API base URL: `http://localhost:8080`
- Script existence checks: disabled by default (`BANANA_E2E_REQUIRE_SCRIPT_FILES=false`)

Compose bootstrap and teardown script delivery is complete in #368 and #369.

The smoke workflow path in #370 can now use these scripts directly.
