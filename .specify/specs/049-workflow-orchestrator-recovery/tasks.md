# Tasks: Workflow Orchestrator Recovery

**Spec**: [spec.md](./spec.md) · **Plan**: [plan.md](./plan.md)
**Branch**: `049-workflow-orchestrator-recovery`

Tasks are dependency-ordered. `[P]` = parallelizable with the prior `[P]` task in the same stage. Stage gates are hard: do not start Stage 2 until every Stage 1 task is complete and reviewed.

## Stage 1 — Spike (read-only)

- [ ] **T001** Enumerate references in `.github/workflows/orchestrate-autonomous-self-training-cycle.yml` → `scripts/workflow-orchestrate-sdlc.sh`. Record entrypoint script, every `python`/`bash`/file reference, and which exist on disk now. (FR-008) (agent: workflow-agent)
- [ ] **T002 [P]** Enumerate references for `.github/workflows/orchestrate-banana-sdlc.yml`. (FR-008) (agent: workflow-agent)
- [ ] **T003 [P]** Enumerate references for `.github/workflows/orchestrate-not-banana-feedback-loop.yml` → `scripts/orchestrate-not-banana-feedback-loop.sh`. (FR-008) (agent: workflow-agent)
- [ ] **T004 [P]** Enumerate references for `.github/workflows/orchestrate-triage-idea-cloud.yml` → `scripts/workflow-triage-idea-cloud.sh`. (FR-008) (agent: workflow-agent)
- [ ] **T005** Cross-check each missing reference against `git log --all --diff-filter=D --name-only -- <path>` to confirm `45ef8bb` is the deletion commit; capture last-known-good blobs via `git show 45ef8bb~1:<path>` (do not commit). (ASSUMPTION-001) (agent: workflow-agent)
- [ ] **T006** Write `.specify/specs/049-workflow-orchestrator-recovery/spike-report.md` with one section per workflow plus a summary table: entrypoint · downstream refs · deleted-in-45ef8bb · owning helper agent · proposed restoration option (restore / re-implement / trim). (FR-008, SC-004) (agent: technical-writer-agent)
- [ ] **T007** For each deleted dependency, create `decisions/<dep-name>.md` from `.specify/templates/decision-log-template.md` recording the chosen option, rationale, and owning agent. (FR-009) (agent: workflow-agent)
- [ ] **T008** Spike gate: human review of `spike-report.md` and `decisions/`. No Stage 2 task starts until this is approved. (gate)

## Stage 2 — Implementation

> Only the tasks matching the spike's chosen option for each dependency are executed. Tasks below cover all three branches; the spike output dictates which ones run.

### 2A — Restore-verbatim (only if chosen in spike)

- [ ] **T010 [P]** Restore `scripts/generate-deterministic-agent-pulse-plan.sh` from `45ef8bb~1`; `chmod +x`; smoke-run with `--help` or with the catalog path. (FR-003) (agent: workflow-agent)
- [ ] **T011 [P]** Restore `scripts/build-deterministic-agent-pulse-plan.py` from `45ef8bb~1`; smoke-run `python ... --help`. (FR-003) (agent: workflow-agent)
- [ ] **T012 [P]** Restore `src/native/core/domain/banana_agent_pulse_model.{c,h}` and `banana_agent_pulse_model_cli.c` from `45ef8bb~1`; verify they compile via the command embedded in `generate-deterministic-agent-pulse-plan.sh`. (FR-003, RISK-001) (agent: native-core-agent)
- [ ] **T013 [P]** Restore `docs/automation/agent-pulse/autonomous-self-training-default-increments.json` from `45ef8bb~1`; validate it parses as JSON and that every `id` is unique. (FR-003) (agent: workflow-agent)
- [ ] **T014 [P]** Restore `scripts/apply-not-banana-feedback.py` from `45ef8bb~1`; run `python scripts/apply-not-banana-feedback.py --help`; run `python scripts/check-corpus-schema.py` against `data/not-banana/corpus.json`. (FR-004, RISK-002) (agent: banana-classifier-agent)

### 2B — Re-implement (only if chosen in spike, per dependency)

- [ ] **T020** For any deleted asset whose decision is `re-implement`, write a v2-compatible replacement satisfying the orchestrator's CLI contract; cite the originating decision-log file in the new file's header comment. (FR-003, FR-004) (agent: per decision)

### 2C — Trim (only if chosen in spike, per dependency)

- [ ] **T030** For any orchestrator whose decision is `trim`, edit the orchestrator script to remove the deleted step and emit `echo "[<orchestrator>] feature-removed: <capability> (see .specify/specs/049-workflow-orchestrator-recovery/decisions/<file>.md)"`. Remove now-unused env vars from the matching workflow YAML. (FR-002, FR-010) (agent: workflow-agent)

### 2D — Recurrence prevention (always)

- [ ] **T040** Add `scripts/validate-workflow-dependencies.sh` that parses `.github/workflows/orchestrate-*.yml`, walks invoked shell scripts for `python <path>` / `bash <path>` / `cp <path>` references, and exits non-zero listing each missing path as `<workflow>.yml -> <path>: missing`. Must finish in under 10s on a clean checkout. (FR-006, FR-007, SC-003) (agent: workflow-agent)
- [ ] **T041** Wire `scripts/validate-workflow-dependencies.sh` into the existing CI lane that already runs `python scripts/validate-ai-contracts.py` (same workflow file, additional step before or after). (FR-007) (agent: workflow-agent)

## Stage 3 — QA & Verification

- [ ] **T050** Local dry run: `Orchestrate Autonomous Self-Training Cycle` via `bash scripts/workflow-orchestrate-sdlc.sh` with `BANANA_SDLC_OUTPUT_DIR=$(mktemp -d)`; assert exit 0 and a `summary-*.json` (or documented skip). (FR-001, FR-002) (agent: integration-agent)
- [ ] **T051 [P]** Local dry run for `Orchestrate Banana SDLC` with the same harness. (FR-001, FR-005) (agent: integration-agent)
- [ ] **T052 [P]** Local dry run for `Orchestrate Not-Banana Feedback Loop` with an empty `data/not-banana/feedback/inbox.json`; assert exit 0 with `No feedback entries matched` skip line. (FR-002, FR-004) (agent: integration-agent)
- [ ] **T053 [P]** Local dry run for `Orchestrate Triage Idea Cloud`; assert exit 0. (FR-001) (agent: integration-agent)
- [ ] **T054** Pre-flight regression test: in a scratch worktree, delete one referenced script and confirm `validate-workflow-dependencies.sh` exits non-zero naming that path. Restore the file. (SC-003) (agent: integration-agent)
- [ ] **T055** Open PR; confirm the AI-contracts CI lane now runs the pre-flight and is green. Confirm `gh pr checks` reports all required checks passing. (FR-007) (agent: workflow-agent)
- [ ] **T056** After merge, observe the next two scheduled runs of each of the four workflows on `main` (8 runs). Record run IDs and conclusions in `decisions/post-merge-cron-observation.md`. (SC-001) (agent: integration-agent)

## Traceability

| Task         | FR / SC               |
|--------------|-----------------------|
| T001-T005    | FR-008, ASSUMPTION-001|
| T006         | FR-008, SC-004        |
| T007         | FR-009                |
| T008         | gate                  |
| T010-T014    | FR-003, FR-004, RISK-001, RISK-002 |
| T020         | FR-003, FR-004        |
| T030         | FR-002, FR-010        |
| T040-T041    | FR-006, FR-007, SC-003|
| T050-T053    | FR-001, FR-002, FR-004, FR-005 |
| T054         | SC-003                |
| T055         | FR-007                |
| T056         | SC-001, SC-002        |
