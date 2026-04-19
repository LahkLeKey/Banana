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

## Runtime Contract Lessons (2026-04)

- Treat `Banana Channels (Container Driven)` as the canonical one-click local runtime entry point: run `DETACH=1 scripts/compose-apps.sh`, then launch local desktop Electron with `npm run start:ui` in `src/typescript/electron`.
- Distinguish channels clearly: compose `electron-example` is a smoke container (`npm run smoke`) and is not the desktop UI runtime channel.
- For React container builds that consume `@banana/ui` via file dependency, keep `.dockerignore` excluding `**/node_modules` so host Windows symlinks do not overwrite Linux container installs.
- In `docker/react.Dockerfile`, copy shared UI package files before install and run `bun install --cwd /workspace/src/typescript/shared/ui` before app-level install so shared deps resolve at runtime.
- Keep Electron container builds reproducible with lockfile install (`npm ci --omit=dev`) and compatibility flags required by current native modules (`CXXFLAGS=-fpermissive`).
- Base-image scanner highs on Debian slim may remain; mitigate drift by pinning immutable image digests and tracking residual base CVEs as platform risk when no direct runtime exploit path is introduced.

