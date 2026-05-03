# Feature Specification: Vercel Preview Deploys Per Branch (129)

**Feature Branch**: `feature/129-vercel-preview-deploys`
**Created**: 2026-05-02
**Status**: Draft (pending spike 118 go verdict)
**Input**: Scaffolded from spike `118-staging-environment-spike`

## In Scope

- Enable automatic Vercel preview deployments for every pull request that touches `src/typescript/react/**`.
- Confirm each preview URL uses the Preview-tier `VITE_BANANA_API_BASE_URL` (staging API, not production).
- Add a GitHub Actions step that posts the preview URL as a PR comment (similar to the Storybook preview bot in spec 115).

## Out of Scope

- Production deployments (spec 122).
- Staging API (spec 130).

## Success Criteria

```bash
# Preview comment appears on the next qualifying PR
python scripts/validate-spec-tasks-parity.py .specify/specs/129-vercel-preview-deploys
```
