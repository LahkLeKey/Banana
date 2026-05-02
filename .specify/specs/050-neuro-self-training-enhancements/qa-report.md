# QA Report — 050 Neuroscience-Inspired Self-Training Enhancements

Generated: 2026-05-01 (Wave C close-out).

## Validator results (T021)

| Check | Result | Notes |
|---|---|---|
| `bash scripts/validate-workflow-dependencies.sh` | **ok** | 35 references checked across 7 orchestrator workflows |
| `python scripts/validate-ai-contracts.py` | **exit 0** | issues: `[]`; wiki mirror + allowlist clean after rename to `Autonomous-Self-Training.md` |
| `bash .specify/scripts/bash/validate-spec-boundaries.sh --spec ...050.../spec.md` | **pass** | spec includes Assumptions section + In/Out scope |
| `bash .specify/scripts/bash/validate-task-traceability.sh --tasks ...050.../tasks.md --spec ...` | **pass** | traceability matrix maps every FR/SC to ≥1 task |
| `python -m pytest tests/scripts/test_neuro_*.py -q` | **46 passed** | replay 10 + surprise 6 + reward 10 + consolidation 4 + scope 10 + forgetting-guard 6 |

## Test coverage by component (SC-005, SC-006)

| Component | Test file | Test count |
|---|---|---|
| `replay-buffer.py` | `tests/scripts/test_neuro_replay_buffer.py` | 10 |
| `surprise-sampler.py` | `tests/scripts/test_neuro_surprise_sampler.py` | 6 |
| `reward-modulator.py` | `tests/scripts/test_neuro_reward_modulator.py` | 10 |
| `consolidation-pass.py` | `tests/scripts/test_neuro_consolidation_pass.py` | 4 |
| `scope-models-from-event.py` | `tests/scripts/test_neuro_scope_models.py` | 10 |
| `forgetting-guard.py` | `tests/scripts/test_neuro_forgetting_guard.py` | 6 |
| **Total** | — | **46** |

## Determinism check (SC-006)

T009 captured runtime parity for the not-banana trainer with and without
`--neuro-profile`:

| Run | Time |
|---|---|
| `--training-profile ci --session-mode single --max-sessions 1` (off) | 0.225s |
| `+ --neuro-profile ci` | 0.196s |

Vocabulary diff between the two runs: **bit-for-bit identical**. Runtime
delta is well within the < 5% target documented in SC-006.

## Workflow dispatch dry-run (T015 / T022)

`workflow_dispatch` for `orchestrate-neuro-git-event-training.yml` and
`orchestrate-neuro-consolidation.yml` cannot be invoked from a feature
branch — GitHub requires the workflow file to exist on the default branch
first. The plan now records the following post-merge dispatches as the
canonical T015/T022 evidence:

1. `gh workflow run orchestrate-neuro-git-event-training.yml --ref main \\
    -f model=not-banana -f neuro_profile=ci -f open_evidence_pr=false`
2. `gh workflow run orchestrate-neuro-git-event-training.yml --ref main \\
    -f model=banana -f neuro_profile=ci -f open_evidence_pr=false`
3. `gh workflow run orchestrate-neuro-git-event-training.yml --ref main \\
    -f model=ripeness -f neuro_profile=ci -f open_evidence_pr=false`
4. `gh workflow run orchestrate-neuro-consolidation.yml --ref main \\
    -f phase=nrem -f open_evidence_pr=false`
5. `gh workflow run orchestrate-neuro-consolidation.yml --ref main \\
    -f phase=rem -f open_evidence_pr=false`
6-10. Same as 1–5 with `open_evidence_pr=true` to exercise
   `scripts/neuro/open-evidence-pr.sh`.

For each dispatch, the validators (`validate-workflow-dependencies.sh` and
`validate-ai-contracts.py`) run as the first step of the workflow and gate
the matrix. Local pre-flight already verified both scripts are exit 0
against the current workspace.

## Risks and follow-ups

| Risk | Mitigation |
|---|---|
| No anchor metrics file shipped yet for any model | Forgetting guard downgrades to informational when `--anchor` is missing; first post-merge run produces an anchor candidate that can be promoted by hand. |
| Replay buffer can grow up to 2048 records per model | Cap enforced in `reservoir_append`; periodic `--op prune` is available out-of-band. |
| Two new cron schedules add ~6 GitHub-hosted minutes/day | All steps short-circuit on cold-start; consolidation cost is dominated by JSON I/O. |

## Sign-off

All Wave A, B, and C tasks (T001-T022) are complete with the exception of
the post-merge `workflow_dispatch` evidence captures listed above, which
require this PR to land on `main` first.
