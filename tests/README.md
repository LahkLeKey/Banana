# Tests (spec 014)

Taxonomy:

- `integration/` — Postgres-backed; loud DAL gating (`DbNotConfigured` when `BANANA_PG_CONNECTION` unset).
- `api/` — TypeScript API contract tests and route/domain behavior in `src/typescript/api` (`bun test --cwd src/typescript/api`).
- `native/` — C tests via CMake + ctest with hard gates:
	- `scripts/run-native-c-tests-with-coverage.sh` enforces native line coverage threshold (default 100%, configurable via `BANANA_NATIVE_COVERAGE_THRESHOLD`).
	- `scripts/run-native-static-memory-analysis.sh` enforces clang static memory analysis (`scan-build` + `unix.Malloc`), sanitizer leak checks (ASAN/UBSAN), and strict optimization compile quality.

Coverage denominator stability for the TypeScript API is enforced by
`scripts/sync-banana-api-coverage-denominator.py`.

## Controller Acceptance Steps (Spec 048 / US1)

- Launch the game viewport and hold `W`, `A`, `S`, `D` one at a time; verify each starts movement immediately.
- Repeat with `ArrowUp`, `ArrowLeft`, `ArrowDown`, `ArrowRight`; verify directional parity with WASD.
- Release all movement keys and confirm the player stops within one frame budget.
- Right-click in viewport and confirm movement does not start from mouse input.
