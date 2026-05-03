# Tasks: Vercel Build Config (121)

**Input**: Spec at `.specify/specs/121-vercel-build-config/spec.md`
**Prerequisites**: Spike 116 complete with go verdict

- [ ] T001 Validate and harden `vercel.json` based on spike 116 research findings.
- [ ] T002 Create `.env.example` in `src/typescript/react/` documenting required `VITE_*` environment variables with placeholder values.
- [ ] T003 Run `cd src/typescript/react && rm -rf node_modules && bun install && bun run build` to confirm clean-install build works.
- [ ] T004 Run `python3 -c "import json; json.load(open('vercel.json')); print('OK')"` to confirm valid JSON.
- [ ] T005 Run `python scripts/validate-spec-tasks-parity.py .specify/specs/121-vercel-build-config` and confirm `OK`.
