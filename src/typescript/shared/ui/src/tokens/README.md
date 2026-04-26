# Tokens (slice 026)

This folder is the single source of truth for Banana design tokens
across web (React + Electron) and React Native.

## Files

- `index.ts` -- TypeScript object literal. Edit here to change a value.
- `web.css` -- generated CSS custom properties for web consumers.
  **Do not edit by hand.** Regenerate via `bun run tokens:gen`.
- `README.md` -- this file.

## Adding or changing a token

1. Edit `index.ts`.
2. If your token is web-relevant, run:
   ```sh
   bun run --cwd src/typescript/shared/ui tokens:gen
   ```
   This rewrites `web.css`. The generation is idempotent: re-running
   without source changes leaves the file untouched.
3. If you added a new semantic color, extend
   `src/typescript/shared/ui/tailwind.config.ts` to map the Tailwind
   theme key onto the new CSS variable.
4. Run the validation lane:
   ```sh
   bun run --cwd src/typescript/shared/ui tokens:gen
   git diff --exit-code src/typescript/shared/ui/src/tokens/web.css
   bun run --cwd src/typescript/shared/ui tsc --noEmit
   bun run --cwd src/typescript/shared/ui test
   ```

## Naming convention

`<group>.<scope>.<role>` with kebab-case CSS variables
(e.g. `tokens.color.banana.bg` -> `--color-banana-bg`).

The contract -- including which slices each token unblocks -- is
pinned at
[../../../../.specify/specs/021-frontend-shared-design-system-spike/analysis/token-contract.md](../../../../.specify/specs/021-frontend-shared-design-system-spike/analysis/token-contract.md).
