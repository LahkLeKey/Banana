# Feature Specification: Vite Bundle Split and Lazy Routes (133)

**Feature Branch**: `feature/133-vite-bundle-split`
**Created**: 2026-05-02
**Status**: Draft (pending spike 119 go verdict)
**Input**: Scaffolded from spike `119-performance-cdn-spike`

## In Scope

- Implement `React.lazy` + `Suspense` for all page-level routes (`ClassifyPage`, `OperatorPage`, `NotFoundPage`).
- Add `build.rollupOptions.output.manualChunks` in `vite.config.ts` to extract `react`, `react-dom`, and `@radix-ui` into a `vendor` chunk.
- Measure and document the before/after initial JS transfer size.
- Confirm `bun run build` and `bun test` still exit 0.

## Out of Scope

- Font optimization (spec 134). Image optimization (spec 135). Lighthouse CI (spec 136).

## Success Criteria

```bash
cd src/typescript/react && bun run build
bun test
python scripts/validate-spec-tasks-parity.py .specify/specs/133-vite-bundle-split
```
