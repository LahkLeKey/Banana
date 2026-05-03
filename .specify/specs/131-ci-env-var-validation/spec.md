# Feature Specification: CI Environment Variable Validation Gate (131)

**Feature Branch**: `feature/131-ci-env-var-validation`
**Created**: 2026-05-02
**Status**: Draft (pending spike 118 go verdict)
**Input**: Scaffolded from spike `118-staging-environment-spike`

## In Scope

- Add a Vite build-time check (in `vite.config.ts`) that throws a descriptive error if required `VITE_*` variables are missing or empty during production builds.
- Add a CI step in `.github/workflows/vercel-deploy.yml` that validates required env vars are present before the build runs.
- Document all required variables in `src/typescript/react/.env.example` (spec 124 delivers the file; this spec adds the CI gate).

## Out of Scope

- Secrets rotation (spec 132).
- API env vars (spec 124 covers VITE_ vars only).

## Success Criteria

```bash
VITE_BANANA_API_BASE_URL="" bun run build --cwd src/typescript/react && echo "FAIL: should have thrown" || echo "PASS: build rejected empty var"
python scripts/validate-spec-tasks-parity.py .specify/specs/131-ci-env-var-validation
```
