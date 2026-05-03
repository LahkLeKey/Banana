# Feature Specification: Vercel CI Deploy Workflow (122)

**Feature Branch**: `feature/122-vercel-ci-deploy-workflow`
**Created**: 2026-05-02
**Status**: Draft (pending spike 116 go verdict)
**Input**: Scaffolded from spike `116-vercel-react-deploy-spike`
## Problem Statement

Feature Specification: Vercel CI Deploy Workflow (122) aims to improve system capability and user experience by implementing the feature described in the specification.


## In Scope

- Create `.github/workflows/vercel-deploy.yml` that triggers on `push` to `main` and calls the Vercel CLI to deploy to production.
- Gate deployment on `bun run build` exit 0 and all required status checks passing.
- Pass `VERCEL_TOKEN`, `VERCEL_ORG_ID`, and `VERCEL_PROJECT_ID` as GitHub Actions secrets.
- Skip bot commits (same guard as `screenshot-ci.yml`).

## Out of Scope

- Preview deployment workflow (spec 129).
- Domain DNS (spec 123).
- Storybook deploy (already spec 115).

## Success Criteria

```bash
ls .github/workflows/vercel-deploy.yml
python scripts/validate-spec-tasks-parity.py .specify/specs/122-vercel-ci-deploy-workflow
```
