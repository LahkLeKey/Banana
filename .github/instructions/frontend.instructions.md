---
name: Frontend Standards
description: Guidance for Banana React code under src/typescript.
applyTo: "src/typescript/**/*.{ts,tsx,js,json,css}"
---

# Frontend Standards

- Use `react-ui-agent` for `src/typescript/react/src` UI, state, and styling work.
- Use `react-agent` only when React and API/runtime helpers need coordination.
- Use Bun-first workflows in `src/typescript/react`; do not replace them with npm in frontend app paths.
- Keep API access centralized and typed, and preserve `VITE_BANANA_API_BASE_URL` as the React runtime contract.
- Maintain compatibility with the compose-based local runtime described in `README.md` and `docker-compose.yml`.
- Prefer minimal, typed UI changes and avoid introducing frontend dependencies without clear need.
- If a frontend change affects backend payload shape, document the contract assumption and coordinate with API tests or prompts.

## Shared Frontend Contract

- If a task touches src/typescript/react or src/typescript/shared/ui, keep shared primitives in @banana/ui instead of app-local thin re-export stubs.
- Reuse @banana/ui/tailwind/preset and @banana/ui/styles/tokens.css from consuming apps.
- Install dependencies in src/typescript/shared/ui before running app-level bun check/build flows.
- Reference .github/shared-typescript-ui.md for the full contract.

## Runtime Contract Lessons (2026-04)

- For React container builds that consume `@banana/ui` via file dependency, keep `.dockerignore` excluding `**/node_modules` so host Windows symlinks do not overwrite Linux container installs.
- In `docker/react.Dockerfile`, copy shared UI package files before install and run `bun install --cwd /workspace/src/typescript/shared/ui` before app-level install so shared deps resolve at runtime.
- Base-image scanner highs on Debian slim may remain; mitigate drift by pinning immutable image digests and tracking residual base CVEs as platform risk when no direct runtime exploit path is introduced.

