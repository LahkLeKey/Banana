# Drift Realignment Decision Log — 045ef8bb Asset Restoration

Records the per-asset decision for every dependency the four scheduled orchestrators need but that commit `45ef8bb` ("004 trim vibe drift") deleted from `main`.

## Context

- Feature: 049-workflow-orchestrator-recovery
- Intake reference: failing scheduled runs 25225673590 / 25208504622 / 25203366883 (and #146)
- Reviewer lane: workflow-agent (primary), banana-classifier-agent (feedback path), native-core-agent (deterministic model sources)
- Spike: [spike-report.md](../spike-report.md)

## Decisions

| Timestamp (UTC)        | Finding ID | Decision Type | Decision Owner            | Follow-up Destination                                                                       | Rationale |
| ---                    | ---        | ---           | ---                       | ---                                                                                         | --- |
| 2026-05-01T18:00:00Z   | DF-001     | retain (restore-verbatim from `45ef8bb~1`) | workflow-agent            | `scripts/generate-deterministic-agent-pulse-plan.sh`                                        | 94-line shell glue with no v2 successor; required by Self-Training Cycle to render the deterministic plan. Restoring is a single `git show` blob; rollback is symmetric. |
| 2026-05-01T18:00:01Z   | DF-002     | retain (restore-verbatim) | workflow-agent            | `scripts/build-deterministic-agent-pulse-plan.py`                                            | Companion to DF-001; renders catalog + native-model JSON into the SDLC plan format expected by `workflow-orchestrate-sdlc.sh`. No v2 alternative exists. |
| 2026-05-01T18:00:02Z   | DF-003     | retain (restore-verbatim) | banana-classifier-agent   | `scripts/apply-not-banana-feedback.py`                                                       | 373-line Python tool that ingests `data/not-banana/feedback/inbox.json` into `data/not-banana/corpus.json`. Both the Feedback Loop and the Banana SDLC inline plan call it. Re-implementing would replicate a stable contract for no benefit. Risk RISK-002 is mitigated by re-running `scripts/check-corpus-schema.py` against the restored script. |
| 2026-05-01T18:00:03Z   | DF-004     | retain (restore-verbatim) | workflow-agent            | `docs/automation/agent-pulse/autonomous-self-training-default-increments.json`               | 17-line catalog consumed by the deterministic plan builder. Self-Training Cycle workflow YAML hard-codes this exact path via `BANANA_SDLC_INCREMENT_CATALOG_PATH`. |
| 2026-05-01T18:00:04Z   | DF-005     | retain (restore-verbatim) | native-core-agent         | `src/native/core/domain/banana_agent_pulse_model.{c,h}` + `..._cli.c`                        | 104 lines combined; compiled on-the-fly by DF-001's shell script. RISK-001 mitigated by `grep -rn banana_agent_pulse src/ tests/ scripts/` returning empty (no v2 successor or symbol clash). |
| 2026-05-01T18:00:05Z   | DF-006     | retain (create-empty seed) | banana-classifier-agent   | `data/not-banana/feedback/inbox.json`                                                        | The orchestrator already tolerates an empty inbox (`processed=0` triggers a clean skip). Seeding `{"feedback": []}` lets the directory exist so future contributors have an obvious drop point and so cron ticks have something to read. |
| 2026-05-01T18:00:06Z   | DF-007     | retain (add pre-flight) | workflow-agent            | `scripts/validate-workflow-dependencies.sh` + CI wiring                                      | New script (FR-006/FR-007). Walks `.github/workflows/orchestrate-*.yml` and the shell scripts they invoke for `(bash\|python\|cp) <path>` references; exits non-zero on any missing path. Wired into the existing AI-contracts CI lane so this regression class fails the PR instead of the next cron tick. |

## Guidance

- All seven findings chose **restore-verbatim** because the deletion in `45ef8bb` was unintentional drift, not a deliberate capability removal — the orchestrator scripts and workflow YAMLs still reference every deleted asset by its original path.
- DF-007 is the recurrence-prevention control; without it the same regression class would re-occur the next time a "trim" PR runs.
- No `defer`, `split`, or `reject` decisions were taken; if any restoration fails QA (Stage 3) the failing item escalates to a follow-up `defer` entry plus a trim of the corresponding orchestrator step under FR-010.
