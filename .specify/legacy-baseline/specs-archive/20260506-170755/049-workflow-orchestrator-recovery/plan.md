# Implementation Plan: Workflow Orchestrator Recovery

**Branch**: `049-workflow-orchestrator-recovery`
**Spec**: [.specify/specs/049-workflow-orchestrator-recovery/spec.md](./spec.md)
**Status**: Draft

## Overview

Recover four scheduled orchestrators that started failing within 8-17 seconds because commit `45ef8bb` ("004 trim vibe drift") deleted scripts, native sources, and the increment catalog they depend on, but did not update the orchestrator scripts or the workflow YAMLs. Work is staged as **Spike → Implementation → QA** so each stage produces a reviewable artifact before code lands.

## Touched Domains

- **Infra / workflow-agent** (primary): `.github/workflows/orchestrate-*.yml`, `scripts/workflow-orchestrate-sdlc.sh`, `scripts/orchestrate-not-banana-feedback-loop.sh`, `scripts/workflow-triage-idea-cloud.sh`, new pre-flight script.
- **Native core / native-core-agent** (conditional, only if "restore" is chosen for the deterministic plan): `src/native/core/domain/banana_agent_pulse_model{.c,.h,_cli.c}`.
- **API classifier surface / banana-classifier-agent** (conditional, only if "restore" is chosen for feedback loop): `scripts/apply-not-banana-feedback.py`, `data/not-banana/feedback/`.
- **Spec Kit / .specify**: spec, plan, tasks, spike report, decision log.

## Stage 1 - Spike (read-only, ~half day)

**Goal**: Produce `.specify/specs/049-workflow-orchestrator-recovery/spike-report.md` and one decision-log file per deleted dependency before any code is written.

**Activities**:
1. For each of the four `.github/workflows/orchestrate-*.yml`, follow the entrypoint shell script and enumerate every `python ...`, `bash ...`, file-read, and binary invocation it makes.
2. For each reference, classify it as `present`, `deleted-in-45ef8bb`, or `present-but-renamed`.
3. For each deleted reference, recover the last-known-good copy from `git show 45ef8bb~1:<path>` for inspection only (do not commit yet) and decide:
   - **restore-verbatim** if the asset is small, isolated, and still fits the v2 codebase.
   - **re-implement** if the asset's contract is needed but its implementation conflicts with v2 layout.
   - **trim** if the capability is no longer wanted; the orchestrator step must then be removed and replaced with a `feature-removed` skip log line.
4. Map each restoration to the owning Banana helper agent (per `banana-agent-decomposition`).

**Spike deliverables** (gating Stage 2):
- `spike-report.md` with one section per workflow and a summary table.
- One file per deleted dependency under `decisions/` using the `decision-log-template.md` schema (option chosen + rationale + owning agent).

**Spike exit criteria**:
- Every reference in every orchestrator script is classified.
- Every deleted reference has a decision recorded.
- No code outside `.specify/specs/049-workflow-orchestrator-recovery/` has been modified.

## Stage 2 - Implementation (driven entirely by spike decisions)

**Goal**: Apply the decisions from Stage 1 in the smallest set of changes that turns the four workflows green, plus add the recurrence-prevention pre-flight.

**Activities** (each conditional on the spike decision):
1. **Restore-verbatim path**: `git show 45ef8bb~1:<path> > <path>` for each restored file, then run `git add` and a smoke check (`bash <restored-script> --help` or equivalent) to confirm executability.
2. **Re-implement path**: write a minimal v2-compatible replacement that satisfies the orchestrator's CLI contract (flags, exit codes, output paths) without restoring deleted internals; cite the spike entry in the file header comment.
3. **Trim path**: edit the orchestrator script to remove the deleted step and emit a single-line `echo "[orchestrator] feature-removed: <capability> (see .specify/specs/049-workflow-orchestrator-recovery/decisions/<file>.md)"` skip log; update the workflow YAML only if an env var becomes unused.
4. **Pre-flight script**: add `scripts/validate-workflow-dependencies.sh` (and a Python helper if shell parsing is unwieldy) that:
   - Parses `.github/workflows/orchestrate-*.yml` for `run:` blocks.
   - Walks each invoked shell script for `python <path>`, `bash <path>`, and `cp/mv` references.
   - Exits non-zero listing each `<workflow>.yml -> <missing-path>` pair.
5. Wire the pre-flight into the existing AI-contracts CI lane (the workflow that already calls `python scripts/validate-ai-contracts.py`).

**Implementation exit criteria**:
- `bash scripts/validate-workflow-dependencies.sh` exits 0 locally.
- Each affected workflow's entrypoint script runs locally with `BANANA_SDLC_REQUIRE_REAL_UPDATES=true` and either applies changes or logs a documented skip and exits 0.

## Stage 3 - QA & verification

**Goal**: Prove the four workflows stay green for two consecutive cron ticks and that the pre-flight catches the regression class.

**Activities**:
1. **Local dry runs** (Bash, repo root): execute each orchestrator with `BANANA_SDLC_REQUIRE_REAL_UPDATES=true BANANA_SDLC_SKIP_NON_REAL_UPDATES=true` and a temp `BANANA_SDLC_OUTPUT_DIR=$(mktemp -d)`; assert exit 0 and presence of `summary-*.json` (or documented skip).
2. **Pre-flight regression test**: temporarily delete one referenced script in a scratch worktree, run `validate-workflow-dependencies.sh`, confirm non-zero exit naming the missing path.
3. **PR-time CI**: open the PR; confirm the AI-contracts lane now includes the pre-flight step and is green.
4. **Post-merge cron observation**: after merge, watch the next two scheduled runs of each of the four workflows on `main` (8 runs total). Record run IDs in `decisions/post-merge-cron-observation.md`.

**QA exit criteria (mapped to spec success criteria)**:
- SC-001: 8 consecutive successful scheduled runs documented.
- SC-002: `Orchestrate Banana SDLC` no-op duration < 5 min.
- SC-003: Pre-flight green at HEAD, red at HEAD~ before this feature.
- SC-004: Spike report sufficient to answer "what is workflow X" without other files.

## Validation Hooks

- `bash scripts/validate-workflow-dependencies.sh` (new, must exist by end of Stage 2).
- `python scripts/validate-ai-contracts.py` (existing; must remain green).
- `bash .specify/scripts/bash/validate-spec-boundaries.sh --spec .specify/specs/049-workflow-orchestrator-recovery/spec.md`.
- `bash .specify/scripts/bash/validate-task-traceability.sh --tasks .specify/specs/049-workflow-orchestrator-recovery/tasks.md`.

## Rollback

- Each restored file is a single `git show 45ef8bb~1:<path>` blob; reverting the feature branch removes them cleanly.
- Each trimmed orchestrator change is contained in its own commit (per the task list) so individual trims can be reverted without disturbing others.
- The pre-flight script is additive; disabling it only requires removing one CI step.
