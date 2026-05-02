<!-- breadcrumb: Operations > CI and Compose Notes -->

# CI and Compose Notes

> [Home](../Home.md) › [Operations](README.md) › CI and Compose Notes

Related pages: [Build, Run, and Test Commands](../getting-started/build-run-test-commands.md)

Primary workflow:

- `.github/workflows/compose-ci.yml`

## Workflow Jobs

1. `native-c-tests-coverage`
        - Starts PostgreSQL service.
        - Runs `scripts/run-native-c-tests-with-coverage.sh`.
        - Enforces native 80 percent line-coverage gate.
        - Uploads `.artifacts/native-c` artifacts.

2. `dotnet-tests-coverage`
        - Depends on native coverage job.
        - Builds native library.
        - Runs unit and integration tests with coverage.
        - Generates report in `.artifacts/coverage/report`.
        - Uploads `test-results` and `coverage-report` artifacts.

3. `compose-tests`
        - Runs Docker Compose tests profile using `test-all` container.

4. `compose-runtime`
        - Runs runtime profile and validates `http://localhost:8080/health`.

5. `compose-electron`
        - Runs electron profile smoke path.

## Local-Only Runtime Channels

- Mobile emulator launchers are currently local runtime paths, not CI jobs:
        - `scripts/launch-container-channels-with-wsl2-mobile.sh`
        - `scripts/compose-mobile-emulators-wsl2.sh`
- These launchers reuse the compose `apps` profile, then open Android/iOS preview channels through Ubuntu WSL2/WSLg.

## Compose Profiles In Use

- `tests`
- `runtime`
- `electron`
- `apps`
- `all`

Note: mobile emulator launch does not require a separate compose profile today.

## Required Contracts In CI

- `BANANA_PG_CONNECTION` must point to reachable PostgreSQL host and port.
- `BANANA_NATIVE_PATH` must point at built native artifacts for managed test runs.
- CI jobs print DB host and port only for debugging visibility.

## Local Mirrors For CI Stages

- `scripts/run-native-c-tests-with-coverage.sh`
- `scripts/run-tests-with-coverage.sh`
- `scripts/compose-tests.sh`
- `scripts/compose-runtime.sh`
- `scripts/compose-apps.sh`

## Fast Triage Map

- Native build or ABI failure: inspect `native-c-tests-coverage` first.
- Managed coverage failure: inspect `dotnet-tests-coverage` and `ErrorHandlingMiddleware` logs.
- Compose health failure: inspect `compose-runtime` and `/health` readiness.
- Electron smoke failure: inspect `compose-electron` and native bridge path resolution.
- Mobile emulator launch failure: inspect WSL2 distro resolution, WSLg preflight checks, Android SDK/AVD setup, and `http://localhost:19006` endpoint readiness.
