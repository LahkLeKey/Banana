# @banana/panels

Reusable Banana panel primitives packaged as a standalone workspace library.

## Install

```bash
npm install @banana/panels
```

## Entry Points

- `@banana/panels` full panel surface
- `@banana/panels/resizable-panel`
- `@banana/panels/resizable-dock-grid`
- `@banana/panels/chrome`
- `@banana/panels/header`
- `@banana/panels/host`
- `@banana/panels/refs`
- `@banana/panels/styles`
- `@banana/panels/stage`
- `@banana/panels/interactions`
- `@banana/panels/view-model`
- `@banana/panels/resize`
- `@banana/panels/drag-snap`
- `@banana/panels/raf-buffered-dispatch`

## Monorepo Workspace

This package is part of the `src/typescript` workspaces root so it can be versioned and published independently.

The current frontend in `src/typescript/shared/ui` consumes this package through thin compatibility wrappers while the implementation source lives in `src/typescript/packages/panels/src/internal`.

From repository root, bootstrap workspaces with:

```bash
bun install
```

## Module Packages

Module-level public wrapper packages live under `src/typescript/packages/panels/modules/*`.

Each wrapper package:

- re-exports one module from `@banana/panels`
- has its own `tailwindcss` dependency for primitive orchestration
- can be published independently

## Standalone Package Environment

This package ships with a local env template:

```bash
cp .env.example .env
```

The env file is package-local so this package can run Storybook independently from other workspaces.

## Storybook

Run Storybook from this package directory:

```bash
bun run storybook
```

Build a static Storybook bundle:

```bash
bun run build-storybook
```

Default port is `6006`. To run multiple package Storybooks simultaneously, pass a custom port:

```bash
bun run storybook -- -p 6010
```

## Release Automation

- Pack all panel packages into tarballs:

```bash
bun run pack:panels
```

- Dry-run npm publish across all panel packages:

```bash
bun run publish:panels:dry-run
```

- CI workflow for packing/publishing:
	- `.github/workflows/panels-packages-release.yml`
