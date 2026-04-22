# CI And Runtime Triage Checklist

## Common Failure Classes

- PostgreSQL not ready or wrong host/port in `BANANA_PG_CONNECTION`
- Native library not built or wrong `BANANA_NATIVE_PATH`
- Compose health checks failing on `http://localhost:8080/health`
- Profile mismatch between local compose script and workflow stage
- Coverage tool output path missing or changed unexpectedly

## Likely Helper Owners

- `native-core-agent` or `native-wrapper-agent` for native build or ABI failures
- `api-interop-agent` for `BANANA_NATIVE_PATH` loading failures or managed/native contract drift
- `compose-runtime-agent` for local runtime and compose health-check failures
- `workflow-agent` for artifact paths, coverage jobs, or workflow-only breakage
- `test-triage-agent` when the owner is still unclear or a harness failure is suspected

## Workflow Map

- Native coverage stage: `.github/workflows/compose-ci.yml` job `native-c-tests-coverage`
- .NET coverage stage: `.github/workflows/compose-ci.yml` job `dotnet-tests-coverage`
- Compose tests stage: `.github/workflows/compose-ci.yml` job `compose-tests`
- Runtime health stage: `.github/workflows/compose-ci.yml` job `compose-runtime`

## Local Mirrors

- `scripts/run-native-c-tests-with-coverage.sh`
- `scripts/run-tests-with-coverage.sh`
- `scripts/compose-tests.sh`
- `scripts/compose-apps.sh`

## Native ML Domain Contract (2026-04)

- Keep public ML API declarations in `src/native/core/domain/banana_ml_models.h` and wrapper exports in `src/native/wrapper/banana_wrapper.h` stable unless a contract change is explicit.
- Keep model internals split under `src/native/core/domain/ml/{shared,regression,binary,transformer}`.
- Keep wrapper ML bridges split under `src/native/wrapper/domain/ml/{shared,regression,binary,transformer}`.
- When ML model source files move, update `BANANA_CORE_SOURCES` and `BANANA_WRAPPER_SOURCES` in `CMakeLists.txt` in the same change.
- Validate ML refactors with `Build Native Library` and `ctest --test-dir build/native -C Release --output-on-failure`.

## Not-Banana Training Contract (2026-04)

- Keep `data/not-banana/corpus.json` as the canonical labeled corpus for not-banana vocabulary training.
- Use `scripts/train-not-banana-model.py` to regenerate metrics and model artifacts.
- Use `.github/workflows/train-not-banana-model.yml` as the CI training and drift-check path.
- Track any vocabulary drift between training outputs and runtime classifiers explicitly, especially `src/native/core/domain/banana_not_banana.c` and `src/typescript/api/src/domains/not-banana/routes.ts`.

## Shared Frontend Contract

- If a task touches src/typescript/react, src/typescript/electron, or src/typescript/shared/ui, keep shared primitives in @banana/ui instead of app-local thin re-export stubs.
- Reuse @banana/ui/tailwind/preset and @banana/ui/styles/tokens.css from consuming apps.
- Install dependencies in src/typescript/shared/ui before running app-level bun check/build flows.
- Reference .github/shared-typescript-ui.md for the full contract.

## Cross-Domain Teaming Protocol

- Follow [domain-teaming-playbook.md](../../agents/domain-teaming-playbook.md) for ownership boundaries, handoff packet expectations, and escalation rules.
- Identify the primary owner, supporting helpers, and validation owner before implementation.
- When handing work to another agent, include objective, ownership reason, touched files, contract impacts, validation state, and open risks.
- Preserve context continuity by carrying forward confirmed assumptions and prior validation signals.
- Escalate to `banana-sdlc` for multi-domain implementation orchestration and `integration-agent` for multi-domain validation orchestration.

## Runtime Contract Lessons (2026-04)

- Treat `Banana Channels (Container Driven)` as the canonical one-click local runtime entry point: launch from a Windows shell with `scripts/launch-container-channels-with-wsl2-electron.sh` (or the VS Code profile) so Docker Desktop + Ubuntu-24.04/Ubuntu WSL2 runtime contracts stay enforced.
- Distinguish channels clearly: compose `electron-example` is a smoke container (`npm run smoke`) and is not the desktop UI runtime channel.
- For React container builds that consume `@banana/ui` via file dependency, keep `.dockerignore` excluding `**/node_modules` so host Windows symlinks do not overwrite Linux container installs.
- In `docker/react.Dockerfile`, copy shared UI package files before install and run `bun install --cwd /workspace/src/typescript/shared/ui` before app-level install so shared deps resolve at runtime.
- Keep Electron container builds reproducible with lockfile install (`npm ci --omit=dev`) and compatibility flags required by current native modules (`CXXFLAGS=-fpermissive`).
- Base-image scanner highs on Debian slim may remain; mitigate drift by pinning immutable image digests and tracking residual base CVEs as platform risk when no direct runtime exploit path is introduced.


## Ubuntu WSL2 Reproducible Contract (2026-04)

- Enforce the standard Windows + Docker Desktop + Ubuntu (Microsoft Store) workflow described in [ubuntu-wsl2-runtime-contract.md](../../ubuntu-wsl2-runtime-contract.md).
- Keep `scripts/launch-container-channels-with-wsl2-electron.sh` as the Windows-shell entry point and `scripts/compose-electron-desktop-wsl2.sh` as the Ubuntu entry point.
- Preserve Ubuntu-first distro selection order: `BANANA_WSL_DISTRO` override, then `Ubuntu-24.04`, then `Ubuntu`.
- Preserve strict direct container-to-WSLg rendering behavior and fail fast with actionable setup guidance when display or Docker integration prerequisites are missing.
- Treat missing Ubuntu Docker server/socket as an environment contract failure first; surface clear remediation and keep exit code `42` for integration preflight failures.
