# Validation Matrix

## Native-only change

- Build: `Build Native Library`
- Tests: native CMake tests or `scripts/run-native-c-tests-with-coverage.sh`
- Coverage: native coverage script with 80% line gate

## ASP.NET-only change

- Build: `Build Banana API`
- Tests: `dotnet test tests/unit/Banana.UnitTests.csproj -c Release`
- Expand to integration tests if interop, data access, or runtime config changed

## Cross-layer backend change

- Tests: `dotnet test tests/integration/Banana.IntegrationTests.csproj -c Release`
- Aggregate coverage: `scripts/run-tests-with-coverage.sh`
- Runtime parity: compose profile or workflow path if containers changed

## Delivery or CI change

- Run the smallest matching compose script or task locally
- Compare to `.github/workflows/compose-ci.yml`
- Use aggregate coverage or runtime health checks when the change affects container orchestration
