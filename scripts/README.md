# Scripts V3 Scaffold

- `validate-api-parity-governance.sh`: validates API route ownership and parity inventory.
- `validate-steam-launch-gating.sh`: validates spec 008 setup artifacts and reports completed/remaining task counts.
- `validate-coherent-world-unified-slice.sh`: CI guard that blocks superseded/new coherent-lane spec drift outside 031.
- `run-native-feedback-loop.sh`: runs the native human-feedback loop factory in scenario mode or DX12 playloop script mode (`--script`), emitting operator-readable telemetry snapshots.
- `run-war-test-suites.sh`: scenario-suite orchestrator for the feedback loop factory (`focused`, `evidence`, `soak`, `gameplay`, `legacy`, `mmo-only`) with optional scenario overrides (`warfront`, `negotiate`, `comeback`, `flank`, `pressure`, `truce`, `rally`) and optional `--script-dir` (`<scenario>.dx12play`) execution.
- `run-dx12-playtest-suites.sh`: launches the DX12 POC with polished demo-scene assets for interactive playtests or autotest sweeps (`playtest`, `showcase`, `metro`, `corridor`, `continent`, `all`, `features`) with optional `--feature <name>` overrides.
- `refresh-coherent-world-evidence.sh`: refreshes 031 evidence artifacts by capturing feedback-loop suite output logs, optionally forwarding `BANANA_NATIVE_FEEDBACK_SCRIPT_DIR` into suite runs.
- `refresh-coherent-world-api-baselines.sh`: refreshes 031 API unit/integration evidence artifacts.

Playtest examples:
- `bash scripts/run-dx12-playtest-suites.sh --suite playtest` (interactive playtest with telemetry + auto-target defaults)
- `bash scripts/run-dx12-playtest-suites.sh --suite playtest --autotest 6` (autotest sweep)
- `bash scripts/run-dx12-playtest-suites.sh --suite features --autotest 6` (autotest sweep for all war-sentience feature playtests)

Notes:
- Autotest runs with auto-target enabled inject a move target automatically by setting `BANANA_DX12_POC_AUTOTEST_ALLOW_TARGET=1`.
