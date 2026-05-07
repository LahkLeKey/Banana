# Tasks: Vite Bundle Split and Lazy Routes (133)

**Input**: Spec at `.specify/specs/133-vite-bundle-split/spec.md`
**Prerequisites**: Spike 119 complete with bundle-split recommendation

- [x] T001 Wrap `ClassifyPage`, `OperatorPage`, and `NotFoundPage` imports in `React.lazy()` in the router file; add a `<Suspense fallback={<PageSpinner />}>` wrapper.
- [x] T002 Add `build.rollupOptions.output.manualChunks` to `vite.config.ts` extracting `react`, `react-dom`, `@radix-ui`, and `lucide-react` into a `vendor` chunk.
- [x] T003 Run `cd src/typescript/react && bun run build` and record the initial JS transfer size from the build output; confirm it is ≥ 15% smaller than the baseline.
- [x] T004 Run `bun test --cwd src/typescript/react` and confirm 62 tests pass.
- [x] T005 Run `bunx tsc --noEmit --cwd src/typescript/react` and confirm no type errors.
- [x] T006 Run `python scripts/validate-spec-tasks-parity.py .specify/specs/133-vite-bundle-split` and confirm `OK`.
