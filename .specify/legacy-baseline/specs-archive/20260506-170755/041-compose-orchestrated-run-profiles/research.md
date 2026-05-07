# Research: Compose-Orchestrated Run Profiles

## Decision 1: Compose is the canonical profile orchestrator

- Decision: Treat Docker Compose profile-driven launch as the default runtime contract for all supported local run profiles.
- Rationale: This directly addresses machine drift by moving startup behavior to containerized orchestration.
- Alternatives considered:
  - Keep mixed local-host and compose profile launches: rejected due to non-deterministic local setup variance.

## Decision 2: Use existing script entry points as compose gateways

- Decision: Preserve existing launch script surfaces, but ensure they consistently delegate to compose profile orchestration and readiness checks.
- Rationale: Minimizes migration friction while enforcing deterministic runtime behavior.
- Alternatives considered:
  - Introduce entirely new launcher namespace and deprecate all existing scripts immediately: rejected as high disruption for current contributors.

## Decision 3: Profile matrix as source of truth

- Decision: Define a RunProfileContract matrix that maps each supported profile to compose profiles, required services, readiness checks, and stop behavior.
- Rationale: Deterministic operations require one explicit reference for expected behavior.
- Alternatives considered:
  - Keep profile expectations embedded only in script comments: rejected because drift is hard to detect and test.

## Decision 4: Determinism is validated through repeat-run checks

- Decision: Require repeat launch/stop/relaunch checks for core profiles and consistent readiness outcomes.
- Rationale: Deterministic behavior must be measured by repeated outcomes, not one successful run.
- Alternatives considered:
  - Validate only first-run success: rejected because hidden state drift can still break reproducibility.

## Decision 5: Preserve explicit cross-layer runtime variables

- Decision: Keep existing runtime contract variables explicit (`BANANA_PG_CONNECTION`, `BANANA_NATIVE_PATH`, `VITE_BANANA_API_BASE_URL`) within compose and runtime orchestration.
- Rationale: Existing cross-layer contracts must remain stable while orchestration changes.
- Alternatives considered:
  - Implicit variable discovery from host environment defaults: rejected as fragile and non-reproducible.

## Decision 6: Keep WSL2 contract as primary local baseline

- Decision: Continue enforcing Windows + Docker Desktop + Ubuntu WSL2 entry points as the canonical local desktop/mobile runtime baseline.
- Rationale: Repo already defines this as reproducible standard and it aligns with compose-first orchestration.
- Alternatives considered:
  - Parallel equal-priority launch contracts per host OS path: rejected for now due to increased contract surface and testing complexity.
