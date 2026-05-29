# Scripts V3 Scaffold

- `validate-api-parity-governance.sh`: validates API route ownership and parity inventory.
- `validate-steam-launch-gating.sh`: validates spec 008 setup artifacts and reports completed/remaining task counts.
- `validate-coherent-world-unified-slice.sh`: CI guard that blocks superseded/new coherent-lane spec drift outside 031.
- `refresh-coherent-world-evidence.sh`: one-pass refresh for 031 native umbrella/full-engine/full-mmo evidence artifacts.
- `refresh-coherent-world-api-baselines.sh`: refreshes 031 API unit/integration evidence artifacts.
- `run-war-test-suites.sh`: targeted native war test runner with named suites (`focused`, `evidence`, `soak`, `legacy`, `mmo-only`) plus exact `--test` and custom `--regex` selection, including Game-of-Life/sentience orchestration and negotiate de-escalation coverage lanes.
