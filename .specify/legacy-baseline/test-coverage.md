# Legacy Baseline — Tests & Coverage

**Paths**: `tests/{unit,integration,e2e,native}/`, `scripts/run-tests-with-coverage.sh`, `scripts/run-native-c-tests-with-coverage.sh`, `artifacts/coverage-*.cobertura.xml`, `coverage-denominator.json`

## What exists

- **Unit tests** (`tests/unit/`): mostly C# against `INativeBananaClient` fakes + service/pipeline coverage.
- **Integration tests** (`tests/integration/`): C# tests that exercise the pipeline + interop end-to-end; fakes for native client.
- **Native tests** (`tests/native/`): C tests against core/wrapper/DAL.
- **E2E** (`tests/e2e/`): higher-level scenarios.
- **Coverage**: Cobertura XML artifacts in `artifacts/`, threshold anchored by `src/c-sharp/asp.net/coverage-denominator.json`.
- **Scripts**: `run-tests-with-coverage.sh` (managed), `run-native-c-tests-with-coverage.sh` (native).

## Hard contracts

- Adding a method to `INativeBananaClient` requires updating every fake under `tests/{unit,integration}/*` in the same change (CS0535 cascade).
- Native tests require `BANANA_PG_CONNECTION` for any DAL-touching path.
- Coverage runs gated by CI (`compose-ci.yml`, sdlc workflows).
- `coverage-denominator.json` is checked in to keep coverage thresholds stable across runs.

## What works

- Native C tests are fast and isolate domain rules.
- Pipeline + interop integration tests catch contract drift between C# and native.
- Coverage automation produces stable Cobertura output.

## Vibe drift / pain points

- High volume of `coverage-*.cobertura.xml` files in `artifacts/` — retention is unbounded.
- Multiple fakes per interface — each new method ripples through ~10+ files.
- E2E scope is unclear (unit/integration boundary blur).
- Native tests sometimes silently skip when `BANANA_PG_CONNECTION` is unset rather than failing loudly with remediation.

## Cross-domain dependencies

- Tests every other domain.

## v1 entry points to preserve in v2

- `coverage-denominator.json` path + format
- `run-tests-with-coverage.sh` / `run-native-c-tests-with-coverage.sh` script names
- Cobertura XML as the coverage interchange format
