# Feature Specification: Workflow Orchestrator Recovery

**Feature Branch**: `049-workflow-orchestrator-recovery`
**Created**: 2026-05-01
**Status**: Draft
**Input**: User description: "Recover four failing scheduled orchestration workflows (Autonomous Self-Training Cycle, Banana SDLC, Not-Banana Feedback Loop, plus shared dependencies) by spiking what each workflow is supposed to do, then planning an implementation stage and a QA stage."
## Problem Statement

Feature Specification: Workflow Orchestrator Recovery aims to improve system capability and user experience by implementing the feature described in the specification.


## In Scope *(mandatory)*

- Restore green status for the four scheduled GitHub Actions workflows currently failing on `main`:
  - `Orchestrate Autonomous Self-Training Cycle`
  - `Orchestrate Banana SDLC`
  - `Orchestrate Not-Banana Feedback Loop`
  - `Orchestrate Triage Idea Cloud` (verified concurrently to prevent latent regression in the same orchestrator family)
- Re-establish a documented contract for what each orchestrator script is supposed to do, where its inputs live, and which Banana domain helper agent owns each downstream step.
- Decide per missing dependency whether to (a) restore the asset deleted by commit `45ef8bb` ("004 trim vibe drift"), (b) re-implement it against the v2 codebase, or (c) trim the orchestrator to skip the deleted feature, and capture the decision in a decision log.
- Add a CI pre-flight check that fails fast (with actionable remediation) when an orchestrator references a script, data file, or native binary that no longer exists, so this regression class cannot recur silently on a schedule.

## Out of Scope *(mandatory)*

- Adding new SDLC increments, training profiles, or feedback-loop semantics beyond what the four workflows exercised before commit `45ef8bb`.
- Re-architecting the wiki sync, Spec Kit driver, or PR-orchestration helpers; touch them only if required to unblock the four workflows.
- Changing the cron cadence or the `BANANA_SDLC_*` environment contract.
- Migrating any workflow from `actions/checkout@v4` to a Node.js 24 runtime (tracked separately).

## User Scenarios & Testing *(mandatory)*

### User Story 1 - Failing scheduled workflows return to green (Priority: P1)

A maintainer watching the Actions tab on `main` needs the four scheduled orchestrators to either succeed or skip cleanly with a documented reason, instead of exiting non-zero within 8-17 seconds.

**Why this priority**: All four runs are currently red; this blocks the autonomous SDLC contract, hides real regressions in CI noise, and prevents the not-banana feedback loop from advancing.

**Independent Test**: Trigger each workflow via `workflow_dispatch` from `main` after the fix; each run completes with conclusion `success` (or an explicit guarded `skipped` with a logged reason), and the next scheduled run on cron also succeeds.

**Acceptance Scenarios**:

1. **Given** an empty `BANANA_SDLC_INCREMENT_PLAN_JSON` and the catalog at `docs/automation/agent-pulse/autonomous-self-training-default-increments.json`, **When** `Orchestrate Autonomous Self-Training Cycle` runs, **Then** it produces a deterministic plan, executes the resolved increments, and exits 0.
2. **Given** an approved feedback inbox at `data/not-banana/feedback/inbox.json` (or no inbox), **When** `Orchestrate Not-Banana Feedback Loop` runs, **Then** it either opens a triaged-item PR or logs `No feedback entries matched` and exits 0.
3. **Given** the default Banana SDLC plan, **When** `Orchestrate Banana SDLC` runs on schedule, **Then** every increment either applies real updates and opens a PR or is skipped with `non-impact/doc-only changes` and the job exits 0.

---

### User Story 2 - Spike captures the as-built workflow contract (Priority: P1)

The next contributor needs to understand what each orchestrator was supposed to do, what its dependencies are, and which were deleted, without re-reading the entire `45ef8bb` diff.

**Why this priority**: Without this written-down contract, every future "trim" PR risks deleting the same dependencies again, and the agent decomposition skill cannot route fixes to the right helper agent.

**Independent Test**: A reviewer can read `.specify/specs/049-workflow-orchestrator-recovery/spike-report.md` and answer: which scripts each workflow calls, which were deleted in `45ef8bb`, and which restoration option was chosen, without opening any other file.

**Acceptance Scenarios**:

1. **Given** the spike report exists, **When** a reviewer opens it, **Then** they find a per-workflow table listing entrypoint script, downstream calls, deleted assets, owning helper agent, and chosen restoration option (restore / re-implement / trim).
2. **Given** the decision log entry exists, **When** the workflow contract changes again, **Then** the next contributor links the new change to the prior decision instead of re-litigating it.

---

### User Story 3 - Pre-flight prevents silent regression (Priority: P2)

Future maintainers who rename or delete a script must be warned before the change merges, instead of discovering it via a red scheduled run hours later.

**Why this priority**: Reduces mean-time-to-detect for this regression class from "next cron tick" to "PR check", and protects the four restored workflows from the same failure mode.

**Independent Test**: Introduce a temporary commit that deletes one referenced script; the new pre-flight check fails the PR with a message naming the missing script and the workflow that references it.

**Acceptance Scenarios**:

1. **Given** a PR removes `scripts/<name>.sh` that is referenced by an orchestrator, **When** the pre-flight check runs, **Then** it exits non-zero and prints `<workflow>.yml -> <script>: missing`.
2. **Given** all referenced scripts exist, **When** the pre-flight check runs, **Then** it exits 0 and prints a one-line summary of validated references.

## Functional Requirements *(mandatory)*

- **FR-001**: Each of the four scheduled workflows MUST exit with conclusion `success` on the next scheduled cron tick after the feature merges, on `main`, with no manual intervention.
- **FR-002**: When an orchestrator cannot do useful work (empty inbox, no real updates), it MUST exit 0 with a single-line skip reason that includes the workflow name and the gating condition.
- **FR-003**: The autonomous self-training cycle MUST resolve a plan from the catalog at `docs/automation/agent-pulse/autonomous-self-training-default-increments.json` (or its v2 successor) without requiring any file deleted in commit `45ef8bb` to exist on disk under its original path.
- **FR-004**: The not-banana feedback loop MUST be able to read `data/not-banana/feedback/inbox.json` (creating the directory and an empty inbox if absent) and process zero or more approved entries without crashing.
- **FR-005**: The Banana SDLC orchestrator MUST not abort the entire run when a single increment's command fails when `BANANA_SDLC_CONTINUE_ON_ERROR=true`; it MUST record per-increment status in the summary JSON.
- **FR-006**: The repository MUST include a workflow-dependency pre-flight script that scans `.github/workflows/orchestrate-*.yml` and the orchestration shell scripts they invoke, and fails when any referenced script, Python module, or required data file is missing from the working tree.
- **FR-007**: The pre-flight script MUST be wired into the existing CI lane that runs on every PR (the same lane that runs `validate-ai-contracts.py`), and MUST exit fast (under 10 seconds on a clean checkout).
- **FR-008**: The spike report at `.specify/specs/049-workflow-orchestrator-recovery/spike-report.md` MUST list, per workflow: entrypoint script, all downstream script/data references, which references were deleted in `45ef8bb`, the owning Banana helper agent (per `banana-agent-decomposition`), and the chosen restoration option.
- **FR-009**: A decision-log entry under `.specify/specs/049-workflow-orchestrator-recovery/decisions/` MUST record, for each deleted dependency, whether it was restored verbatim, re-implemented, or removed by trimming the orchestrator, with the rationale.
- **FR-010**: When the chosen option is "trim", the affected workflow MUST log a clear `feature-removed` skip message naming the removed capability so reviewers searching CI for "feedback loop" or "self-training" still find a result.

## Success Criteria *(mandatory)*

- **SC-001**: After merge, the next two consecutive scheduled runs of each of the four workflows succeed (8 successful runs total) without manual re-runs.
- **SC-002**: Total wall-clock duration for `Orchestrate Banana SDLC` on a no-op tick stays under 5 minutes (current baseline before the trim regression).
- **SC-003**: Running the new pre-flight script against `HEAD` of `main` exits 0; running it against the parent commit of this feature (`HEAD~`) exits non-zero and names at least the five missing assets identified in the spike.
- **SC-004**: A reviewer unfamiliar with the orchestrators can answer "what is workflow X supposed to do and which agent owns it?" by reading only `spike-report.md`.

## Assumptions *(mandatory)*

- See **ASSUMPTION-001** and **ASSUMPTION-002** in the section below.

## Risks & Assumptions *(mandatory)*

- **RISK-001**: Restoring deleted native sources may conflict with the v2 native domain layout under `src/native/core/domain/ml/`; mitigated by deferring restoration of `banana_agent_pulse_model*` until the spike confirms the v2 successor (or chooses "trim").
- **RISK-002**: The deleted `apply-not-banana-feedback.py` may have evolved schema expectations against `data/not-banana/corpus.json`; mitigated by re-running the existing `check-corpus-schema.py` against the restored or re-implemented script before enabling consume mode.
- **ASSUMPTION-001**: Commit `45ef8bb` ("004 trim vibe drift") is the canonical deletion point and its parent (`45ef8bb~1`) contains the last-known-good copy of every deleted asset.
- **ASSUMPTION-002**: The four scheduled workflows are the only orchestration entrypoints currently red on `main`; if more surface during the spike they are added to scope explicitly.
