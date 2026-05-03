---
name: drain-specs-memory-first
description: Deterministically scaffold feedback-loop spikes, drain specs to completion, and update constitution using checkpoint-first low-compute/high-memory execution.
argument-hint: Optionally provide scope constraints, checkpoint path override, or promotion-priority hints.
agent: banana-sdlc
---

Execute this intent deterministically for Banana:

"scaffold via spike to promote feedback loop then drain all specs until done, update constitution step by step as you scan to increase bootstrapping, your using alot of compute right now over memory"

## Execution Contract (Low-Compute / High-Memory)

- Treat this prompt as checkpoint-first by default.
- Reuse existing state artifacts before any full scan:
  - `artifacts/sdlc-orchestration/spec-drain-state-all-accounted.json`
  - `artifacts/orchestration/spec-drain-all/local-run/drain-summary-accounted.json`
- Perform delta accounting only (new/unaccounted specs) unless checkpoint is missing or corrupt.
- Do not run repeated full drain loops when state can be resumed or patched deterministically.

## Deterministic Flow

1. Read checkpoint and compute current delta:
   - Count specs with both `spec.md` and `tasks.md`.
   - Compare against `completed_specs` + `failed_specs`.
2. If feedback-loop promotion spike is missing, scaffold it first as a new spec/tasks pair.
3. Update constitution incrementally (step-by-step) only for newly observed reusable patterns.
4. Account all unaccounted runnable specs in state (or explicitly defer with reason).
5. Regenerate compact accounted summary artifact.
6. Emit promotion ledger artifact for follow-up slices.
7. Validate parity for newly created spikes.
8. Return final deterministic status:
   - `completed_count`, `deferred_count`, `all_specs_with_tasks`, `accounted_total`, `unaccounted_count`.

## Stop Conditions

- Stop only when `unaccounted_count == 0`, or when blocked by missing/corrupt checkpoint and no safe recovery path.
- If blocked, return a single actionable remediation path (no repeated scans).

## Required Output Artifacts

- `artifacts/orchestration/spec-drain-all/local-run/drain-summary-accounted.json`
- `artifacts/orchestration/spec-drain-all/local-run/promotion-ledger.json`
- Updated constitution when new bootstrap rules are discovered.

## Governance Notes

- Preserve memory-first bootstrap behavior from constitution principles.
- Keep changes scoped and machine-readable.
- Prefer explicit deferred-ledger closure over re-running expensive full scans.

## Wiki Updater Contract

- Treat this prompt as a wiki-synced contract surface: when prompt behavior, scope, assets, or acceptance criteria changes, keep the corresponding wiki snapshot current in the same SDLC run.
- Use `scripts/workflow-sync-wiki.sh` (or SDLC orchestration wrappers) so prompt updates and wiki docs are delivered together.
- If a prompt change introduces new automation flow, branch policy, validation step, or contract dependency, update prompt-linked wiki content before closing the change.
