# Implementation Plan: 050 Neuroscience-Inspired Self-Training Enhancements

**Spec**: [spec.md](spec.md)
**Spike**: [spike-report.md](spike-report.md)
**Status**: Draft
**Created**: 2026-05-02

## Strategy

Three sequential waves on top of the spike, each independently mergeable behind the additive `--neuro-profile` flag (default `off` keeps current behavior). Every wave preserves the existing `validate-ai-contracts.py` markers and `validate-workflow-dependencies.sh` pre-flight introduced in 049.

```
Spike  →  Wave A: Foundations    →  Wave B: Workflows    →  Wave C: QA & Docs
(read-     (replay buffer +         (git-event +            (forgetting guard
 only)      consolidation +         consolidation            metric, wiki sync,
            surprise + reward)      workflows + PR opener)   release notes)
```

## Wave A — Foundations (FR-001..FR-005, FR-011, FR-012)

Implement the four `scripts/neuro/*.py` components and integrate them as an additive `--neuro-profile` path in the three trainers. No workflow changes yet. Output: `neuro-trace.json`, replay buffer files, Fisher snapshots written to local paths.

Validation:
- New unit tests under `tests/unit/neuro/` for reservoir cap, surprise rank ordering, and lr-modulation bounds.
- `Train: not-banana (ci profile)` task remains green when run with and without `--neuro-profile ci`.
- `scripts/validate-ai-contracts.py` stays green (no workflow markers touched yet).

## Wave B — Workflows (FR-006..FR-010, FR-013)

Add the two new workflows and the PR-opener helper that consumes Wave A outputs.

- `.github/workflows/orchestrate-neuro-git-event-training.yml` (pull_request closed+merged, push to main, workflow_dispatch).
- `.github/workflows/orchestrate-neuro-consolidation.yml` (cron nightly NREM, weekly REM, workflow_dispatch).
- `scripts/neuro/open-evidence-pr.sh` reusing the existing GH CLI orchestration pattern (`scripts/workflow-orchestrate-triaged-item-pr.sh`) to open a follow-up PR with `evidence.json`.

Validation:
- `scripts/validate-workflow-dependencies.sh` reports zero missing refs for both new workflows.
- `scripts/validate-ai-contracts.py` finds the required markers (`training-profile ci --session-mode single --max-sessions 1`, `agent:banana-classifier-agent`) in both new workflow YAMLs.
- `gh workflow run orchestrate-neuro-git-event-training.yml -f model=not-banana -f dry_run=true` completes successfully on a feature branch.

## Wave C — QA, Forgetting Guard, and Docs (SC-002, SC-004, FR-014)

- Add `scripts/neuro/forgetting-guard.py` that compares the latest model artifact against an anchor validation set and fails the consolidation workflow if regression > 1.0 pp (SC-002).
- Wire the guard into both new workflows as the final step.
- Update `.specify/wiki/human-reference/autonomous-self-training-cycle.md` and corresponding `.wiki/Autonomous-Self-Training.md` page; sync via `scripts/workflow-sync-wiki.sh`.
- Add a release note to `docs/release-notes/` describing the neuro layer and how to opt-in/out via `--neuro-profile`.

Validation:
- 10 sample dry-run dispatches across the two workflows; all green for `validate-ai-contracts` and `validate-workflow-dependencies` (SC-004).
- Anchor-set regression test passes for not-banana over 4 simulated weekly cycles in CI profile (SC-002 acceptance).
- Wiki sync produces no diff drift between `.specify/wiki/` and `.wiki/`.

## Cross-Wave Concerns

- Concurrency: every new workflow declares `concurrency: { group: neuro-train-${{ inputs.model || matrix.model }}, cancel-in-progress: true }`.
- Determinism: all new Python scripts seed RNGs from `BANANA_NEURO_SEED` (default 42) so CI runs are reproducible.
- Security: no new permissions; reuse the existing PR-opener token contract; replay buffers contain IDs and label/confidence only, never raw PII payloads.
- Backwards compatibility: `--neuro-profile off` (default) keeps the legacy training behavior bit-for-bit; existing `Train: *` tasks are not modified.

## Risks & Mitigations

- Risk: Replay buffer file growth on busy weeks. Mitigation: reservoir cap (FR-011) + nightly NREM prune step.
- Risk: EWC term destabilizes early runs with cold-start Fisher. Mitigation: `ewc=cold-start` path returns zero penalty (edge case in spec) until 2 consolidation passes have produced snapshots.
- Risk: Surprise weighting collapses to a few items if confidence is bimodal. Mitigation: blend weight = `0.7 * uniform + 0.3 * surprise` floor; assert in unit test.
- Risk: AI-contract validator regression if a marker drifts. Mitigation: explicit marker assertion in the workflow's own pre-flight step (`grep -q "agent:banana-classifier-agent" "$GITHUB_WORKFLOW_REF"` style check).
