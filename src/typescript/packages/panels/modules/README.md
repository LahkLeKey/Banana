# Panels Module Packages

This folder contains publish-ready thin wrapper packages for individual panel modules.

Each module package:

- Depends on `@banana/panels` for implementation source.
- Declares its own `tailwindcss` dependency for primitive orchestration.
- Can be published independently for package-manager installs.

Examples:

- `@banana/panels-resizable-panel`
- `@banana/panels-resizable-dock-grid`
- `@banana/panels-styles`
- `@banana/panels-interactions`
- `@banana/panels-panel-base`
- `@banana/panels-panel-overlay`
- `@banana/panels-panel-pipeline`
- `@banana/panels-panel-variant-pipeline`
- `@banana/panels-dock-layout-store`

## Standalone Env + Storybook Per Module

Each module package includes:

- `.env.example` for package-local runtime flags
- `.storybook/main.ts`
- `.storybook/preview.ts`
- `src/stories/__package.stories.tsx`

From any module package directory (for example `modules/panel-base`):

```bash
cp .env.example .env
bun run storybook
```

Build static Storybook output for a module package:

```bash
bun run build-storybook
```

By default Storybook uses port `6006`. Use a custom port when running multiple module packages at once:

```bash
bun run storybook -- -p 6011
```
