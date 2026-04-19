# Validation Matrix

## Native-only change

- Primary helpers: `native-core-agent`, `native-dal-agent`, `native-wrapper-agent`
- Build: `Build Native Library`
- Tests: native CMake tests or `scripts/run-native-c-tests-with-coverage.sh`
- Coverage: native coverage script with 80% line gate

## ASP.NET-only change

- Primary helpers: `api-pipeline-agent`, `api-interop-agent`
- Build: `Build Banana API`
- Tests: `dotnet test tests/unit/Banana.UnitTests.csproj -c Release`
- Expand to integration tests if interop, data access, or runtime config changed

## Frontend or Electron change

- Primary helpers: `react-ui-agent`, `electron-agent`
- Build or check: `bun run check` and `bun run build` in `src/typescript/react`
- Expand to integration validation if backend contracts or runtime wiring changed

## Cross-layer backend change

- Likely coordinators: `csharp-api-agent`, `native-c-agent`, `integration-agent`
- Tests: `dotnet test tests/integration/Banana.IntegrationTests.csproj -c Release`
- Aggregate coverage: `scripts/run-tests-with-coverage.sh`
- Runtime parity: compose profile or workflow path if containers changed

## Delivery or CI change

- Primary helpers: `compose-runtime-agent`, `workflow-agent`
- Run the smallest matching compose script or task locally
- Compare to `.github/workflows/compose-ci.yml`
- Use aggregate coverage or runtime health checks when the change affects container orchestration

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
