# CI And Runtime Triage Checklist

## Common Failure Classes

- PostgreSQL not ready or wrong host/port in `BANANA_PG_CONNECTION`
- Native library not built or wrong `BANANA_NATIVE_PATH`
- Compose health checks failing on `http://localhost:8080/health`
- Profile mismatch between local compose script and workflow stage
- Coverage tool output path missing or changed unexpectedly

## Likely Helper Owners

- `native-core-agent` or `native-wrapper-agent` for native build or ABI failures
- `api-interop-agent` for `BANANA_NATIVE_PATH` loading failures or managed/native contract drift
- `compose-runtime-agent` for local runtime and compose health-check failures
- `workflow-agent` for artifact paths, coverage jobs, or workflow-only breakage
- `test-triage-agent` when the owner is still unclear or a harness failure is suspected

## Workflow Map

- Native coverage stage: `.github/workflows/compose-ci.yml` job `native-c-tests-coverage`
- .NET coverage stage: `.github/workflows/compose-ci.yml` job `dotnet-tests-coverage`
- Compose tests stage: `.github/workflows/compose-ci.yml` job `compose-tests`
- Runtime health stage: `.github/workflows/compose-ci.yml` job `compose-runtime`

## Local Mirrors

- `scripts/run-native-c-tests-with-coverage.sh`
- `scripts/run-tests-with-coverage.sh`
- `scripts/compose-tests.sh`
- `scripts/compose-apps.sh`

## Shared Frontend Contract

- If a task touches src/typescript/react, src/typescript/electron, or src/typescript/shared/ui, keep shared primitives in @banana/ui instead of app-local thin re-export stubs.
- Reuse @banana/ui/tailwind/preset and @banana/ui/styles/tokens.css from consuming apps.
- Install dependencies in src/typescript/shared/ui before running app-level bun check/build flows.
- Reference .github/shared-typescript-ui.md for the full contract.

## Cross-Domain Teaming Protocol

- Follow [domain-teaming-playbook.md](../../agents/domain-teaming-playbook.md) for ownership boundaries, handoff packet expectations, and escalation rules.
- Identify the primary owner, supporting helpers, and validation owner before implementation.
- When handing work to another agent, include objective, ownership reason, touched files, contract impacts, validation state, and open risks.
- Preserve context continuity by carrying forward confirmed assumptions and prior validation signals.
- Escalate to `banana-sdlc` for multi-domain implementation orchestration and `integration-agent` for multi-domain validation orchestration.
