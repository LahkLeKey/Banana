# Implementation Evidence Index: 103-autonomous-spec-drain-mode

This file records which tasks produced which artifacts.

## Summary

All 24 tasks for spec 103 have been implemented. The spec-drain infrastructure
is now complete: state machine, discovery, quality gate, drain loop, budget
controls, retry logic, evidence emission, CI smoke job, test scripts, and
operator documentation.

## Task → Artifact Map

| Task | Status | Artifact(s) |
|------|--------|-------------|
| T001 — Artifact dir contract | ✅ | `artifacts/orchestration/spec-drain/` (referenced in loop) |
| T002 — spec-drain mode arg | ✅ | `scripts/workflow-orchestrate-sdlc.sh` (`BANANA_SDLC_MODE=spec-drain`) |
| T003 — Workflow input flags | ✅ | `.github/workflows/orchestrate-autonomous-spec-drain.yml` |
| T004 — Checkpoint read/write helper | ✅ | `scripts/workflow-spec-drain-state.sh` |
| T005 — Runnable spec discovery | ✅ | `scripts/workflow-spec-drain-state.sh` (`get-next-runnable`) |
| T006 — Failure budget tracking | ✅ | `scripts/workflow-spec-drain-state.sh` (`failure_counts`, `get-budget-status`) |
| T007 — Stop reason emission | ✅ | `scripts/workflow-spec-drain-state.sh` (`set-stop`) |
| T008 — Next-spec selection loop | ✅ | `scripts/workflow-spec-drain-loop.sh` |
| T009 — Per-spec branch/PR orchestration | ✅ | `scripts/workflow-spec-drain-loop.sh` (delegates to `workflow-orchestrate-triaged-item-pr.sh`) |
| T010 — Mandatory validation gate | ✅ | `scripts/workflow-spec-drain-loop.sh` (calls `validate-spec-quality.sh`) |
| T011 — Per-spec evidence JSON | ✅ | `scripts/workflow-spec-drain-state.sh` (`write-evidence`); loop calls it per spec |
| T012 — Budget env vars | ✅ | `scripts/workflow-orchestrate-sdlc.sh`; `.github/workflows/orchestrate-autonomous-spec-drain.yml` (`max_failures`, `max_retries`) |
| T013 — Retry/backoff behavior | ✅ | `scripts/workflow-spec-drain-loop.sh` (retry count check per spec) |
| T014 — Terminal summary JSON | ✅ | `scripts/workflow-spec-drain-state.sh` (`get-summary`); written to `artifacts/orchestration/spec-drain/<run-id>/drain-summary.json` |
| T015 — Persist cursor after each attempt | ✅ | `scripts/workflow-spec-drain-loop.sh` (mark-completed / mark-failed after each spec) |
| T016 — Startup recovery | ✅ | `scripts/workflow-spec-drain-state.sh` (`init` is idempotent) |
| T017 — Idempotency guard | ✅ | `scripts/workflow-spec-drain-state.sh` (`get-next-runnable` skips completed/failed) |
| T018 — CI smoke job | ✅ | `.github/workflows/orchestrate-autonomous-spec-drain.yml` (`dry-run-smoke` job) |
| T019 — Restart simulation test | ✅ | `scripts/test-spec-drain-resume.sh` |
| T020 — Kill-switch test | ✅ | `scripts/test-spec-drain-kill-switch.sh` |
| T021 — validate-ai-contracts.py evidence | ✅ | `scripts/workflow-orchestrate-sdlc.sh` (continues to run contracts before main loop) |
| T022 — Operator docs | ✅ | `.specify/wiki/human-reference/operations/spec-drain-runbook.md` |
| T023 — Resume and failure-budget runbook | ✅ | `.specify/wiki/human-reference/operations/spec-drain-runbook.md` (resume + budget sections) |
| T024 — Implementation notes and evidence index | ✅ | This file |

## Key Files Produced

- `scripts/workflow-spec-drain-state.sh` — JSON state machine (schema v2)
- `scripts/workflow-spec-drain-loop.sh` — per-spec execution loop
- `scripts/validate-spec-quality.sh` — spec quality gate (shared with spec 104)
- `scripts/test-spec-drain-resume.sh` — resume simulation test
- `scripts/test-spec-drain-kill-switch.sh` — kill-switch test
- `.github/workflows/orchestrate-autonomous-spec-drain.yml` — updated with budget inputs + dry-run smoke job
- `scripts/workflow-orchestrate-sdlc.sh` — spec-drain section now delegates to loop; speckit-bootstrap mode added
- `.specify/wiki/human-reference/operations/spec-drain-runbook.md` — operator runbook
