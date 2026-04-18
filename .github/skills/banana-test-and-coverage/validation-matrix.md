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
