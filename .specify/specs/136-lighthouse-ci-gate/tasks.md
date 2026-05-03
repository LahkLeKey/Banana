# Tasks: Lighthouse CI Gate (136)

**Input**: Spec at `.specify/specs/136-lighthouse-ci-gate/spec.md`
**Prerequisites**: Spike 119 complete; Vercel preview URL available; spec 122 (CI workflow exists)

- [x] T001 Install `@lhci/cli` as a devDependency: `bun add -d @lhci/cli` in `src/typescript/react`.
- [x] T002 Create `lighthouserc.json` at the repo root with assert config: `lcp < 2500`, `cls < 0.1`, `preset: "lighthouse:recommended"`.
- [x] T003 Add a `lighthouse-ci` job to `.github/workflows/vercel-deploy.yml` that: waits for the Vercel preview URL, runs `lhci autorun --collect.url=$VERCEL_PREVIEW_URL`, and uploads the HTML report as an artifact.
- [ ] T004 Run `lhci autorun` locally against the preview URL and confirm the report is generated in `.lighthouseci/`.
- [x] T005 Create `docs/lighthouse-baseline.md` with the initial mobile scores from `banana.engineer`.
- [x] T006 Run `python scripts/validate-spec-tasks-parity.py .specify/specs/136-lighthouse-ci-gate` and confirm `OK`.
