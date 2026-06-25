# Scripts V3 Scaffold

- `validate-api-parity-governance.sh`: validates API route ownership and parity inventory.
- `validate-steam-launch-gating.sh`: validates spec 008 setup artifacts and reports completed/remaining task counts.
- `validate-coherent-world-unified-slice.sh`: CI guard that blocks superseded/new coherent-lane spec drift outside 031.
- `build-k3h4-standalone-release.sh`: packages a standalone K3H4 model release bundle (native library, ABI header, contracts, metadata, checksum) under `artifacts/native/k3h4/releases/<version>/`.
- `run-native-feedback-loop.sh`: runs the native human-feedback loop factory in scenario mode or DX12 playloop script mode (`--script`), emitting operator-readable telemetry snapshots.
- `run-war-test-suites.sh`: scenario-suite orchestrator for the feedback loop factory (`focused`, `evidence`, `soak`, `gameplay`, `legacy`, `mmo-only`) with optional scenario overrides (`warfront`, `negotiate`, `comeback`, `flank`, `pressure`, `truce`, `rally`) and optional `--script-dir` (`<scenario>.dx12play`) execution.
- `run-native-coverage.sh`: configures a coverage-enabled native build, runs the native test suite, and exports HTML/text coverage artifacts that can be run locally or in CI.
- `run-native-coverage-ci-container.sh`: builds a CI-parity Ubuntu container and runs the native coverage workflow inside Docker before pushing.
- `run-dx12-playtest-suites.sh`: launches the DX12 POC with polished demo-scene assets for interactive playtests or autotest sweeps (`playtest`, `showcase`, `metro`, `corridor`, `continent`, `all`, `features`) with optional `--feature <name>` overrides.
- `refresh-coherent-world-evidence.sh`: refreshes 031 evidence artifacts by capturing feedback-loop suite output logs, optionally forwarding `BANANA_NATIVE_FEEDBACK_SCRIPT_DIR` into suite runs.
- `refresh-coherent-world-api-baselines.sh`: refreshes 031 API unit/integration evidence artifacts.
- `panels-storybook-build.sh`: builds static Storybook assets for `@banana/panels` into `artifacts/storybook/panels-static`.
- `panels-storybook-local.sh`: builds and serves static `@banana/panels` Storybook locally (default `http://localhost:6010`).
- `panels-storybook-deploy.sh`: deploys static `@banana/panels` Storybook to `panels.banana.engineer` over SSH (`rsync` with `scp` fallback).

Coverage examples:
- `bash scripts/run-native-coverage.sh`
- `bash scripts/run-native-coverage.sh --build-dir out/native-coverage-ci --output-dir artifacts/native/coverage-ci`
- `bash scripts/run-native-coverage-ci-container.sh`
- `bash scripts/run-native-coverage-ci-container.sh --no-build-image --build-dir out/native-coverage-ci-docker --output-dir artifacts/native/coverage-ci-docker`

Playtest examples:
- `bash scripts/run-dx12-playtest-suites.sh --suite playtest` (interactive playtest with telemetry + auto-target defaults)
- `bash scripts/run-dx12-playtest-suites.sh --suite playtest --autotest 6` (autotest sweep)
- `bash scripts/run-dx12-playtest-suites.sh --suite features --autotest 6` (autotest sweep for all war-sentience feature playtests)

Panels Storybook examples:
- `bash scripts/panels-storybook-build.sh`
- `bash scripts/panels-storybook-local.sh --port 6010`
- `PANELS_STORYBOOK_DEPLOY_USER=<ssh-user> bash scripts/panels-storybook-deploy.sh`

Notes:
- Autotest runs with auto-target enabled inject a move target automatically by setting `BANANA_DX12_POC_AUTOTEST_ALLOW_TARGET=1`.
- Standalone K3H4 bundle example: `bash scripts/build-k3h4-standalone-release.sh --version 2026.06.14`.
