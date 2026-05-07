# Contract: Compose Run Profile Orchestration

## Purpose

Define the runtime contract for deterministic, reproducible local run profiles orchestrated via Docker Compose.

## Contract requirements

- Each supported local run profile has a canonical compose-driven start and stop path.
- Profile orchestration must not require undocumented host-specific commands.
- Profile startup must declare deterministic readiness conditions.
- Profile validation must support repeat-run reproducibility checks.
- Existing cross-layer runtime environment contracts remain explicit where already required.

## Required contract fields per profile

- profile identifier
- compose profile set
- required service set
- startup command entry point
- shutdown command entry point
- readiness checks with timeout/retry policy
- troubleshooting mappings for common failure classes
- validation lane commands

## Canonical profile matrix

| Profile ID | Compose Profiles | Primary Service | Startup Entrypoint | Shutdown Entrypoint | Readiness Signal |
|------------|------------------|-----------------|--------------------|---------------------|------------------|
| `runtime` | `runtime` | `api` | `scripts/compose-run-profile.sh --profile runtime --action up` | `scripts/compose-run-profile.sh --profile runtime --action down` | `api` service running + `http://localhost:8080/health` |
| `apps` | `apps` | `react-app` | `scripts/compose-run-profile.sh --profile apps --action up` | `scripts/compose-run-profile.sh --profile apps --action down` | `react-app` service running + `http://localhost:5173` |
| `tests` | `tests` | `test-all` | `scripts/compose-run-profile.sh --profile tests --action up --service test-all` | `scripts/compose-run-profile.sh --profile tests --action down` | `test-all` service running/completed |
| `electron-smoke` | `electron` | `electron-example` | `scripts/compose-run-profile.sh --profile electron-smoke --action up --service electron-example` | `scripts/compose-run-profile.sh --profile electron-smoke --action down` | `electron-example` service running/completed |
| `electron-desktop` | `electron` | `electron-desktop` | `scripts/compose-run-profile.sh --profile electron-desktop --action up --service electron-desktop` | `scripts/compose-run-profile.sh --profile electron-desktop --action down` | `electron-desktop` service running |
| `mobile` | `apps`, `android-emulator` | `android-emulator` | `scripts/compose-run-profile.sh --profile mobile --action up --service android-emulator` | `scripts/compose-run-profile.sh --profile mobile --action down` | `android-emulator` service running + `http://localhost:6080` |
| `api-fastify` | `api-fastify` | `api-fastify` | `scripts/compose-run-profile.sh --profile api-fastify --action up --service api-fastify` | `scripts/compose-run-profile.sh --profile api-fastify --action down` | `api-fastify` service running + `http://localhost:8081/health` |
| `workflows` | `workflows` | `workflow-train-not-banana-local` | `scripts/compose-run-profile.sh --profile workflows --action up --service workflow-train-not-banana-local` | `scripts/compose-run-profile.sh --profile workflows --action down` | workflow service running/completed |

## Invariants

- Compose-driven orchestration is the source of truth for run-profile lifecycle.
- Script entry points may wrap compose calls, but cannot diverge from profile contract behavior.
- Optional channels must still conform to contract shape when enabled.
- Deterministic profile behavior is measured by repeated launch/ready/stop runs with consistent outcomes.

## Compatibility guarantees

- Existing runtime contract variables remain explicit in affected profile flows.
- Existing profile names are preserved unless migration notes and compatibility aliases are defined.
- Documentation and runbooks are updated in the same slice when profile contract behavior changes.

## Validation expectations

- Profile matrix completeness check for all supported local profiles.
- Readiness check verification for each profile.
- Repeat-run determinism check for selected primary profiles.
- Contract parity validation between compose config, scripts, and runtime docs.
