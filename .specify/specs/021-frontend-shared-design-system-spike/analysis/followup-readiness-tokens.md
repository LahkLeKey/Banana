# 021 -- Followup Readiness: Slice 026 (Tokens + Theme Primitives)

Bounded scope packet for the foundational design-system slice.

## Goal

Land the token contract from
[token-contract.md](./token-contract.md) and the theming strategy
from [theming-strategy.md](./theming-strategy.md) in
`src/typescript/shared/ui` WITHOUT changing any component behavior.
This slice is the contract; component refactor is slice 027.

## In-scope files

- `src/typescript/shared/ui/src/tokens/index.ts` (NEW; the contract)
- `src/typescript/shared/ui/src/tokens/web.css` (NEW; generated)
- `src/typescript/shared/ui/src/tokens/README.md` (NEW)
- `src/typescript/shared/ui/scripts/generate-web-tokens.ts` (NEW;
  bun-runnable)
- `src/typescript/shared/ui/package.json` (add `tokens:gen` script)
- `src/typescript/shared/ui/tailwind.config.ts` (extend theme to
  reference CSS variables)
- `src/typescript/shared/ui/src/index.ts` (re-export tokens)
- `src/typescript/shared/ui/src/native/index.ts` (NEW or extend;
  re-export tokens for `@banana/ui/native/tokens` import path)
- `src/typescript/shared/ui/tokens.test.ts` (NEW; vitest snapshot of
  the generated CSS to lock the contract)

## Out of scope

- Component refactor (slice 027).
- Per-channel adoption (slice 028).
- Dark mode tokens.
- Storybook / Figma integration (deferred with trigger; see
  deferred-registry).

## Validation lane

```
bun run --cwd src/typescript/shared/ui tokens:gen
git diff --exit-code src/typescript/shared/ui/src/tokens/web.css
bun run --cwd src/typescript/shared/ui tsc --noEmit
bun run --cwd src/typescript/shared/ui test
```

## Estimated task count

10-12 tasks. Buckets: setup (1) + token TS module (2) + generation
script (2) + Tailwind wiring (1) + native re-export (1) + snapshot
test (1) + README (1) + close-out (2).

## Acceptance

- `tokens.gen` is idempotent: running twice leaves `web.css`
  unchanged.
- Tailwind classes that reference token-mapped names compile.
- Existing components (`BananaBadge`, `RipenessLabel`,
  `ChatMessageBubble`) still render byte-identical (no consumer
  changes in this slice).
- Snapshot test fails loudly if the token contract changes
  unintentionally.
