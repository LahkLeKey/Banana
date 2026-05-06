# Contract: Training Session Persistence

## Required fields

- session_id
- lane
- corpus_id
- training_profile
- session_mode
- max_sessions
- selected_session
- thresholds
- metrics
- run_fingerprint
- generated_at_utc
- artifact_paths

## Contract invariants

- artifact_paths must be repository-relative.
- records are append-only; existing historical records must not be rewritten.
- each new CI run appends one new record per lane run.

## Compatibility

- Existing trainer CLI arguments remain unchanged.
- Lane-specific metric details may expand without removing existing required keys.
