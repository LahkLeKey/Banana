# Tasks: Performance and CDN Spike (119)

**Input**: Spec at `.specify/specs/119-performance-cdn-spike/spec.md`
**Prerequisites**: Spike 116 go verdict; preview deployment URL available for measurement

## Phase 1: Baseline Measurement

- [ ] T001 Run Lighthouse (mobile, simulated throttling) against the Vercel preview URL and record LCP, CLS, INP, TTFB, and Total Blocking Time in `research.md`.
- [ ] T002 Run `curl -I <preview-url>/assets/index-*.js` and confirm `Cache-Control: public, max-age=31536000, immutable` is returned.
- [x] T003 Inspect the `dist/` bundle using `bunx vite-bundle-visualizer` (or `rollup-plugin-visualizer`) and identify the top-5 largest chunks by gzip size.

## Phase 2: Code-Split Evaluation

- [x] T004 Evaluate React lazy-loading for each route (`ClassifyPage`, `OperatorPage`, `NotFoundPage`) using `React.lazy` + `Suspense` and measure the reduction in initial JS transfer.
- [x] T005 Evaluate `build.rollupOptions.output.manualChunks` in `vite.config.ts` to extract `react`, `react-dom`, and `@radix-ui` into separate vendor chunks and measure the impact on cache-hit rate.

## Phase 3: Font and Asset Optimization

- [x] T006 Identify any render-blocking font loads (Google Fonts, system font stack) in the current build and evaluate `font-display: swap` or local font subsetting.
- [x] T007 Evaluate Vercel Image Optimization (`/_vercel/image`) for user-submitted image URLs in the classify flow and note any React component changes required.

## Phase 4: Research Documentation

- [x] T008 Write completed `research.md` with: baseline Lighthouse scores, top optimization opportunities ranked by effort vs impact, recommended bundle-split config, and open questions for specs 133–136.
- [x] T009 Update `.specify/specs/133-vite-bundle-split/spec.md` status to "Ready for implementation" if the split delivers > 20% initial-JS reduction.
- [x] T010 Run `python scripts/validate-spec-tasks-parity.py .specify/specs/119-performance-cdn-spike` and confirm `OK`.
