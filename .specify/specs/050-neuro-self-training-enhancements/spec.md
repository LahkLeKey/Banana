# Feature Specification: Neuroscience-Inspired Autonomous Self-Training Enhancements

**Feature Branch**: `050-neuro-self-training-enhancements`
**Created**: 2026-05-02
**Status**: Draft
**Input**: User description: "scaffold neuroscience enhancements we can do with web research as a spike then plan out an entire sprint based on neuroscience enhancements for autonomous self training, we want the model to train itself via workflows during git events and periodically"
## Problem Statement

Feature Specification: Neuroscience-Inspired Autonomous Self-Training Enhancements aims to improve system capability and user experience by implementing the feature described in the specification.


## In Scope *(mandatory)*

- A neuroscience-inspired enhancement layer for the Banana classifier autonomous self-training cycle (banana, not-banana, ripeness models) covering: episodic replay buffers, sleep-style consolidation passes, elastic weight consolidation (EWC) regularization, predictive-coding surprise-weighted sampling, and dopamine-style reward modulation of learning rate.
- Two trigger surfaces: (a) git-event-driven training (push to `main`, merged PR, label `train:*`) via a new GitHub Actions workflow; (b) periodic consolidation cron (NREM-style nightly + REM-style weekly) layered on the existing `orchestrate-autonomous-self-training-cycle.yml`.
- Persistent replay corpora under `data/<model>/replay/` and Fisher-information snapshots under `artifacts/training/<model>/consolidation/` so successive runs can re-use prior knowledge without raw data loss.
- A `scripts/neuro/` orchestration surface (replay-buffer.py, consolidation-pass.py, surprise-sampler.py, reward-modulator.py) with deterministic CI profile (single-session) and richer local profile.
- Spec Kit-driven sprint plan (spike → impl waves → QA gate) with traceability between FR/SC, tasks, and validators.

## Out of Scope *(mandatory)*

- Replacing the existing classifier architecture or migrating to a spiking neural network runtime.
- Cross-model joint training (each of banana/not-banana/ripeness keeps its own pipeline; only orchestration is shared).
- GPU/accelerator provisioning changes; all enhancements must run on existing CI runners and the local CPU profile.
- Real-time / streaming inference changes; this feature only affects training and consolidation.
- New mobile, Electron, or React surfaces; the API/contract surface for trained artifacts is unchanged in v1.
- Removing or weakening the existing `validate-ai-contracts.py` markers or `validate-workflow-dependencies.sh` pre-flight (must remain green).

## User Scenarios & Testing *(mandatory)*

### User Story 1 - Git-event-triggered incremental self-training (Priority: P1)

When a contributor merges a PR to `main` that touches `data/<model>/corpus.json`, `scripts/train-*-model.py`, or carries the `train:<model>` label, an automated workflow runs an incremental self-training pass for the affected model(s) that (a) draws a replay batch from the persistent episodic buffer, (b) applies EWC regularization against the latest Fisher snapshot, and (c) opens a follow-up PR with refreshed artifacts plus a short evidence report.

**Why this priority**: Delivers the core "model trains itself via workflows during git events" outcome and is the smallest user-visible slice that proves the loop end-to-end.

**Independent Test**: Trigger `workflow_dispatch` on the new `orchestrate-neuro-git-event-training.yml` with `model=not-banana` against a feature branch; verify a PR is opened with refreshed `artifacts/training/not-banana/local/` outputs, an updated Fisher snapshot, and an evidence JSON listing replay items used.

**Acceptance Scenarios**:

1. **Given** a merged PR to `main` modifying `data/banana/corpus.json`, **When** the git-event workflow runs, **Then** an automated PR is opened within one workflow run that updates `artifacts/training/banana/` and includes an `evidence.json` referencing replay buffer items and Fisher version.
2. **Given** a PR labeled `train:ripeness`, **When** the git-event workflow runs, **Then** only the ripeness model is retrained and other models' artifacts are untouched.
3. **Given** a doc-only PR with no model-relevant paths or labels, **When** the workflow evaluates triggers, **Then** it short-circuits with `status=skipped` and `reason=non-impact`.

---

### User Story 2 - Periodic sleep-style consolidation (Priority: P2)

A nightly cron performs an NREM-style consolidation pass (replay-only, no new corpus ingestion) that interleaves recent episodic items with older pseudo-rehearsal samples to refresh long-term weights and refit the EWC Fisher matrix. A weekly cron performs a REM-style "creative" pass that samples high-surprise items (predictive-coding error rank) for re-evaluation and human-feedback queueing.

**Why this priority**: Mitigates catastrophic forgetting introduced by frequent git-event passes and produces the long-horizon stability story.

**Independent Test**: Manually dispatch `orchestrate-neuro-consolidation.yml` with `mode=nrem`; confirm a new Fisher snapshot is produced under `artifacts/training/<model>/consolidation/<utc-date>/` and that no `data/<model>/corpus.json` mutations occur.

**Acceptance Scenarios**:

1. **Given** an existing replay buffer of ≥ N items, **When** the nightly NREM workflow runs, **Then** a refreshed Fisher snapshot and a `consolidation-report.json` are written and the workflow exits zero.
2. **Given** the weekly REM workflow runs, **When** ≥ 1 high-surprise item is found, **Then** a triage issue is opened (or appended) listing the top-K surprise items for human review.

---

### User Story 3 - Surprise-weighted sampling and reward-modulated learning rate (Priority: P3)

Training scripts accept a `--neuro-profile` flag selecting the new enhancement layer; when active, the sampler weights inputs by predictive-coding surprise (`1 − model confidence on prior pass`) and the optimizer scales its base learning rate by a dopamine-style modulator derived from the rolling validation-accuracy delta.

**Why this priority**: Sharpens learning on hard examples and stabilizes the loop, but requires P1+P2 infrastructure to be measurable.

**Independent Test**: Run `scripts/train-not-banana-model.py --neuro-profile ci --session-mode single --max-sessions 1`; assert the run emits a `neuro-trace.json` with non-empty `surprise_weights` and `lr_modulation` arrays.

**Acceptance Scenarios**:

1. **Given** a corpus where some items were misclassified on the previous run, **When** training runs with `--neuro-profile`, **Then** those items receive a sampling weight strictly greater than the corpus-mean weight in the emitted trace.
2. **Given** validation accuracy improves run-over-run, **When** the reward modulator updates, **Then** the effective learning rate logged in the trace is ≤ the base rate (consolidate); when accuracy regresses, the effective rate is ≥ base (explore), bounded by `[0.25×, 4×]`.

---

### Edge Cases

- Empty or missing replay buffer on first run → fall back to corpus-only training and seed the buffer; do not fail the workflow.
- Missing Fisher snapshot → EWC term degenerates to zero (pure new-task loss); record `ewc=cold-start` in evidence.
- Concurrent git-event and consolidation runs on the same model → workflow concurrency group `neuro-train-${{ model }}` cancels older in-progress runs.
- Replay buffer growing without bound → enforce reservoir-sampling cap of `BANANA_NEURO_REPLAY_CAP` items per model (default 2048).
- `validate-ai-contracts.py` markers must continue to be present in any modified workflow YAML.

## Requirements *(mandatory)*

### Functional Requirements

- **FR-001**: System MUST provide a `scripts/neuro/replay-buffer.py` that maintains per-model reservoir-sampled JSONL buffers under `data/<model>/replay/buffer.jsonl` with append, sample, and prune operations.
- **FR-002**: System MUST provide a `scripts/neuro/consolidation-pass.py` that, given a model and a replay buffer, runs an EWC-regularized consolidation training pass and writes a Fisher snapshot to `artifacts/training/<model>/consolidation/<utc-date>/fisher.json`.
- **FR-003**: System MUST provide a `scripts/neuro/surprise-sampler.py` that ranks corpus items by `1 − prior_confidence` using the most recent model artifact and emits weighted sampling indices.
- **FR-004**: System MUST provide a `scripts/neuro/reward-modulator.py` that computes a learning-rate scalar in `[0.25, 4.0]` from the rolling delta of a validation metric.
- **FR-005**: Existing `scripts/train-{banana,not-banana,ripeness}-model.py` MUST accept `--neuro-profile {off,ci,local}` and, when not `off`, integrate the four neuro components and emit `neuro-trace.json` alongside artifacts.
- **FR-006**: A new `.github/workflows/orchestrate-neuro-git-event-training.yml` MUST trigger on `pull_request` (closed+merged), `push` to `main`, and `workflow_dispatch`, scope models by changed paths and `train:*` labels, and open a follow-up PR with refreshed artifacts.
- **FR-007**: A new `.github/workflows/orchestrate-neuro-consolidation.yml` MUST run on cron (`mode=nrem` nightly UTC, `mode=rem` weekly Sunday UTC) and on `workflow_dispatch`, calling consolidation-pass for each model.
- **FR-008**: Both new workflows MUST invoke `scripts/validate-workflow-dependencies.sh` as a pre-flight step and fail fast if dependencies are missing.
- **FR-009**: Both new workflows MUST keep AI-contract markers (`training-profile ci --session-mode single --max-sessions 1`, `agent:banana-classifier-agent`) so `scripts/validate-ai-contracts.py` continues to pass.
- **FR-010**: The git-event workflow MUST short-circuit (`status=skipped`, `reason=non-impact`) for changes that touch no model-relevant paths and carry no `train:*` label, matching the existing `BANANA_REQUIRE_REAL_UPDATES` contract.
- **FR-011**: The replay buffer MUST be capped via reservoir sampling at `BANANA_NEURO_REPLAY_CAP` (default 2048) per model and MUST persist deterministically across runs (sorted, stable schema).
- **FR-012**: All new scripts MUST run cleanly under the existing `.venv` Python interpreter on Windows Git Bash and on Ubuntu CI runners, with `*.sh text eol=lf` enforced for any new shell entry points.
- **FR-013**: Each git-event-triggered PR MUST include an `evidence.json` enumerating: model, replay items used (IDs only), Fisher version, surprise-weighted item count, lr modulation factor, and validator output.
- **FR-014**: The feature MUST update `.specify/wiki/human-reference/` and the corresponding `.wiki/` page describing the autonomous self-training cycle when neuro enhancements ship.

### Key Entities

- **ReplayBuffer**: Per-model JSONL of `{id, payload_ref, label, added_at, prior_confidence}`; reservoir-sampled to a fixed cap.
- **FisherSnapshot**: Per-model JSON of weight-importance estimates produced after a consolidation pass; versioned by UTC date.
- **NeuroTrace**: Per-run JSON capturing `surprise_weights`, `lr_modulation`, `replay_ids_used`, and `ewc_state`.
- **EvidenceReport**: Per-PR JSON summarizing the training pass for review.

## Success Criteria *(mandatory)*

### Measurable Outcomes

- **SC-001**: A merged `main` PR touching a model's corpus produces an automated follow-up PR with refreshed artifacts within a single workflow run (green CI), measured across ≥ 5 consecutive triggers.
- **SC-002**: Across 4 weekly cycles, validation accuracy for the not-banana model on a held-out anchor set does not regress by more than 1.0 percentage point (catastrophic-forgetting guard) when neuro enhancements are active vs. baseline.
- **SC-003**: Consolidation workflows complete within the existing CI minute budget (≤ 10 minutes per model in CI profile) using only existing runner classes.
- **SC-004**: `validate-ai-contracts.py` and `validate-workflow-dependencies.sh` remain green on every PR opened by the new workflows (0 failures across 10 sample runs).
- **SC-005**: Replay buffer file size per model stays bounded (≤ `BANANA_NEURO_REPLAY_CAP` lines) across 30 consecutive runs.
- **SC-006**: `--neuro-profile ci --session-mode single --max-sessions 1` completes a single training session in ≤ 2× the runtime of the existing CI profile baseline on the same runner.

## Assumptions

- The existing `scripts/train-{banana,not-banana,ripeness}-model.py` entry points expose enough hooks (or can be extended via an additive `--neuro-profile` flag) to integrate the new components without breaking current call sites.
- CI runners (Ubuntu) and the local Windows + Git Bash environment can execute the new Python orchestration without GPU dependencies; CPU-only NumPy is sufficient and any optional ML stack is gated behind `--neuro-profile local`.
- `BANANA_PG_CONNECTION` is not required for the neuro layer itself; replay buffers persist as files in-repo (or in `.artifacts/` when too large), keeping the loop runnable without PostgreSQL.
- The `train:<model>` label scheme can be added to the repository label set; if missing, the workflow falls back to path-based detection only.
- Existing GH Actions ruleset allows the autonomous PR opener (already used by `orchestrate-autonomous-self-training-cycle`) to open and label PRs; no new permissions are required.
- Wiki sync via `scripts/workflow-sync-wiki.sh` is the canonical path for documentation propagation.
