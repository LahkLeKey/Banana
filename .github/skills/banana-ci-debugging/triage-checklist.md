# CI And Runtime Triage Checklist

## Common Failure Classes

- PostgreSQL not ready or wrong host/port in `BANANA_PG_CONNECTION`
- Native library not built or wrong `BANANA_NATIVE_PATH`
- Compose health checks failing on `http://localhost:8080/health`
- Profile mismatch between local compose script and workflow stage
- Coverage tool output path missing or changed unexpectedly

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
