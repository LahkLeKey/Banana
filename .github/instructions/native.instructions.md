---
name: Native C Standards
description: Guidance for work in the Banana native core, DAL, wrapper, and CMake targets.
applyTo: "{CMakeLists.txt,src/native/**/*.c,src/native/**/*.h,tests/native/**/*.c}"
---

# Native C Standards

- Preserve the wrapper ABI in `src/native/wrapper/banana_wrapper.h` unless the task explicitly requires a contract change.
- Keep PostgreSQL support behind the existing `BANANA_ENABLE_POSTGRES` build option and related compile definitions.
- Prefer updating CMake targets and existing native tests over adding ad-hoc shell build logic.
- Validate native changes with the existing CMake test flow or the `Build Native Library` workspace task when appropriate.
- When coverage is relevant, use `scripts/run-native-c-tests-with-coverage.sh` and respect the 80% line coverage gate.
