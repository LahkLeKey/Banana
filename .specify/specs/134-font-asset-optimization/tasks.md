# Tasks: Font and Asset Loading Optimization (134)

**Input**: Spec at `.specify/specs/134-font-asset-optimization/spec.md`
**Prerequisites**: Spike 119 complete; baseline Lighthouse scores recorded

- [ ] T001 Audit `src/typescript/react/src/index.css` and `tailwind.config.ts` for `@import url("fonts.googleapis.com/...")` or similar external font loads.
- [ ] T002 If external fonts are found, install self-hosted equivalents via `bun add @fontsource/<font-name>` and replace the CDN import with the fontsource import in `index.css`.
- [ ] T003 Add `font-display: swap` to all `@font-face` declarations in `index.css`.
- [ ] T004 Run `cd src/typescript/react && bun run build` and confirm no external font CDN references in `dist/`.
- [ ] T005 Run Lighthouse on the Vercel preview and confirm "Eliminate render-blocking resources" warning is resolved for fonts.
- [ ] T006 Run `python scripts/validate-spec-tasks-parity.py .specify/specs/134-font-asset-optimization` and confirm `OK`.
