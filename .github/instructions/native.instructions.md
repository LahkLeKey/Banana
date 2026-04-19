---
name: Native C Standards
description: Guidance for work in the Banana native core, DAL, wrapper, and CMake targets.
applyTo: "{CMakeLists.txt,src/native/**/*.c,src/native/**/*.h,tests/native/**/*.c}"
---

# Native C Standards

- Use `native-core-agent` for `src/native/core` logic, execution context, and focused native tests.
- Use `native-dal-agent` for `src/native/core/dal`, PostgreSQL-gated behavior, and DB payload shaping.
- Use `native-wrapper-agent` for `src/native/wrapper`, ABI exports, statuses, and interop-facing contracts.
- Use `native-c-agent` only when more than one native helper-owned surface must move together.
- Preserve the wrapper ABI in `src/native/wrapper/banana_wrapper.h` unless the task explicitly requires a contract change.
- Keep PostgreSQL support behind the existing `BANANA_ENABLE_POSTGRES` build option and related compile definitions.
- Prefer updating CMake targets and existing native tests over adding ad-hoc shell build logic.
- Validate native changes with the existing CMake test flow or the `Build Native Library` workspace task when appropriate.
- When coverage is relevant, use `scripts/run-native-c-tests-with-coverage.sh` and respect the 80% line coverage gate.

## Shared Frontend Contract

- If a task touches src/typescript/react, src/typescript/electron, or src/typescript/shared/ui, keep shared primitives in @banana/ui instead of app-local thin re-export stubs.
- Reuse @banana/ui/tailwind/preset and @banana/ui/styles/tokens.css from consuming apps.
- Install dependencies in src/typescript/shared/ui before running app-level bun check/build flows.
- Reference .github/shared-typescript-ui.md for the full contract.
