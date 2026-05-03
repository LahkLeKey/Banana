# Feature Specification: Lighthouse CI Gate (136)

**Feature Branch**: `feature/136-lighthouse-ci-gate`
**Created**: 2026-05-02
**Status**: Draft (pending spike 119 go verdict)
**Input**: Scaffolded from spike `119-performance-cdn-spike`

## In Scope

- Add a Lighthouse CI step to `.github/workflows/vercel-deploy.yml` that runs against the Vercel preview URL after each PR deploy.
- Fail the CI job if LCP > 2.5 s or CLS > 0.1 on the mobile-throttled Lighthouse run.
- Upload the Lighthouse HTML report as a GitHub Actions artifact.
- Record the baseline scores from `banana.engineer` in `docs/lighthouse-baseline.md`.

## Out of Scope

- Real-user monitoring (spec 140).
- Backend performance profiling.

## Success Criteria

```bash
ls .github/workflows/vercel-deploy.yml
grep "lhci" .github/workflows/vercel-deploy.yml
python scripts/validate-spec-tasks-parity.py .specify/specs/136-lighthouse-ci-gate
```
