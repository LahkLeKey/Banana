# Quickstart: Frontend shadcn Baseline Migration (048)

```bash
cd src/typescript/react
bun install
bunx tsc --noEmit
bun test src/App.test.tsx
bun run dev   # http://localhost:5173
```

Container path (preserved):

```bash
bash scripts/compose-run-profile.sh --profile apps --action up -- --build
bash scripts/compose-profile-ready.sh --profile apps
```

Expected:
- `bunx tsc --noEmit` reports 0 errors.
- `bun test src/App.test.tsx` reports >= 13 prior tests + new shadcn regression tests, all green.
- Manual smoke: clicking **Predict ensemble** updates the verdict card in place; forcing a network failure shows an inline `Alert` and preserves the prior verdict; the page never goes blank.
