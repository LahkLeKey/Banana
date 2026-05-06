# Tasks — 060 Frontend E2E Playwright Harness

- [x] T001 — Add `@playwright/test` to `src/typescript/react` dev deps
- [x] T002 — Create `e2e/` directory with `playwright.config.ts` targeting `localhost:5173`
- [x] T003 — Write smoke test: load app, expect health check passes, classify page renders
- [x] T004 — Add CI job `frontend-e2e` that starts `bun run preview` then runs Playwright
- [x] T005 — Upload Playwright HTML report as CI artifact
- [x] T006 — Add `@axe-core/playwright` accessibility scan to smoke test
