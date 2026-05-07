# Tasks: 050 Neuroscience-Inspired Self-Training Enhancements

**Spec**: [spec.md](spec.md) · **Plan**: [plan.md](plan.md) · **Spike**: [spike-report.md](spike-report.md)

Tasks are dependency-ordered. `[P]` indicates tasks safe to run in parallel within the same wave. Each task lists the FR/SC it satisfies for traceability.

## Wave A — Foundations

- **T001** Create `scripts/neuro/` directory + module README documenting the four-component contract. *(supports FR-001..FR-005)*
- **T002** [P] Implement `scripts/neuro/replay-buffer.py` (append/sample/prune, reservoir cap, deterministic stable JSONL) with unit tests in `tests/unit/neuro/test_replay_buffer.py`. *(FR-001, FR-011, SC-005)*
- **T003** [P] Implement `scripts/neuro/surprise-sampler.py` reading the most recent model artifact and emitting `surprise_weights` with a `0.7 * uniform + 0.3 * surprise` floor; unit-test the floor and rank-ordering. *(FR-003)*
- **T004** [P] Implement `scripts/neuro/reward-modulator.py` returning a clamped scalar in `[0.25, 4.0]` from a rolling delta of validation accuracy; unit-test bounds and direction. *(FR-004, US3 acceptance #2)*
- **T005** Implement `scripts/neuro/consolidation-pass.py` that, given model + buffer, produces `artifacts/training/<model>/consolidation/<utc-date>/fisher.json` (cold-start safe) and a `consolidation-report.json`. *(FR-002, US2 acceptance #1)*
- **T006** Extend `scripts/train-not-banana-model.py` with `--neuro-profile {off,ci,local}` (additive, default `off`) wiring T002–T005 and emitting `neuro-trace.json`. *(FR-005, FR-013 partial)*
- **T007** [P] Apply the same `--neuro-profile` wiring to `scripts/train-banana-model.py`. *(FR-005)*
- **T008** [P] Apply the same `--neuro-profile` wiring to `scripts/train-ripeness-model.py`. *(FR-005)*
- **T009** Confirm `Train: not-banana (ci profile)` task passes both with and without `--neuro-profile ci`; record runtime ratio for SC-006. *(SC-006)*

## Wave B — Workflows

- **T010** Add `.github/workflows/orchestrate-neuro-git-event-training.yml` (triggers: `pull_request` closed+merged on main, `push` on main, `workflow_dispatch`; matrix over `[banana, not-banana, ripeness]`; concurrency `neuro-train-${{ matrix.model }}`). Include AI-contract markers and `scripts/validate-workflow-dependencies.sh` pre-flight. *(FR-006, FR-008, FR-009)*
- **T011** Add `.github/workflows/orchestrate-neuro-consolidation.yml` (cron nightly NREM, cron weekly REM, `workflow_dispatch`; same markers + pre-flight). *(FR-007, FR-008, FR-009)*
- **T012** Add `scripts/neuro/open-evidence-pr.sh` reusing `scripts/workflow-orchestrate-triaged-item-pr.sh` patterns to open a follow-up PR with refreshed artifacts and `evidence.json`. Enforce LF endings via `.gitattributes`. *(FR-013, FR-012)*
- **T013** Add the path/label scoping helper `scripts/neuro/scope-models-from-event.py` returning the matrix of models to retrain; short-circuit non-impact events. *(FR-010, US1 acceptance #3)*
- **T014** Run `scripts/validate-workflow-dependencies.sh` and `scripts/validate-ai-contracts.py` locally; both must be green for both new workflows. *(SC-004 partial, FR-008, FR-009)*
- **T015** Dry-run dispatch `orchestrate-neuro-git-event-training.yml` with `model=not-banana` on this feature branch; capture run URL in evidence. *(US1 Independent Test)*

## Wave C — QA, Forgetting Guard, Docs

- **T016** Implement `scripts/neuro/forgetting-guard.py` comparing latest artifact accuracy vs. anchor set; fail with non-zero exit if regression > 1.0 pp. Unit test with synthetic accuracies. *(SC-002)*
- **T017** Wire `forgetting-guard.py` as the final step of both new workflows. *(SC-002, SC-004)*
- **T018** [P] Update `.specify/wiki/human-reference/autonomous-self-training-cycle.md` (or create) with the neuro layer description, opt-in/out via `--neuro-profile`, and trigger surface diagram. *(FR-014)*
- **T019** [P] Update `.wiki/Autonomous-Self-Training.md` (human page) and run `scripts/workflow-sync-wiki.sh` to confirm zero drift. *(FR-014)*
- **T020** Add `docs/release-notes/050-neuro-self-training.md` summarizing the new flag, workflows, and rollback path (`--neuro-profile off`). *(FR-014)*
- **T021** Run `scripts/validate-spec-boundaries.sh --spec .specify/specs/050-neuro-self-training-enhancements/spec.md` and `scripts/validate-task-traceability.sh --tasks .specify/specs/050-neuro-self-training-enhancements/tasks.md`; record results in `traceability-report.md`. *(SC-004)*
- **T022** Capture 10 sample dry-run dispatches across both new workflows, asserting validators stay green; summarize in `qa-report.md`. *(SC-001, SC-004)*

## Traceability Matrix

| Requirement / Criterion | Task(s) |
|---|---|
| FR-001 | T002 |
| FR-002 | T005 |
| FR-003 | T003 |
| FR-004 | T004 |
| FR-005 | T006, T007, T008 |
| FR-006 | T010 |
| FR-007 | T011 |
| FR-008 | T010, T011, T014 |
| FR-009 | T010, T011, T014 |
| FR-010 | T013 |
| FR-011 | T002 |
| FR-012 | T012 |
| FR-013 | T006, T012 |
| FR-014 | T018, T019, T020 |
| SC-001 | T015, T022 |
| SC-002 | T016, T017 |
| SC-003 | T009, T011 |
| SC-004 | T014, T021, T022 |
| SC-005 | T002 |
| SC-006 | T009 |
