# Legacy Baseline — React App + Shared UI

**Paths**: `src/typescript/react/`, `src/typescript/shared/ui/`

## What exists

- **React app** (`react/`): Vite + React + Tailwind, Bun-managed. Entry `src/main.tsx`, `App.tsx`, `lib/`, `types/`.
- **Shared UI** (`shared/ui/`): consumed by the React app and by Electron via file dependency.
- Configs: `vite.config.ts`, `tailwind.config.ts`, `postcss.config.js`, `tsconfig.{json,app,node}.json`.

## Hard contracts

- `VITE_BANANA_API_BASE_URL` is the API base URL contract for all HTTP calls.
- Bun is the package manager.
- For container builds, `.dockerignore` must exclude `**/node_modules` so host Windows symlinks do not overwrite Linux container installs (see `/memories/repo/typescript-shared-ui.md`).
- `docker/react.Dockerfile` copies shared UI package files before install and runs `bun install --cwd /workspace/src/typescript/shared/ui` before app-level install.

## What works

- Vite dev/build is fast.
- Shared UI as a file dep means a single source of components for web + desktop.
- Tailwind classes are consistent across components.

## Vibe drift / pain points

- `tailwind.config.{js,ts,d.ts}` triplicate creates ambiguity about the source of truth.
- `vite.config.{js,ts,d.ts}` similar duplication.
- Shared UI lacks a documented public API surface — consumers reach into deep paths.
- No story/visual harness for shared components.
- Type definitions in `types/` overlap with API contract types — divergence risk.

## Cross-domain dependencies

- Consumes: `007-aspnet-pipeline` HTTP and/or `008-typescript-api`.
- Consumed by: `010-electron-desktop` (renderer), `011-react-native-mobile` (selected primitives only — RN cannot consume web Tailwind directly).

## v1 entry points to preserve in v2

- `VITE_BANANA_API_BASE_URL`
- Bun + Vite toolchain
- Shared UI package name `@banana/ui`
