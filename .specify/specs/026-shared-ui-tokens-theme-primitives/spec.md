# Feature Specification: Shared UI Tokens + Theme Primitives

**Feature Branch**: `[026-shared-ui-tokens-theme-primitives]`
**Created**: 2026-04-26
**Status**: GATED. Foundational for slices 027 + 028.
**Source**: [../021-frontend-shared-design-system-spike/analysis/followup-readiness-tokens.md](../021-frontend-shared-design-system-spike/analysis/followup-readiness-tokens.md)

## Goal

Land the token contract from
[../021-.../analysis/token-contract.md](../021-frontend-shared-design-system-spike/analysis/token-contract.md)
and the theming strategy from
[../021-.../analysis/theming-strategy.md](../021-frontend-shared-design-system-spike/analysis/theming-strategy.md)
in `src/typescript/shared/ui` WITHOUT changing any component
behavior. This slice ships the contract; component refactor is
slice 027.

## Functional requirements

- **FR-026-01**: `src/typescript/shared/ui/src/tokens/index.ts` is
  the single source of truth for color / typography / spacing /
  radius / motion tokens.
- **FR-026-02**: `bun run --cwd src/typescript/shared/ui tokens:gen`
  generates `tokens/web.css` with `:root` CSS custom properties.
  Generation is idempotent: re-running leaves the file unchanged.
- **FR-026-03**: Tailwind config extends `theme.colors` (etc.) to
  reference the CSS variables, so existing Tailwind class consumers
  resolve through tokens.
- **FR-026-04**: Tokens are re-exported via `@banana/ui` (web) and
  `@banana/ui/native/tokens` (native) entry points.
- **FR-026-05**: Existing components (`BananaBadge`,
  `RipenessLabel`, `ChatMessageBubble`) render byte-identical
  before/after this slice. No consumer changes.

## Out of scope

- Component refactor (slice 027).
- Per-channel adoption (slice 028).
- Dark mode tokens (D-021-01).
- Storybook / Figma (D-021-02 / D-021-03).

## Validation lane

```
bun run --cwd src/typescript/shared/ui tokens:gen
git diff --exit-code src/typescript/shared/ui/src/tokens/web.css
bun run --cwd src/typescript/shared/ui tsc --noEmit
bun run --cwd src/typescript/shared/ui test
```

## Success criteria

- Generation idempotent (git diff exit 0 after second run).
- Snapshot test in `tokens.test.ts` locks the contract.
- Existing components unchanged.

## In-scope files

See [readiness packet](../021-frontend-shared-design-system-spike/analysis/followup-readiness-tokens.md#in-scope-files).
