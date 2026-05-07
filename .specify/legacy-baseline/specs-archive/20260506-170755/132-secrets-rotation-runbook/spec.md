# Feature Specification: Secrets Rotation Runbook (132)

**Feature Branch**: `feature/132-secrets-rotation-runbook`
**Created**: 2026-05-02
**Status**: Draft (pending spike 118 go verdict)
**Input**: Scaffolded from spike `118-staging-environment-spike`
## Problem Statement

Feature Specification: Secrets Rotation Runbook (132) aims to improve system capability and user experience by implementing the feature described in the specification.


## In Scope

- Document the rotation procedure for all production secrets: `BANANA_PG_CONNECTION`, `VERCEL_TOKEN`, `SENTRY_AUTH_TOKEN`, `BANANA_CORS_ORIGINS`.
- Define who owns each secret and the rotation cadence (quarterly or on-breach).
- Create `docs/secrets-rotation.md` with step-by-step rotation instructions per host (Vercel dashboard, API host, GitHub Actions secrets).
- Add a GitHub Actions `workflow_dispatch` job that reminds the team to rotate secrets on a quarterly schedule.

## Out of Scope

- Automated secrets rotation (Vault, AWS Secrets Manager) — out of scope for v1.
- Storing secrets in code or documentation.

## Success Criteria

```bash
ls docs/secrets-rotation.md
python scripts/validate-spec-tasks-parity.py .specify/specs/132-secrets-rotation-runbook
```
