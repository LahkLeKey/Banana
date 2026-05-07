# Spike Report — Workflow Orchestrator Recovery

**Feature**: 049-workflow-orchestrator-recovery
**Date**: 2026-05-01
**Author**: workflow-agent (automated spike)

## Method

For each `.github/workflows/orchestrate-*.yml`, follow the entrypoint script and grep every `bash <path>`, `python <path>`, and `cp <path>` invocation. Classify each reference as `present` or `MISSING`. For each MISSING reference, confirm the deletion commit via `git log --diff-filter=D` and capture the last-known-good blob via `git show <commit>~1:<path>`.

## Failing run signatures

| Run ID        | Workflow                                    | Wall-clock | First error                                                                                  |
|---------------|---------------------------------------------|-----------:|----------------------------------------------------------------------------------------------|
| 25225673590   | Orchestrate Autonomous Self-Training Cycle  | 17s        | `bash: scripts/generate-deterministic-agent-pulse-plan.sh: No such file or directory`        |
| 25208504622   | Orchestrate Not-Banana Feedback Loop        | 10s        | `python: can't open file 'scripts/apply-not-banana-feedback.py': [Errno 2] No such file...`  |
| 25203366883   | Orchestrate Banana SDLC                     | 12s        | Same `apply-not-banana-feedback.py` missing (first increment of inline default plan)         |

## Per-workflow inventory

### Orchestrate Autonomous Self-Training Cycle

**Workflow**: `.github/workflows/orchestrate-autonomous-self-training-cycle.yml`
**Entrypoint**: `bash scripts/workflow-orchestrate-sdlc.sh`
**Owning agent**: `workflow-agent`

| Reference                                                                                  | Status   | Decision         |
|--------------------------------------------------------------------------------------------|----------|------------------|
| `scripts/workflow-ensure-speckit.sh`                                                       | present  | n/a              |
| `python scripts/validate-ai-contracts.py`                                                  | present  | n/a              |
| `bash scripts/validate-api-parity-governance.sh` (gated by `BANANA_SDLC_RUN_API_PARITY_CHECK`) | present | n/a |
| `bash scripts/generate-deterministic-agent-pulse-plan.sh`                                  | **MISSING** | **restore-verbatim** |
| `python scripts/build-deterministic-agent-pulse-plan.py` (called by the script above)      | **MISSING** | **restore-verbatim** |
| `src/native/core/domain/banana_agent_pulse_model.{c,h}` + `..._cli.c` (compiled by the script above) | **MISSING** | **restore-verbatim** (no v2 successor; nothing else in `src/native/core/domain/` references the symbol) |
| `docs/automation/agent-pulse/autonomous-self-training-default-increments.json`             | **MISSING** | **restore-verbatim** |
| `bash scripts/workflow-orchestrate-triaged-item-pr.sh`                                     | present  | n/a              |
| `bash scripts/workflow-sync-wiki.sh`                                                       | present  | n/a              |

### Orchestrate Banana SDLC

**Workflow**: `.github/workflows/orchestrate-banana-sdlc.yml`
**Entrypoint**: `bash scripts/workflow-orchestrate-sdlc.sh` (same script, different env)
**Owning agent**: `workflow-agent`

When `BANANA_SDLC_INCREMENT_PLAN_JSON` is empty and `BANANA_SDLC_INCREMENT_PLAN_MODEL` is unset, the script falls through to the **inline default plan** (lines 79-95 of `workflow-orchestrate-sdlc.sh`). That plan's first increment depends on `scripts/apply-not-banana-feedback.py`, which is the same MISSING file that breaks the feedback loop.

| Reference                                                                              | Status   | Decision         |
|----------------------------------------------------------------------------------------|----------|------------------|
| All Self-Training Cycle refs above                                                     | mixed    | (covered above)  |
| `python scripts/apply-not-banana-feedback.py` (inline plan increment 1)                | **MISSING** | **restore-verbatim** |
| `python scripts/train-not-banana-model.py` (inline plan increment 1)                   | present  | n/a              |
| `python scripts/sync-banana-api-coverage-denominator.py` (inline plan increment 2)     | present  | n/a              |
| `cp artifacts/not-banana-model/banana_signal_tokens.h src/native/core/domain/generated/...` | dynamic | depends on training output; verified separately by training step |

### Orchestrate Not-Banana Feedback Loop

**Workflow**: `.github/workflows/orchestrate-not-banana-feedback-loop.yml`
**Entrypoint**: `bash scripts/orchestrate-not-banana-feedback-loop.sh`
**Owning agent**: `banana-classifier-agent` (data path) + `workflow-agent` (orchestration shell)

| Reference                                                | Status   | Decision         |
|----------------------------------------------------------|----------|------------------|
| `scripts/workflow-ensure-speckit.sh`                     | present  | n/a              |
| `python scripts/apply-not-banana-feedback.py` (preview + apply) | **MISSING** | **restore-verbatim** |
| `data/not-banana/feedback/inbox.json`                    | **MISSING** (directory absent) | **create-empty** (script already tolerates an empty inbox; we add an empty seed file so the cron tick has something to read) |
| `data/not-banana/corpus.json`                            | present  | n/a              |
| `bash scripts/workflow-orchestrate-triaged-item-pr.sh`   | present  | n/a              |

### Orchestrate Triage Idea Cloud

**Workflow**: `.github/workflows/orchestrate-triage-idea-cloud.yml`
**Entrypoint**: `bash scripts/workflow-triage-idea-cloud.sh`
**Owning agent**: `workflow-agent`

| Reference                                                | Status   | Decision         |
|----------------------------------------------------------|----------|------------------|
| `scripts/workflow-ensure-speckit.sh`                     | present  | n/a              |
| `bash scripts/workflow-orchestrate-triaged-item-pr.sh`   | present  | n/a              |

**Note**: This workflow is included in the spec's scope as a "verified concurrently" check; it has no MISSING refs. We will still smoke-run it locally during QA to catch any latent regression.

## Summary table

| Deleted asset (45ef8bb)                                                    | Owning agent          | Decision           |
|----------------------------------------------------------------------------|-----------------------|--------------------|
| `scripts/generate-deterministic-agent-pulse-plan.sh` (94 lines)            | workflow-agent        | restore-verbatim   |
| `scripts/build-deterministic-agent-pulse-plan.py` (179 lines)              | workflow-agent        | restore-verbatim   |
| `scripts/apply-not-banana-feedback.py` (373 lines)                         | banana-classifier-agent | restore-verbatim |
| `docs/automation/agent-pulse/autonomous-self-training-default-increments.json` (17 lines) | workflow-agent | restore-verbatim |
| `src/native/core/domain/banana_agent_pulse_model.c` (55 lines)             | native-core-agent     | restore-verbatim   |
| `src/native/core/domain/banana_agent_pulse_model.h` (17 lines)             | native-core-agent     | restore-verbatim   |
| `src/native/core/domain/banana_agent_pulse_model_cli.c` (32 lines)         | native-core-agent     | restore-verbatim   |
| `data/not-banana/feedback/inbox.json` (never existed in 45ef8bb~1)         | banana-classifier-agent | create-empty (`{"feedback": []}`) |

## Why restore-verbatim across the board

- All seven code/data assets are small, self-contained, and have **no v2 successor** (verified by `grep -rn banana_agent_pulse src/ tests/ scripts/` returning empty, and by the absence of any `apply-not-banana-feedback*` or `agent-pulse-plan*` file in the v2 tree).
- Re-implementing would mean rewriting a 373-line corpus-merge tool and a 55-line C model that are the canonical contract the orchestrator scripts expect.
- Trimming would silently drop two scheduled capabilities (deterministic SDLC plan rendering and approved-feedback ingestion) that the workflow descriptions and `.github/copilot-instructions.md` still treat as canonical.
- Restoration is a single-file `git show 45ef8bb~1:<path> > <path>` per asset; rollback is the same in reverse.

## Recurrence prevention (FR-006/FR-007)

The pre-flight script added in T040 (`scripts/validate-workflow-dependencies.sh`) walks every `.github/workflows/orchestrate-*.yml` and the shell scripts they invoke, extracts `(bash|python|cp) <path>` references, and exits non-zero listing each missing path. It will be wired into the same CI job that already runs `validate-ai-contracts.py` so future "trim" PRs cannot delete a referenced asset without a failing PR check.
