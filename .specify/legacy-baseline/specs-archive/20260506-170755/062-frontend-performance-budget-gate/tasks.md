# Tasks — 062 Frontend Performance Budget Gate

- [x] T001 — Add `bundlewatch` or `vite-bundle-visualizer` to track bundle size
- [x] T002 — Set budget: main JS chunk ≤ 250 KB gzipped, CSS ≤ 50 KB gzipped
- [x] T003 — Add CI step that runs `bun run build` and checks bundle sizes
- [x] T004 — Fail CI if any budget is exceeded by more than 10%
- [x] T005 — Configure code-splitting at route level with `lazy()` for OperatorPage
- [x] T006 — Report final sizes in CI summary output
