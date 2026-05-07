# Feature Specification: Vercel Environment Variable Contract (124)

**Feature Branch**: `feature/124-vercel-env-var-contract`
**Created**: 2026-05-02
**Status**: Draft (pending spike 116 go verdict)
**Input**: Scaffolded from spike `116-vercel-react-deploy-spike`
## Problem Statement

Feature Specification: Vercel Environment Variable Contract (124) aims to improve system capability and user experience by implementing the feature described in the specification.


## In Scope

- Define and document all required `VITE_*` environment variables for the React app in a canonical `.env.example` file.
- Confirm each variable is set correctly per Vercel environment tier (Development, Preview, Production).
- Add a CI validation step that fails the build if any required `VITE_*` variable is unset.
- Update `docs/developer-onboarding.md` to include local `.env.local` setup instructions.

## Out of Scope

- API-side environment variables (spec 128).
- Secrets rotation runbook (spec 132).

## Success Criteria

```bash
grep "VITE_BANANA_API_BASE_URL" src/typescript/react/.env.example
grep "VITE_" docs/developer-onboarding.md
python scripts/validate-spec-tasks-parity.py .specify/specs/124-vercel-env-var-contract
```
