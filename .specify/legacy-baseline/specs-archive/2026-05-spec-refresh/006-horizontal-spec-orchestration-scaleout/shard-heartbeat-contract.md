# Shard Heartbeat Contract

## Required Fields Per Step

- `timestampUtc`: ISO-8601 timestamp.
- `shardId`: shard lane identifier.
- `featureDirectory`: active feature path for this step.
- `step`: short step label.
- `confidence`: integer percentage [0..100].
- `decision`: `continue` or `pause`.
- `humanCheckpoint`: `yes` or `no`.
- `notes`: concise rationale.

## Storage Paths

- Default per-shard path:
  - `.specify/specs/<feature>/heartbeat-log.md`
- Optional aggregate stream path:
  - `artifacts/spec-validation/shard-heartbeats/<shard-id>.md`

## Enforcement

- If `confidence < 80`, decision must be `pause` and `humanCheckpoint` must be `yes`.
- If `confidence >= 80`, autonomous `continue` is allowed.
