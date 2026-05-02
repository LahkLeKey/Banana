# Spec-Drain Operator Runbook

**Audience**: Operators running or monitoring autonomous spec-drain mode.
**Updated**: See Git history for last change date.

---

## Overview

Spec-drain mode runs the Banana Speckit backlog sequentially. For each spec that has a `spec.md` and `tasks.md`, it:

1. Validates the spec with `validate-spec-quality.sh`.
2. Delegates implementation to `workflow-orchestrate-triaged-item-pr.sh`.
3. Records per-spec evidence JSON under `artifacts/orchestration/spec-drain/`.
4. Updates the checkpoint state at `artifacts/sdlc-orchestration/spec-drain-state-<run-id>.json`.
5. Stops when the backlog is exhausted, a failure budget is exceeded, or a kill switch fires.

---

## Triggering a Drain Run

### Via GitHub Actions (recommended)

1. Go to **Actions → Orchestrate Autonomous Spec Drain**.
2. Click **Run workflow**.
3. Fill in the inputs (all optional; defaults are safe):
   - `sdlc_id` — human-readable label for this run.
   - `autonomy_stop` — set `true` to hard-stop immediately.
   - `max_failures` — max unique failed specs before stopping (default: 5).
   - `max_retries` — max retries per spec before permanent failure (default: 2).
   - `dry_run` — set `true` to simulate without creating branches or PRs.
   - `auto_debug_ci` — set `true` to scaffold CI-followup specs before the drain loop.

### Via local shell

```bash
export BANANA_SDLC_MODE=spec-drain
export BANANA_DRAIN_MAX_FAILURES=5
export BANANA_DRAIN_MAX_RETRIES=2
export BANANA_DRAIN_DRY_RUN=false   # or true for smoke testing
export GH_TOKEN=<your-token>
bash scripts/workflow-orchestrate-sdlc.sh
```

---

## Resuming After Interruption

The drain loop persists a checkpoint at:
```
artifacts/sdlc-orchestration/spec-drain-state-<run-id>-attempt-<n>.json
```

If a run is interrupted, re-run with the **same run-id and attempt number** to resume:

```bash
export BANANA_SDLC_DRAIN_STATE_PATH=artifacts/sdlc-orchestration/spec-drain-state-<run-id>-attempt-<n>.json
bash scripts/workflow-orchestrate-sdlc.sh
```

The `init` command is idempotent — it will **not** overwrite an existing checkpoint.

### Verifying resume state

```bash
bash scripts/workflow-spec-drain-state.sh get-summary \
  artifacts/sdlc-orchestration/spec-drain-state-<run-id>.json
```

---

## Failure Budget

The failure budget controls how many unique spec failures are tolerated before the drain stops.

| Setting | Env var | Workflow input | Default |
|---------|---------|---------------|---------|
| Max unique failures | `BANANA_DRAIN_MAX_FAILURES` | `max_failures` | 5 |
| Max retries per spec | `BANANA_DRAIN_MAX_RETRIES` | `max_retries` | 2 |

When the budget is exceeded, the state gets `stop_reason: budget_exceeded` and the loop exits with code `2`.

### Example: Liberal budget for unblocking a stale backlog

```bash
export BANANA_DRAIN_MAX_FAILURES=20
export BANANA_DRAIN_MAX_RETRIES=3
```

### Example: Strict budget for a guarded production drain

```bash
export BANANA_DRAIN_MAX_FAILURES=2
export BANANA_DRAIN_MAX_RETRIES=1
```

---

## Emergency Kill Switch

To stop the drain mid-run without waiting for the current spec to finish:

1. Set `BANANA_AUTONOMY_STOP=true` in the environment or workflow input.
2. The loop checks this at the start of **every iteration**, not just between runs.
3. The stop reason `kill_switch` is written to the checkpoint before exit.

### Via workflow re-dispatch

If a workflow run is already in progress, trigger a new dispatch with `autonomy_stop=true`. The running job reads the env on next iteration.

---

## Reading Evidence Artifacts

Per-spec evidence is written to:
```
artifacts/orchestration/spec-drain/<run-id>/<spec-name>/result.json
```

Each file contains:
```json
{
  "spec_id": "103-autonomous-spec-drain-mode",
  "status": "completed",
  "reason": "",
  "written_at": "2025-01-01T00:00:00Z",
  "evidence_path": "artifacts/orchestration/spec-drain/local-run/103.../result.json"
}
```

The terminal drain summary is at:
```
artifacts/orchestration/spec-drain/<run-id>/drain-summary.json
```

---

## Validating a Spec Before Drain

To check if a spec is ready to enter the drain queue:

```bash
bash scripts/validate-spec-quality.sh --spec .specify/specs/<spec-name>/spec.md
```

Exit 0 = ready. Exit 1 = has unfilled placeholders or missing sections.

---

## Bootstrapping a New Spec

Use `speckit-bootstrap` mode to run `specify plan` + `specify tasks` in one command:

```bash
export BANANA_SDLC_MODE=speckit-bootstrap
export BANANA_SPECKIT_BOOTSTRAP_TARGET=<spec-name>
bash scripts/workflow-orchestrate-sdlc.sh
```

This will:
1. Validate the spec quality gate.
2. Run `specify plan` to generate `plan.md`.
3. Run `specify tasks` to generate `tasks.md`.

---

## Dry-Run Smoke Testing

To validate the drain plumbing without touching real branches or PRs:

```bash
bash scripts/test-spec-drain-resume.sh
bash scripts/test-spec-drain-kill-switch.sh
```

Or run the `dry-run-smoke` CI job from the workflow dispatch.

---

## Stop Reason Reference

| Reason | Meaning | Exit Code |
|--------|---------|-----------|
| `exhausted` | All specs processed | 0 |
| `kill_switch` | `BANANA_AUTONOMY_STOP=true` | 0 |
| `budget_exceeded` | Too many unique failures | 2 |
| `policy_blocked` | Hard policy stop | 2 |
| `quality_gate` | Spec failed quality check (per-spec, not drain-level) | — |
| `max_retries_exceeded` | Individual spec exceeded retry limit | — |
