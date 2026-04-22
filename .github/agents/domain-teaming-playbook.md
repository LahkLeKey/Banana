# Domain Teaming Playbook

Use this playbook when coordinating Banana work across agents so specialists operate as one team with shared context.

## Domain Responsibility Map

- Native domain: `native-core-agent`, `native-dal-agent`, `native-wrapper-agent`, coordinated by `native-c-agent`
- ASP.NET domain: `api-pipeline-agent`, `api-interop-agent`, coordinated by `csharp-api-agent`
- Frontend domain: `react-ui-agent`, `electron-agent`, coordinated by `react-agent`
- Runtime and CI domain: `compose-runtime-agent`, `mobile-runtime-agent`, `workflow-agent`, coordinated by `infrastructure-agent`
- Cross-domain quality and orchestration: `integration-agent`, `test-triage-agent`, `banana-reviewer`, `banana-planner`, `banana-sdlc`

## Team-First Operating Model

1. Discover touched files, runtime contracts, and validation scope before editing.
2. Select the narrowest owner agent for each slice.
3. Handoff immediately when a slice crosses ownership boundaries.
4. Keep context moving with each handoff instead of restarting discovery.
5. Validate on the narrowest surface first, then expand only when contracts cross domains.

## Required Handoff Packet

Every handoff should include:

1. Outcome goal and expected behavior.
2. Why this target agent owns the next slice.
3. Touched files and likely follow-on files.
4. Contract impacts, especially:
   - `BANANA_PG_CONNECTION`
   - `BANANA_NATIVE_PATH`
   - `VITE_BANANA_API_BASE_URL`
   - `@banana/ui` shared package and Tailwind preset integration
5. Validation already run, validation still required, and failure signals to watch.
6. Open assumptions, risks, and rollback notes.

## Inbound Handoff Checklist

Before coding on a received handoff:

1. Restate the goal and owning scope.
2. Confirm contract assumptions.
3. Confirm validation expectations.
4. Execute the slice or reroute to a narrower owner with updated packet details.

## Escalation Rules

- Escalate to `banana-sdlc` when more than two domains must move together.
- Escalate to `integration-agent` when validation spans multiple domains.
- Escalate to `banana-reviewer` when residual risk remains after implementation.

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

## Ubuntu WSL2 Reproducible Contract (2026-04)

- Enforce the standard Windows + Docker Desktop + Ubuntu (Microsoft Store) workflow described in [ubuntu-wsl2-runtime-contract.md](../ubuntu-wsl2-runtime-contract.md).
- Keep `scripts/launch-container-channels-with-wsl2-electron.sh` as the Windows-shell entry point and `scripts/compose-electron-desktop-wsl2.sh` as the Ubuntu entry point.
- Preserve Ubuntu-first distro selection order: `BANANA_WSL_DISTRO` override, then `Ubuntu-24.04`, then `Ubuntu`.
- Preserve strict direct container-to-WSLg rendering behavior and fail fast with actionable setup guidance when display or Docker integration prerequisites are missing.
- Treat missing Ubuntu Docker server/socket as an environment contract failure first; surface clear remediation and keep exit code `42` for integration preflight failures.
