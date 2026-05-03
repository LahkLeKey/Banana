# Tasks: Vercel Preview Deploys Per Branch (129)

**Input**: Spec at `.specify/specs/129-vercel-preview-deploys/spec.md`
**Prerequisites**: Spike 118 complete; Vercel project linked; staging `VITE_BANANA_API_BASE_URL` set for Preview tier

- [ ] T001 Confirm Vercel automatic preview deployments are enabled for the project (enabled by default; verify in Vercel dashboard → Settings → Git).
- [ ] T002 Open a test PR touching `src/typescript/react/` and confirm Vercel posts a preview URL as a PR check status.
- [ ] T003 Inspect the deployed preview's `index.html` source to confirm `VITE_BANANA_API_BASE_URL` resolves to the staging API URL (not production).
- [ ] T004 Create or update `.github/workflows/vercel-deploy.yml` to include a PR comment step that echoes the Vercel preview URL using `actions/github-script`.
- [ ] T005 Run `python scripts/validate-spec-tasks-parity.py .specify/specs/129-vercel-preview-deploys` and confirm `OK`.
