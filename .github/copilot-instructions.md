# Banana Copilot Instructions

- Treat Banana as a multi-language monorepo with four primary domains: native C under `src/native`, ASP.NET under `src/c-sharp/asp.net`, React/Electron/Mobile under `src/typescript`, and delivery/runtime assets under `docker`, `scripts`, and `.github/workflows`.
- Preserve the controller -> service -> pipeline -> native interop flow documented in `docs/developer-onboarding.md`.
- Keep changes scoped to the touched domain unless a cross-layer contract actually changes.
- Break broad work into helper-sized slices and prefer the narrowest helper agent that owns the touched files: banana classifier, native core, native DAL, native wrapper, API pipeline, API interop, React UI, Electron, mobile runtime, compose runtime, workflow, technical writer, value-risk prioritization, or test triage.
- Use parent domain agents only when more than one helper in the same domain must move together.
- Prefer existing entry points over inventing new ones: workspace tasks, `scripts/*.sh`, CMake targets, `dotnet` test projects, Bun scripts, and Docker Compose profiles.
- For Spec Kit-driven work, use the Specify CLI flow (`specify specify`, `specify plan`, `specify tasks`) and verify `.specify/feature.json` points at the intended feature and that the corresponding `.specify/specs/<feature>/plan.md` exists before implementation.
- For `003-coverage-80-rehydration` execution, validate normalized lane schema parity, tuple completeness, and workspace-relative evidence paths before closing tasks.
- For native or integration work, assume `BANANA_PG_CONNECTION` is required whenever PostgreSQL-backed flows are exercised.
- For ASP.NET local/runtime work, keep `BANANA_NATIVE_PATH` explicit and avoid hardcoded machine-specific paths.
- For React work, preserve Bun as the package manager and `VITE_BANANA_API_BASE_URL` as the API base URL contract.
- When behavior changes, update the nearest docs or prompt/skill references rather than scattering duplicate guidance.
- Do not rewrite or reformat unrelated code when adding agent customizations.
- Treat feedback-loop updates as SDLC inputs that should flow through incremental feature branches and automated pull requests.
- For automation-driven changes, prefer GH CLI orchestration entry points (`scripts/workflow-orchestrate-triaged-item-pr.sh` and `scripts/workflow-orchestrate-sdlc.sh`) over one-off branch/PR commands.
- Keep wiki updates in the same orchestration path (via `scripts/workflow-sync-wiki.sh`) when delivery behavior, workflows, or runbooks change.
- Treat `.specify/wiki/` as the canonical AI-consumable wiki store and keep `.wiki/` as a human-reference publication surface.
- Until expanded intentionally, enforce `.wiki` markdown pages via `.specify/wiki/human-reference-allowlist.txt` to prevent unreviewed page growth.

## Runtime Contract Lessons (2026-04)

- Treat `Banana Channels (Container Driven)` as the canonical one-click local runtime entry point: launch from a Windows shell with `scripts/launch-container-channels-with-wsl2-electron.sh` (or the VS Code profile) so Docker Desktop + Ubuntu-24.04/Ubuntu WSL2 runtime contracts stay enforced.
- Distinguish channels clearly: compose `electron-example` is a smoke container (`npm run smoke`) and is not the desktop UI runtime channel.
- For React container builds that consume `@banana/ui` via file dependency, keep `.dockerignore` excluding `**/node_modules` so host Windows symlinks do not overwrite Linux container installs.
- In `docker/react.Dockerfile`, copy shared UI package files before install and run `bun install --cwd /workspace/src/typescript/shared/ui` before app-level install so shared deps resolve at runtime.
- Keep Electron container builds reproducible with lockfile install (`npm ci --omit=dev`) and compatibility flags required by current native modules (`CXXFLAGS=-fpermissive`).
- Keep mobile emulator channels explicit in Ubuntu WSL2: Android emulator may launch through WSLg when SDK tools are installed; Apple iOS Simulator remains macOS-only and must stay a web-preview fallback on Ubuntu.
- Base-image scanner highs on Debian slim may remain; mitigate drift by pinning immutable image digests and tracking residual base CVEs as platform risk when no direct runtime exploit path is introduced.


## Ubuntu WSL2 Reproducible Contract (2026-04)

- Enforce the standard Windows + Docker Desktop + Ubuntu (Microsoft Store) workflow described in [ubuntu-wsl2-runtime-contract.md](./ubuntu-wsl2-runtime-contract.md).
- Keep `scripts/launch-container-channels-with-wsl2-electron.sh` as the Windows-shell entry point and `scripts/compose-electron-desktop-wsl2.sh` as the Ubuntu entry point.
- Keep `scripts/launch-container-channels-with-wsl2-mobile.sh` as the Windows-shell entry point and `scripts/compose-mobile-emulators-wsl2.sh` as the Ubuntu entry point for mobile emulator channels.
- Preserve Ubuntu-first distro selection order: `BANANA_WSL_DISTRO` override, then `Ubuntu-24.04`, then `Ubuntu`.
- Preserve strict direct container-to-WSLg rendering behavior and fail fast with actionable setup guidance when display or Docker integration prerequisites are missing.
- Treat missing Ubuntu Docker server/socket as an environment contract failure first; surface clear remediation and keep exit code `42` for integration preflight failures.

<!-- SPECKIT START -->
For additional context about technologies to be used, project structure,
shell commands, and other important information, read the current plan
at `.specify/specs/007-dal-runtime-contract-hardening/plan.md`
<!-- SPECKIT END -->
