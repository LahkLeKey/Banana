---
name: Testing And Coverage Standards
description: Guidance for Banana unit, integration, native, and coverage workflows.
applyTo: "{tests/**/*.cs,tests/**/*.c,scripts/run-tests-with-coverage.sh,scripts/run-native-c-tests-with-coverage.sh,.github/workflows/**/*.yml}"
---

# Testing And Coverage Standards

- Use `test-triage-agent` to isolate failures, repair harness issues, and route fixes to the right helper agent.
- Use `integration-agent` when validation must cross helpers, domains, runtime scripts, or coverage flows.
- Prefer the existing test projects and scripts over one-off commands: native tests in `tests/native`, unit tests in `tests/unit`, integration tests in `tests/integration`.
- Keep integration assumptions explicit: `BANANA_NATIVE_PATH` and `BANANA_PG_CONNECTION` must be set when native-backed integration tests run.
- When a change touches more than one layer, favor the repo’s aggregated test path in `scripts/run-tests-with-coverage.sh`.
- Coverage artifacts should continue to land in the existing `build`, `.artifacts`, or `artifacts` locations rather than new output folders.
- Do not lower or bypass the native coverage gate without an explicit user request.

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

