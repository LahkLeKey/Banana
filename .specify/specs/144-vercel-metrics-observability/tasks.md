# Tasks: Vercel Metrics API and Observability (144)

**Input**: Spec at `.specify/specs/144-vercel-metrics-observability/spec.md`
**Prerequisites**: Spec 140 (CWV dashboard active); real traffic for 7+ days

- [ ] T001 Explore `vercel metrics` CLI: `vercel metrics query --help` and document available metrics.
- [ ] T002 Query metrics: `vercel metrics query LCP --since 7d` and capture output.
- [ ] T003 Create `scripts/export-vercel-metrics.py` to query and export metrics as JSON.
- [ ] T004 Run export script and verify JSON structure matches Vercel API schema.
- [ ] T005 Create weekly report template in `docs/metrics-report.md` with key metrics and thresholds.
- [ ] T006 Run `python scripts/validate-spec-tasks-parity.py .specify/specs/144-vercel-metrics-observability` and confirm `OK`.
