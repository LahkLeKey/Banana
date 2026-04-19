# Shared TypeScript UI Contract

Use this contract whenever work touches Banana frontend code in src/typescript/react, src/typescript/electron, or src/typescript/shared/ui.

## Source Of Truth

- Keep shared UI primitives, Tailwind preset, and token CSS in @banana/ui under src/typescript/shared/ui.
- Do not keep or reintroduce app-local thin re-export stubs for shared UI components.
- React and Electron renderer should import shared primitives directly from @banana/ui.

## Tailwind And Styles

- Import @banana/ui/tailwind/preset from app tailwind.config.ts files.
- Include ./node_modules/@banana/ui/src/**/*.{ts,tsx} in Tailwind content globs for consuming apps.
- Import @banana/ui/styles/tokens.css in app index.css before Tailwind layers.

## Dependency And Build Flow

- Install dependencies in src/typescript/shared/ui before running app-level Bun check/build scripts.
- Keep explicit package exports for consumed @banana/ui subpaths.
- Keep shared package dependencies in src/typescript/shared/ui/package.json so linked source resolution stays stable.

## Validation

- For React: bun run check and bun run build in src/typescript/react.
- For Electron renderer: bun run check and bun run build in src/typescript/electron/renderer.
- If shared package contracts changed, rerun validation in both apps.
