# Contract — Shared UI Public Surface

## Package

- Name: `@banana/ui`.
- Consumed via file dependency by `src/typescript/react` and `src/typescript/electron`.
- Selectively consumed (cross-platform exports only) by `src/typescript/react-native` (`011`).

## Public API

- The package exposes exactly what's re-exported from `src/index.ts`.
- Deep imports (`@banana/ui/src/...`, `@banana/ui/components/...`) are forbidden by lint.
- Each export is annotated as `web` or `cross` in the package README; `cross` exports must not import web-only modules transitively.

## Configuration

- One canonical `tailwind.config.ts` per package. No `.js` or `.d.ts` siblings committed.
- One canonical `vite.config.ts` per package. Same rule.

## Container build invariants

- Repo `.dockerignore` excludes `**/node_modules`.
- `docker/react.Dockerfile` copies shared UI package files first, runs `bun install --cwd /workspace/src/typescript/shared/ui`, then installs the React app.

## API URL

- The web app reads `VITE_BANANA_API_BASE_URL`. No hardcoded base URLs anywhere in source.
