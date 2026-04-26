# Tests (spec 014)

Taxonomy:

- `unit/` — fast, isolated; ≤2 fakes per change (interop seam invariant).
- `integration/` — Postgres-backed; loud DAL gating (`DbNotConfigured` when `BANANA_PG_CONNECTION` unset).
- `e2e/` — integration-equivalent API contract lane centered on deterministic contract tests in `tests/e2e/Contracts`; run with `dotnet test tests/e2e --filter "FullyQualifiedName~Contracts"` for fast cross-surface contract validation.
- `native/` — C tests via CMake + ctest (`scripts/run-native-c-tests-with-coverage.sh`).

Coverage denominator stability is enforced by
`scripts/sync-banana-api-coverage-denominator.py` (to be re-added when the
banana API surface grows).
