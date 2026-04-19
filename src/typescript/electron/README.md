# Banana Electron App

Banana Desktop runtime with two surfaces:

- CLI smoke flow (`npm run smoke`) used by compose and CI.
- Desktop renderer flow powered by Bun + React + shadcn + Tailwind in `renderer/`.

## Renderer Setup (Bun)

```bash
cd src/typescript/shared/ui
bun install

cd src/typescript/electron/renderer
bun install
bun run dev
```

Then launch Electron against the dev server:

```bash
cd src/typescript/electron
BANANA_ELECTRON_RENDERER_URL=http://127.0.0.1:4173 npm run start
```

## Build Desktop Renderer

```bash
cd src/typescript/shared/ui
bun install

cd src/typescript/electron
bun run renderer:build
npm run start
```

`main.js` loads `renderer/dist/index.html` by default and shows a guidance page if the renderer has not been built.

## Shared UI Suite

- Shared renderer/ui primitives are provided by `@banana/ui` from `src/typescript/shared/ui`.
- Renderer Tailwind config consumes `@banana/ui/tailwind/preset` and scans `./node_modules/@banana/ui/src/**/*.{ts,tsx}`.
- Renderer global CSS imports `@banana/ui/styles/tokens.css`.

## Existing Smoke Flow

The existing smoke flow remains unchanged and still runs through Node scripts:

```bash
cd src/typescript/electron
npm install
npm run smoke
```
