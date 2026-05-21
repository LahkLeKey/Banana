# Blocker Taxonomy

## checkpoint-pending

- Definition: shard paused because confidence dropped below 80% and awaits human decision.
- Required fields: shardId, step, confidence, requestedInput.
- Resolution: capture human input and rerun shard gate.

## extension-unhealthy

- Definition: shard cannot be admitted because required extension health preflight failed.
- Required fields: shardId, extensionId(s), remediationAttempted, defermentRef.
- Resolution: repair extension or provide valid deferment contract.

## conflict-sequenced

- Definition: shard candidate cannot run in parallel due to shared-contract/file mutation conflict.
- Required fields: shardId, conflictingFeature, conflictPath(s).
- Resolution: route candidate to sequential lane and preserve deterministic ordering.
