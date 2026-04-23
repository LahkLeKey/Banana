# Domain Teaming Playbook

Use this playbook when coordinating Banana work across agents so specialists operate as one team with shared context.

## Domain Responsibility Map

- Native domain: `native-core-agent`, `native-dal-agent`, `native-wrapper-agent`, coordinated by `native-c-agent`
- ASP.NET domain: `api-pipeline-agent`, `api-interop-agent`, coordinated by `csharp-api-agent`
- Frontend domain: `react-ui-agent`, `electron-agent`, coordinated by `react-agent`
- Runtime and CI domain: `compose-runtime-agent`, `mobile-runtime-agent`, `workflow-agent`, coordinated by `infrastructure-agent`
- Product-specialized classifier domain: `banana-classifier-agent` coordinating banana-vs-not-banana behavior across training, API, and simple frontend slices
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

- During planning, review, and triage, ensure ML changes stay inside `src/native/core/domain/ml/{shared,regression,binary,transformer}` and `src/native/wrapper/domain/ml/{shared,regression,binary,transformer}`.
- Require explicit confirmation that public contracts in `src/native/core/domain/banana_ml_models.h` and `src/native/wrapper/banana_wrapper.h` remain stable unless a breaking change is approved.
- When ML files move, require coordinated `CMakeLists.txt` updates for `BANANA_CORE_SOURCES` and `BANANA_WRAPPER_SOURCES`.
- Route implementation to native helpers (`native-core-agent`, `native-wrapper-agent`, `native-c-agent`) and require native build plus `ctest` evidence.

## Not-Banana Training Contract (2026-04)

- Treat `data/not-banana/corpus.json`, `scripts/train-not-banana-model.py`, and `.github/workflows/train-not-banana-model.yml` as one coordinated contract.
- Require drift checks whenever vocabulary or classifier logic changes across native and API layers.
- Ensure downstream behavior stays aligned in `src/native/core/domain/banana_not_banana.c` and `src/typescript/api/src/domains/not-banana/routes.ts`.
- Flag missing training validation, stale artifacts, or undocumented threshold shifts as release risk.

## Ubuntu WSL2 Reproducible Contract (2026-04)

- Enforce the standard Windows + Docker Desktop + Ubuntu (Microsoft Store) workflow described in [ubuntu-wsl2-runtime-contract.md](../ubuntu-wsl2-runtime-contract.md).
- Keep `scripts/launch-container-channels-with-wsl2-electron.sh` as the Windows-shell entry point and `scripts/compose-electron-desktop-wsl2.sh` as the Ubuntu entry point.
- Preserve Ubuntu-first distro selection order: `BANANA_WSL_DISTRO` override, then `Ubuntu-24.04`, then `Ubuntu`.
- Preserve strict direct container-to-WSLg rendering behavior and fail fast with actionable setup guidance when display or Docker integration prerequisites are missing.
- Treat missing Ubuntu Docker server/socket as an environment contract failure first; surface clear remediation and keep exit code `42` for integration preflight failures.
