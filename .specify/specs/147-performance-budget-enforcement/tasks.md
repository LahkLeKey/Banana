# Tasks: Performance Budget Enforcement (147)

**Input**: Spec at `.specify/specs/147-performance-budget-enforcement/spec.md`
**Prerequisites**: Spec 119 (bundle analysis), Spec 136 (Lighthouse CI)

- [ ] T001 Create `bundlebudget.json` with budgets: main JS < 150 KB, CSS < 50 KB, LCP < 2.5s, CLS < 0.1.
- [ ] T002 Create `scripts/check-bundle-budget.py` to read bundlebudget.json and compare against current build output.
- [ ] T003 Test script locally: `python scripts/check-bundle-budget.py` should exit 0 if under budget.
- [ ] T004 Integrate into CI: add job in `.github/workflows/vercel-deploy.yml` to run budget check before deploy.
- [ ] T005 Document budget justification and adjustment process in `docs/performance-budgets.md`.
- [ ] T006 Run `python scripts/validate-spec-tasks-parity.py .specify/specs/147-performance-budget-enforcement` and confirm `OK`.
