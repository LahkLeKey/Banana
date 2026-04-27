# Data Model: CI Training Session Persistence

## Entity: TrainingSessionRecord

- session_id: stable unique id for this persisted session
- lane: banana | not-banana | ripeness
- corpus_id: identity/hash/version of corpus input
- training_profile: ci | local
- session_mode: single | multi
- max_sessions: integer
- selected_session: integer
- thresholds: object containing lane threshold values used
- metrics: object containing lane metrics (f1, precision, recall, accuracy, etc. as applicable)
- run_fingerprint: deterministic fingerprint for reproducibility
- generated_at_utc: stable timestamp strategy used by trainer
- artifact_paths: workspace-relative paths only

## Entity: LaneSessionIndex

- lane: training lane key
- latest_session_id: reference to newest appended session
- entries: ordered references to session records

## Validation rules

- All records require lane, corpus_id, run_fingerprint, metrics, and thresholds.
- artifact_paths must be relative paths.
- index must preserve append order; prior entries cannot be mutated in-place.
