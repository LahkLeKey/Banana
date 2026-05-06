# Tasks: Real-User CWV Reporting Dashboard (140)

**Input**: Spec at `.specify/specs/140-cwv-reporting-dashboard/spec.md`
**Prerequisites**: Spec 137 (`<SpeedInsights />` installed); `banana.engineer` live with real traffic

- [x] T001 Confirm `<SpeedInsights />` from `@vercel/speed-insights/react` is rendered in `src/typescript/react/src/main.tsx` (from spec 137).
- [x] T002 After production deploy, navigate to Vercel Dashboard → Speed Insights and confirm LCP, CLS, and INP panels show data (may take 24–48 h of real traffic).
- [x] T003 Create `docs/runbooks/performance-review.md` with: weekly review steps, how to read the CWV dashboard, and a definition of the regression threshold (P75 LCP > 3 s).
- [x] T004 Add a `workflow_dispatch` GitHub Actions job to `.github/workflows/` called `open-cwv-regression-issue.yml` that manually creates a GitHub issue titled "CWV Regression: P75 LCP exceeded 3 s" with the review runbook link.
- [x] T005 Append the first-week CWV baseline snapshot to `docs/lighthouse-baseline.md`.
- [x] T006 Run `python scripts/validate-spec-tasks-parity.py .specify/specs/140-cwv-reporting-dashboard` and confirm `OK`.
