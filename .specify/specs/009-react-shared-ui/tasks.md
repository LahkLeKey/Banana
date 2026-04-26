---
description: "Tasks for v2 React app + shared UI regeneration"
---

# Tasks: React App + Shared UI (v2)

## Phase 0 — Inventory

- [ ] T001 List all imports from `@banana/ui` and where they originate (deep vs root).
- [ ] T002 List all hardcoded API base URLs.

## Phase 1 — Public surface

- [ ] T010 Create `@banana/ui/src/index.ts` with explicit re-exports.
- [ ] T011 [P] Add ESLint rule (or import boundary) to forbid deep imports.
- [ ] T012 Add `@banana/ui/README.md` with public surface + platform labels.

## Phase 2 — Config dedup

- [ ] T020 Remove `tailwind.config.{js,d.ts}` siblings; keep only `.ts`.
- [ ] T021 Remove `vite.config.{js,d.ts}` siblings; keep only `.ts`.
- [ ] T022 Update tsconfigs to compile only canonical configs.

## Phase 3 — API client

- [ ] T030 Centralize HTTP client in `src/lib/api.ts` reading `VITE_BANANA_API_BASE_URL`.
- [ ] T031 [P] Replace hardcoded URLs from inventory.

## Phase 4 — RN reuse labels

- [ ] T040 Annotate components in `@banana/ui` index with `// platform: web | cross`.
- [ ] T041 Document RN-safe subset for `011-react-native-mobile`.

## Dependencies

- T010 unblocks T011/T012.
- T030 should follow T001 inventory.
