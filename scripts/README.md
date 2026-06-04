# Scripts V3 Scaffold

- `validate-api-parity-governance.sh`: validates API route ownership and parity inventory.
- `validate-steam-launch-gating.sh`: validates spec 008 setup artifacts and reports completed/remaining task counts.
- `validate-coherent-world-unified-slice.sh`: CI guard that blocks superseded/new coherent-lane spec drift outside 031.
- `run-native-feedback-loop.sh`: runs the native human-feedback loop factory in scenario mode or DX12 playloop script mode (`--script`), emitting operator-readable telemetry snapshots.
- `run-war-test-suites.sh`: scenario-suite orchestrator for the feedback loop factory (`focused`, `evidence`, `soak`, `legacy`, `mmo-only`) with optional scenario overrides and optional `--script-dir` (`<scenario>.dx12play`) execution.
- `refresh-coherent-world-evidence.sh`: refreshes 031 evidence artifacts by capturing feedback-loop suite output logs, optionally forwarding `BANANA_NATIVE_FEEDBACK_SCRIPT_DIR` into suite runs.
- `refresh-coherent-world-api-baselines.sh`: refreshes 031 API unit/integration evidence artifacts.
