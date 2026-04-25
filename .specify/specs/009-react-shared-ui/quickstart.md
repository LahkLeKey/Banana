# Quickstart — React App + Shared UI (v2)

## Prerequisites

- Bun.
- API reachable at `VITE_BANANA_API_BASE_URL`.

## Install

```bash
cd src/typescript/shared/ui && bun install
cd ../../react && bun install
```

## Dev

```bash
cd src/typescript/react
VITE_BANANA_API_BASE_URL=http://localhost:5000 bun run dev
```

## Build

```bash
cd src/typescript/react
bun run build
```

## Container build note

Repo `.dockerignore` excludes `**/node_modules` so host (Windows) symlinks
don't overwrite the Linux container install. Do not weaken this rule.

## Scope guardrails

- Don't import from `@banana/ui/src/...`. Import from `@banana/ui` only.
- Don't add `tailwind.config.js` or `.d.ts` siblings. Keep one canonical `.ts`.
- Don't hardcode API base URLs.
