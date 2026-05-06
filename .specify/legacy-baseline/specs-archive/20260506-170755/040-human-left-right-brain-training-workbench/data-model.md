# Data Model: Human Left/Right Brain Training Workbench

## Entity: TrainingRunRequest

- request_id
- operator_id
- lane: left-brain | right-brain
- corpus_source
- training_profile
- session_mode
- max_sessions
- notes

## Entity: TrainingRunResult

- run_id
- request_id
- status: running | passed | failed
- metrics
- threshold_outcome
- run_fingerprint
- generated_at_utc
- persisted_session_ref

## Entity: PromotionAction

- action_id
- run_id
- operator_id
- action: mark-candidate | promote-stable
- decision_reason
- timestamp_utc

## Validation rules

- lane must be one of left-brain or right-brain.
- request fields must map to supported trainer arguments.
- promotion requires run status passed and threshold_outcome pass.
