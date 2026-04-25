---
description: "Tasks for v2 native core regeneration"
---

# Tasks: Native Core (v2)

## Phase 0 — Inventory

- [ ] T001 Enumerate every public symbol in `src/native/wrapper/banana_wrapper.h` with its managed consumer (or "unused").
- [ ] T002 Enumerate every domain module in `src/native/core/domain/` with its consumers.
- [ ] T003 Record results in `.specify/specs/006-native-core/research.md`.

## Phase 1 — Stable ABI shim

- [ ] T010 Create v2 wrapper skeleton with the same public symbols and numeric `NativeStatusCode` values.
- [ ] T011 [P] Add coverage tests for each exported symbol in `tests/native/wrapper/`.
- [ ] T012 Verify `007-aspnet-pipeline` integration tests pass against v2 wrapper.

## Phase 2 — Consolidate core domain

- [ ] T020 Move kept modules into v2 layout; drop unused modules.
- [ ] T021 [P] Update CMake to compile only kept modules.
- [ ] T022 [P] Re-run native + managed tests.

## Phase 3 — Unified errors + DAL gating

- [ ] T030 Introduce single error-reporting function and migrate exports to it.
- [ ] T031 Add typed failure when `BANANA_PG_CONNECTION` is unset; remediation message included.
- [ ] T032 [P] Update tests to assert typed failure (not silent skip).

## Phase 4 — Legacy removal

- [ ] T040 Remove v1 domain modules without consumers.
- [ ] T041 Update `legacy-baseline/native-core.md` "Superseded" markers.
- [ ] T042 Final native + managed test pass.

## Dependencies

- T010 blocks all Phase 2/3 work.
- T020 blocks T040.
- T031 must land before tests change to fail-fast (T032).
