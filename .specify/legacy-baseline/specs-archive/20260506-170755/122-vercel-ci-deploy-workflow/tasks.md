# Tasks: Vercel CI Deploy Workflow (122)

**Input**: Spec at `.specify/specs/122-vercel-ci-deploy-workflow/spec.md`
**Prerequisites**: Spike 116 complete; `VERCEL_TOKEN`, `VERCEL_ORG_ID`, `VERCEL_PROJECT_ID` available as repo secrets

- [x] T001 Create `.github/workflows/vercel-deploy.yml` triggered on `push` to `main`, skipping bot commits.
- [x] T002 Add build gate: `cd src/typescript/react && bun install && bun run build` must exit 0 before deploy step.
- [x] T003 Add Vercel CLI deploy step: `vercel --prod --token $VERCEL_TOKEN`.
- [x] T004 Add `VERCEL_ORG_ID` and `VERCEL_PROJECT_ID` as required env vars sourced from GitHub secrets.
- [x] T005 Run `check-yaml` pre-commit hook on the new workflow file and confirm pass.
- [x] T006 Run `python scripts/validate-spec-tasks-parity.py .specify/specs/122-vercel-ci-deploy-workflow` and confirm `OK`.
