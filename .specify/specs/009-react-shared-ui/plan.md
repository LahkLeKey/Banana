# Implementation Plan: React App + Shared UI (v2)

**Branch**: `009-react-shared-ui` | **Date**: 2026-04-25 | **Spec**: [spec.md](./spec.md)

## Technical Context

**Language/Version**: TypeScript / React 18+.  
**Primary Dependencies**: Vite, Tailwind, `@banana/ui` (file dep).  
**Testing**: Vitest or Bun test.  
**Target Platform**: Browser; consumed by Electron renderer (`010`).  
**Project Type**: SPA + shared UI library.  
**Constraints**: Bun PM, single canonical configs, documented `@banana/ui` API.

## Project Structure

```text
src/typescript/
├── shared/
│   └── ui/
│       ├── package.json        # name: "@banana/ui"
│       ├── src/
│       │   └── index.ts        # public re-exports only
│       └── README.md           # public surface + cross-platform labels
└── react/
    ├── package.json            # depends on file:../shared/ui
    ├── tailwind.config.ts      # canonical
    ├── vite.config.ts          # canonical
    └── src/
        ├── main.tsx
        ├── App.tsx
        ├── lib/
        │   └── api.ts          # reads VITE_BANANA_API_BASE_URL
        └── types/              # local-only types; shared types come from contracts
```

## Phasing

- **Phase 0**: Inventory current shared exports + their consumers.
- **Phase 1**: Add `@banana/ui` `index.ts`; lint deep imports.
- **Phase 2**: Delete duplicate config files, keep canonical `.ts`.
- **Phase 3**: Centralize API client; remove hardcoded URLs.
- **Phase 4**: Label cross-platform components for RN reuse.
