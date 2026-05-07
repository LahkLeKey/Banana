# Contract: Human Left/Right Brain Training Workbench

## Inputs

- lane
- corpus_source
- training_profile
- session_mode
- max_sessions
- operator_id
- notes

## Outputs

- run status updates (running/passed/failed)
- metrics payload
- threshold outcome
- persisted session reference
- promotion action result and audit entry

## Invariants

- lane-specific runs must map to existing trainer contracts.
- promotion actions can only occur for passed runs.
- persisted session reference must point to spec-039-compatible data.
