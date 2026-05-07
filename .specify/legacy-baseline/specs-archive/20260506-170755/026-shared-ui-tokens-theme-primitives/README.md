# 026 Shared UI Tokens + Theme Primitives -- Execution Tracker

**Branch**: `026-shared-ui-tokens-theme-primitives`
**Source SPIKE**: [021 Frontend Shared Design System](../021-frontend-shared-design-system-spike/spec.md)
**Status**: COMPLETE (2026-04-26).

## Outcome

- TypeScript token source of truth landed at
  [src/tokens/index.ts](../../../src/typescript/shared/ui/src/tokens/index.ts)
  per the SPIKE 021 contract (color, font, space, radius, motion).
- Authoring guide added at
  [src/tokens/README.md](../../../src/typescript/shared/ui/src/tokens/README.md).
- Generator script at
  [scripts/generate-web-tokens.ts](../../../src/typescript/shared/ui/scripts/generate-web-tokens.ts)
  emits `:root { --color-banana-bg: ...; ... }` to
  [src/tokens/web.css](../../../src/typescript/shared/ui/src/tokens/web.css).
  Idempotent: re-run prints `web.css already up to date`.
- `bun run --cwd src/typescript/shared/ui tokens:gen` script wired in
  [package.json](../../../src/typescript/shared/ui/package.json).
- [tailwind.config.ts](../../../src/typescript/shared/ui/tailwind.config.ts)
  extends `theme.colors`, `borderRadius`, `transitionDuration` to
  reference the generated CSS variables. Existing class consumers
  (e.g. slice 015 `BananaBadge`) are untouched (FR-026-05) -- nothing
  in `src/components/**` was modified.
- Tokens re-exported via:
  - `@banana/ui` (web) -- [src/index.ts](../../../src/typescript/shared/ui/src/index.ts)
  - `@banana/ui/native/tokens` (native) -- [src/native/tokens.ts](../../../src/typescript/shared/ui/src/native/tokens.ts)
- Snapshot test at
  [tokens.test.ts](../../../src/typescript/shared/ui/tokens.test.ts)
  asserts the contract surface and locks the generated CSS shape.

## Validation lane

```
bun run --cwd src/typescript/shared/ui tokens:gen     # idempotent
git diff --exit-code src/typescript/shared/ui/src/tokens/web.css
bun run --cwd src/typescript/shared/ui tsc --noEmit   # clean
bun run --cwd src/typescript/shared/ui test           # 7 pass / 0 fail
bun run --cwd src/typescript/react test               # 25 pass / 0 fail (no consumer regression)
```

## Constraints

- No component refactor (slice 027).
- No per-channel adoption (slice 028).
- No dark mode (D-021-01).
- No Storybook / Figma (D-021-02 / D-021-03).
