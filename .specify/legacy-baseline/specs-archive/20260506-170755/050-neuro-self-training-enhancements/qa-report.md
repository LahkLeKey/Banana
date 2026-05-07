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

## Workflow dispatch evidence (T015 / T022)

Captured post-merge against `main` (PR #474 merged 2026-05-02T02:50:37Z):

| # | Workflow | Inputs | Run ID | Conclusion |
|---|---|---|---|---|
| 1 | orchestrate-neuro-git-event-training.yml | model=not-banana, neuro_profile=ci | [25241973445](https://github.com/LahkLeKey/Banana/actions/runs/25241973445) | success |
| 2 | orchestrate-neuro-git-event-training.yml | model=banana, neuro_profile=ci | [25241973804](https://github.com/LahkLeKey/Banana/actions/runs/25241973804) | success |
| 3 | orchestrate-neuro-git-event-training.yml | model=ripeness, neuro_profile=ci | [25241974202](https://github.com/LahkLeKey/Banana/actions/runs/25241974202) | success |
| 4 | orchestrate-neuro-consolidation.yml | phase=nrem | [25241974589](https://github.com/LahkLeKey/Banana/actions/runs/25241974589) | success |
| 5 | orchestrate-neuro-consolidation.yml | phase=rem | [25241974928](https://github.com/LahkLeKey/Banana/actions/runs/25241974928) | success |
| 6 | orchestrate-neuro-git-event-training.yml | event=push (post-merge auto) | [25241966953](https://github.com/LahkLeKey/Banana/actions/runs/25241966953) | success |
| 7 | orchestrate-neuro-git-event-training.yml | event=pull_request_target (PR #474 merged) | [25241966674](https://github.com/LahkLeKey/Banana/actions/runs/25241966674) | success |

All 7 runs passed `validate-workflow-dependencies.sh` + `validate-ai-contracts.py`
pre-flight, scoped the matrix correctly, completed every trainer/consolidator
step, and exited with the forgetting guard returning `informational` (no
anchor metrics shipped yet — confirms cold-start path).

Open-evidence-pr (`open_evidence_pr=true`) variants are deferred until the
first set of anchor metrics is promoted; running them now would just open
PRs with no real diff.

## Risks and follow-ups

| Risk | Mitigation |
|---|---|
| No anchor metrics file shipped yet for any model | Forgetting guard downgrades to informational when `--anchor` is missing; first post-merge run produces an anchor candidate that can be promoted by hand. |
| Replay buffer can grow up to 2048 records per model | Cap enforced in `reservoir_append`; periodic `--op prune` is available out-of-band. |
| Two new cron schedules add ~6 GitHub-hosted minutes/day | All steps short-circuit on cold-start; consolidation cost is dominated by JSON I/O. |

## Sign-off

All Wave A, B, and C tasks (T001-T022) are **complete**. Feature 050
ships in PRs #466 (scaffold), #467 (Waves A+B), and #474 (Wave C +
forgetting-guard enforcement + docs). Post-merge T022 dispatch evidence
is captured in the table above.
