# Tasks: Shared UI Tokens + Theme Primitives

**Branch**: `026-shared-ui-tokens-theme-primitives` | **Created**: 2026-04-26
**Status**: GATED. Foundational; slices 027 + 028 hard-depend on this.

## Phase 1 -- Setup

- [x] T001 Repoint `.specify/feature.json` -> `026-shared-ui-tokens-theme-primitives`.
- [x] T002 Author `README.md` execution tracker.

## Phase 2 -- Token TS module

- [x] T003 Author `src/typescript/shared/ui/src/tokens/index.ts`
  exporting `tokens` per the SPIKE 021 token contract.
- [x] T004 Author `src/typescript/shared/ui/src/tokens/README.md`
  documenting how to add a token.

## Phase 3 -- Generation script

- [x] T005 Author `src/typescript/shared/ui/scripts/generate-web-tokens.ts`
  emitting `tokens/web.css` from `tokens/index.ts`.
- [x] T006 Add `tokens:gen` script to
  `src/typescript/shared/ui/package.json`.
- [x] T007 Run generation; commit `tokens/web.css`.

## Phase 4 -- Tailwind + native re-export

- [x] T008 Extend `src/typescript/shared/ui/tailwind.config.ts` to
  map semantic colors onto CSS variables.
- [x] T009 Re-export tokens from
  `src/typescript/shared/ui/src/index.ts` and create
  `src/typescript/shared/ui/src/native/tokens.ts` re-export for
  `@banana/ui/native/tokens`.

## Phase 5 -- Lock test + validation

- [x] T010 Add `src/typescript/shared/ui/tokens.test.ts` snapshot
  locking the generated CSS.
- [x] T011 Run validation lane
  (`bun run tokens:gen`,
  `git diff --exit-code src/typescript/shared/ui/src/tokens/web.css`,
  `bun run --cwd src/typescript/shared/ui tsc --noEmit`,
  `bun run --cwd src/typescript/shared/ui test`).

## Phase 6 -- Close-out

- [x] T012 Update README to COMPLETE; mark all tasks `[x]`.

## Out of scope

- Component refactor (slice 027).
- Per-channel adoption (slice 028).
- Dark mode (D-021-01).
