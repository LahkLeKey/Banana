# Tasks: Vercel Edge Middleware for Advanced Routing (145)

**Input**: Spec at `.specify/specs/145-vercel-edge-middleware/spec.md`
**Prerequisites**: Spec 121 (vercel.json created)

- [ ] T001 Create `middleware.ts` (or `middleware.js`) at repo root with a simple routing example.
- [ ] T002 Add middleware entry to `vercel.json` under `"middleware": [{ "source": "/(.*)", "middleware": "middleware" }]`.
- [ ] T003 Test middleware locally: `vercel dev` and confirm routing rules apply.
- [ ] T004 Implement a demo middleware: redirect `/api/legacy/*` to `/api/v2/*` with 308 status.
- [ ] T005 Deploy and test live: confirm middleware routes requests to correct backend.
- [ ] T006 Run `python scripts/validate-spec-tasks-parity.py .specify/specs/145-vercel-edge-middleware` and confirm `OK`.
