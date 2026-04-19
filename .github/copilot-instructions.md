# Banana Copilot Instructions

- Treat Banana as a multi-language monorepo with four primary domains: native C under `src/native`, ASP.NET under `src/c-sharp/asp.net`, React/Electron under `src/typescript`, and delivery/runtime assets under `docker`, `scripts`, and `.github/workflows`.
- Preserve the controller -> service -> pipeline -> native interop flow documented in `docs/developer-onboarding.md`.
- Keep changes scoped to the touched domain unless a cross-layer contract actually changes.
- Break broad work into helper-sized slices and prefer the narrowest helper agent that owns the touched files: native core, native DAL, native wrapper, API pipeline, API interop, React UI, Electron, compose runtime, workflow, or test triage.
- Use parent domain agents only when more than one helper in the same domain must move together.
- Prefer existing entry points over inventing new ones: workspace tasks, `scripts/*.sh`, CMake targets, `dotnet` test projects, Bun scripts, and Docker Compose profiles.
- For native or integration work, assume `BANANA_PG_CONNECTION` is required whenever PostgreSQL-backed flows are exercised.
- For ASP.NET local/runtime work, keep `BANANA_NATIVE_PATH` explicit and avoid hardcoded machine-specific paths.
- For React work, preserve Bun as the package manager and `VITE_BANANA_API_BASE_URL` as the API base URL contract.
- When behavior changes, update the nearest docs or prompt/skill references rather than scattering duplicate guidance.
- Do not rewrite or reformat unrelated code when adding agent customizations.

## Runtime Contract Lessons (2026-04)

- Treat `Banana Channels (Container Driven)` as the canonical one-click local runtime entry point: run `DETACH=1 scripts/compose-apps.sh`, then launch local desktop Electron with `npm run start:ui` in `src/typescript/electron`.
- Distinguish channels clearly: compose `electron-example` is a smoke container (`npm run smoke`) and is not the desktop UI runtime channel.
- For React container builds that consume `@banana/ui` via file dependency, keep `.dockerignore` excluding `**/node_modules` so host Windows symlinks do not overwrite Linux container installs.
- In `docker/react.Dockerfile`, copy shared UI package files before install and run `bun install --cwd /workspace/src/typescript/shared/ui` before app-level install so shared deps resolve at runtime.
- Keep Electron container builds reproducible with lockfile install (`npm ci --omit=dev`) and compatibility flags required by current native modules (`CXXFLAGS=-fpermissive`).
- Base-image scanner highs on Debian slim may remain; mitigate drift by pinning immutable image digests and tracking residual base CVEs as platform risk when no direct runtime exploit path is introduced.

