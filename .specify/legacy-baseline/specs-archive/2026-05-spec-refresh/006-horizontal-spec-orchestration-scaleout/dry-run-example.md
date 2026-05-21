# Dry-Run Example: Multi-Shard Plan

## Input Features

- `.specify/specs/003-retire-self-training-surfaces`
- `.specify/specs/004-wiki-rescaffold-baseline`
- `.specify/specs/005-confidence-heartbeat-orchestration`

## Deterministic Shard Assignment

- Parallel shard `shard-1`: `.specify/specs/003-retire-self-training-surfaces`
- Parallel shard `shard-2`: `.specify/specs/004-wiki-rescaffold-baseline`
- Sequential shard `shard-seq`: `.specify/specs/005-confidence-heartbeat-orchestration`

Reason for sequenced lane:
- `005` mutates constitution/templates shared governance contracts and is conflict-sensitive.

## Generated Evidence Paths

- `artifacts/spec-validation/006-shard-plan.json`
- `artifacts/spec-validation/006-shard-merge-report.json`

## Example Aggregate Report (abbreviated)

```json
{
  "generatedAtUtc": "2026-05-09T18:24:07Z",
  "version": "1.0.0",
  "ordering": "lexicographic-feature-directory",
  "shardOutcomes": [
    {
      "shardId": "shard-1",
      "status": "passed",
      "features": [".specify/specs/003-retire-self-training-surfaces"],
      "evidencePaths": ["artifacts/spec-validation/006-shard-plan.json"]
    },
    {
      "shardId": "shard-2",
      "status": "passed",
      "features": [".specify/specs/004-wiki-rescaffold-baseline"],
      "evidencePaths": ["artifacts/spec-validation/006-shard-plan.json"]
    },
    {
      "shardId": "shard-seq",
      "status": "sequenced",
      "features": [".specify/specs/005-confidence-heartbeat-orchestration"],
      "evidencePaths": ["artifacts/spec-validation/006-shard-merge-report.json"]
    }
  ],
  "blockers": [
    {
      "type": "conflict-sequenced",
      "shardId": "shard-seq",
      "summary": "Shared governance contract updates routed to sequential lane"
    }
  ],
  "overallStatus": "partial"
}
```
