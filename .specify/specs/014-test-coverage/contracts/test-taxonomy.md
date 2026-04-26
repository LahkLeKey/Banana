# Contract — Test Taxonomy

## Categories

| Category | Folder | Process | I/O | Native | API |
|----------|--------|---------|-----|--------|-----|
| Unit        | `tests/unit/`        | one  | none | mocked   | mocked |
| Integration | `tests/integration/` | one  | local | faked    | in-process |
| E2E         | `tests/e2e/`         | many | real  | real     | real |
| Native      | `tests/native/`      | one  | gated by `BANANA_PG_CONNECTION` | n/a (under test) | n/a |

## Rules

- A test that touches the network or a real DB is integration or e2e, not unit.
- A test that exercises real native via P/Invoke is e2e or native; integration may use fakes for native.
- DAL tests fail loud on missing `BANANA_PG_CONNECTION`. Use a `[Gated]` attribute or skip-with-message only when intentional.

## Coverage

- Cobertura XML at `artifacts/coverage-*.cobertura.xml`.
- `src/c-sharp/asp.net/coverage-denominator.json` stable anchor.
- Retention policy bounded; old artifacts pruned by CI step.

## Interop fake update rule

Adding a method to the interop seam (`007`) MUST touch ≤2 test files. If you
find yourself editing many fakes, stop and use the centralized fake helper or
codegen.
