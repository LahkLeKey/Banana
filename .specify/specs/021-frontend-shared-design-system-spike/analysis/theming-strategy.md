# 021 -- Theming Strategy

How the token contract from `token-contract.md` reaches each channel.

## Decision: shared TS export + per-channel adapters

Single source of truth lives at:

```
src/typescript/shared/ui/src/tokens/index.ts
```

This file exports a `tokens` object literal whose shape is the token
contract (color / font / space / radius / motion). React Native
consumes this object directly. Web consumes it via a generated CSS
custom properties stylesheet.

## Per-channel adoption

### React (web) and Electron (renderer)

- Generated stylesheet: `src/typescript/shared/ui/src/tokens/web.css`
  exports `:root { --color-banana-bg: #fef3c7; ... }`.
- Components use CSS variables via Tailwind theme extension OR
  `style={{ backgroundColor: 'var(--color-banana-bg)' }}` for
  components that bypass Tailwind.
- Tailwind config in
  `src/typescript/shared/ui/tailwind.config.ts` extends `theme.colors`
  to map semantic names onto CSS variables, so existing Tailwind
  consumers (slice 015 BananaBadge) get token-driven colors without
  rewriting class names.
- Generation script: `bun run --cwd src/typescript/shared/ui tokens:gen`
  reads `tokens/index.ts` and writes `tokens/web.css`. Committed
  artifact -- no runtime generation.

### React Native

- Imports the `tokens` object directly:
  `import { tokens } from '@banana/ui/native/tokens';`
- Native components use `style={{ backgroundColor: tokens.color.banana.bg }}`.
- No CSS variable indirection (RN has no CSS).
- No generation step required.

## Why not StyleSheet on native + identical CSS on web?

Considered: emit `StyleSheet.create()` from the generation script for
RN. Rejected because the win is small (perf parity) and the cost is
adding a build step to the RN package. Direct object access keeps the
RN side dependency-free.

## Why not a runtime theme provider (Context)?

Considered: wrap each channel root in a `<ThemeProvider>` that injects
tokens via React context. Rejected for v1 because:

- It adds a runtime dependency that every component must opt into.
- Dark mode is deferred (no second token set yet).
- CSS variables on web give us free runtime swap if we ever need it.
- Native can re-introduce a provider in a follow-up SPIKE if dark mode
  lands and proves it needs runtime token swap.

## Output structure

```
src/typescript/shared/ui/src/tokens/
  index.ts        // single source of truth (TS object)
  web.css         // generated; committed
  README.md       // documents how to add a token
```

## Per-channel validation

- Web/Electron: visual byte-identity check that BananaBadge renders
  the same color before and after token migration (slice 028).
- Native: a snapshot of the `BananaBadge` style object pre/post
  migration must match the literal hex values from the inventory
  (no color drift during cutover).
