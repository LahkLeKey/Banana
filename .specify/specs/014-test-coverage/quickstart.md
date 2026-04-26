# Quickstart — Tests & Coverage (v2)

## Run all managed tests with coverage

```bash
bash scripts/run-tests-with-coverage.sh
```

## Run native tests with coverage

```bash
# Requires BANANA_PG_CONNECTION for DAL paths
export BANANA_PG_CONNECTION="postgres://user:pass@host:5432/db"
bash scripts/run-native-c-tests-with-coverage.sh
```

DAL tests fail loudly (with remediation) if `BANANA_PG_CONNECTION` is unset
unless explicitly marked gated.

## Where to put a new test

| Type | Folder | Description |
|------|--------|-------------|
| Unit       | `tests/unit/`        | In-process, mocked seams, no I/O |
| Integration | `tests/integration/` | Cross-component within one process; native is faked |
| E2E        | `tests/e2e/`         | Full stack with real native + real API |
| Native     | `tests/native/`      | C tests against `006-native-core` exports |

## Coverage anchor

`src/c-sharp/asp.net/coverage-denominator.json` is checked in to stabilize
thresholds. Path + format are stable across v1 → v2.

## Scope guardrails

- Don't silently skip DAL tests; mark gated explicitly or fail loud.
- Don't move `coverage-denominator.json`.
- Don't commit large artifacts to `artifacts/` outside the retention policy.
