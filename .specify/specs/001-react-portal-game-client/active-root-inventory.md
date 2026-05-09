# Active Root Inventory

## Purpose

Capture active workflow/spec roots and classify entries against the Banana Engineer pivot scope.

## Snapshot Date

- 2026-05-09

## Workflow Root Inventory (.github/workflows)

- banana.yml -> in-scope (canonical monorepo harness)
- ai-contract-guard.yml -> in-scope (contract governance)
- backup-dr.yml -> in-scope (operations runbook support)
- orchestrate-*.yml lanes -> in-scope only when scoped to active spec execution contracts
- legacy or duplicate top-level workflow surfaces -> out-of-scope for pivot and should be archived or retired

## Spec Root Inventory (.specify/specs)

- 001-react-portal-game-client -> in-scope primary deliverable
- 002-banana-monorepo-harness -> in-scope supporting governance baseline
- 003-retire-self-training-surfaces -> in-scope cleanup dependency for pivot safety
- 004-wiki-rescaffold-baseline -> in-scope tutorial/runbook clarity dependency
- 005-confidence-heartbeat-orchestration -> in-scope orchestration safety dependency
- 006-horizontal-spec-orchestration-scaleout -> in-scope throughput dependency

## Out-of-Scope Entry Types

- Research-era docs/workflows that do not support Banana Engineer delivery or required governance controls
- Runtime telemetry/data dumps tracked in wiki/docs roots
- Duplicate workflow lanes that bypass the canonical Banana-Monorepo harness

## Retirement/Archive Candidate Buckets

- Move inactive historical AI surfaces under `.specify/legacy-baseline/**` only.
- Keep active roots focused on product delivery, safety contracts, and required runbooks.

## Evidence Commands

```bash
find .specify/specs -maxdepth 2 -type f | sort
find .github/workflows -maxdepth 2 -type f | sort
```
