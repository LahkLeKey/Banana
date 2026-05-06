# Research: Frontend Config Drift Closure

## Decision 1: Validate effective runtime value, not only container env

- Decision: Add runtime-level checks that verify what frontend code resolves (`resolveApiBaseUrl`) and what UI renders.
- Rationale: `docker compose exec ... printenv` can pass while UI still shows `<unset>` due to stale process/runtime path mismatches.
- Alternatives considered:
  - Container-only `printenv` checks: rejected because insufficient to catch drift in effective runtime resolution.
  - Manual browser-only checks: rejected because not automatable and too slow for repeated triage.

## Decision 2: Keep Compose + VS Code launch tasks as the canonical startup path

- Decision: Enforce diagnostics and docs around existing `compose-run-profile.sh` and VS Code frontend launch tasks.
- Rationale: Existing project contract already centers deterministic profile startup on Compose scripts and launch tasks.
- Alternatives considered:
  - Introduce new startup wrappers: rejected to avoid parallel orchestration drift.

## Decision 3: Fail loud when API base resolves empty

- Decision: Add explicit user-visible configuration failure messaging whenever effective API base is empty.
- Rationale: Silent degraded behavior (`API base: <unset>`) hides root cause and wastes debugging time.
- Alternatives considered:
  - Continue soft warning only: rejected because it does not enforce contract correctness.

## Decision 4: Add one diagnostic flow covering layers in order

- Decision: Provide a single command/script that checks in sequence:
  1. Launch entry point used
  2. Compose env interpolation
  3. Running container env
  4. Frontend effective runtime value
  5. Remediation commands
- Rationale: Layered checks isolate drift source quickly and align with controller/service/pipeline discipline at runtime-contract level.
- Alternatives considered:
  - Separate ad-hoc commands in docs only: rejected due to low repeatability.

## Decision 5: Preserve host-browser-resolvable API default

- Decision: Keep default API base as `http://localhost:8080` for frontend slices, with explicit override envs.
- Rationale: Host browser cannot resolve Docker-internal service names such as `api`.
- Alternatives considered:
  - Service-hostname defaults: rejected due to host/browser resolution failure.
