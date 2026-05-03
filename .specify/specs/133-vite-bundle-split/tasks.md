# Tasks: Vite Bundle Split and Lazy Routes (133)

**Input**: Spec at `.specify/specs/133-vite-bundle-split/spec.md`
**Prerequisites**: Spike 119 complete with bundle-split recommendation

- [ ] T001 Wrap `ClassifyPage`, `OperatorPage`, and `NotFoundPage` imports in `React.lazy()` in the router file; add a `<Suspense fallback={<PageSpinner />}>` wrapper.
- [ ] T002 Add `build.rollupOptions.output.manualChunks` to `vite.config.ts` extracting `react`, `react-dom`, `@radix-ui`, and `lucide-react` into a `vendor` chunk.
- [ ] T003 Run `cd src/typescript/react && bun run build` and record the initial JS transfer size from the build output; confirm it is ≥ 15% smaller than the baseline.
- [ ] T004 Run `bun test --cwd src/typescript/react` and confirm 62 tests pass.
- [ ] T005 Run `bunx tsc --noEmit --cwd src/typescript/react` and confirm no type errors.
- [ ] T006 Run `python scripts/validate-spec-tasks-parity.py .specify/specs/133-vite-bundle-split` and confirm `OK`.
