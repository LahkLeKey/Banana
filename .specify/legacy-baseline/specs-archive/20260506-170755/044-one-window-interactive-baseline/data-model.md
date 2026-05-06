# Data Model: One-Window Interactive Baseline

## Entity: OneWindowValidationLoop

- Description: Canonical sequence for frontend/Electron interactive validation entirely inside VS Code.
- Fields:
  - startup_entrypoint: canonical compose tasks/scripts used to boot runtime + app profiles.
  - interactive_surface: VS Code integrated browser URL and channel.
  - rendered_checks: required on-screen assertions proving runtime behavior.
  - diagnostics_entrypoint: command/task for drift diagnostics from same workspace.
  - teardown_entrypoint: canonical stop sequence for apps/runtime profiles.
- Validation Rules:
  - startup_entrypoint must reference existing compose profile scripts/tasks.
  - interactive_surface must be in-workspace and not require external browser.
  - rendered_checks must include at least one user-visible state assertion.

## Entity: InteractiveClosureEvidence

- Description: Evidence bundle required to close frontend/Electron tasks.
- Fields:
  - feature_task_id: task identifier in tasks.md.
  - rendered_state_observed: recorded result of integrated browser validation.
  - diagnostics_status: pass/fail and summary from drift diagnostic flow.
  - runtime_profile_context: profiles used during validation.
- Validation Rules:
  - rendered_state_observed cannot be empty for frontend/Electron behavior tasks.
  - diagnostics_status must indicate either pass or documented remediation path.

## Entity: DriftSignal

- Description: Detectable mismatch between declared runtime contract and rendered behavior.
- Fields:
  - layer: launch-task, compose-interpolation, container-env, or rendered-runtime.
  - symptom: observed mismatch (for example unresolved API base).
  - remediation: deterministic next step to recover parity.
- Validation Rules:
  - each signal must map to one actionable remediation.
  - rendered-runtime signals cannot be considered resolved by env-only checks.

## Relationships

- OneWindowValidationLoop produces InteractiveClosureEvidence.
- DriftSignal instances are captured during OneWindowValidationLoop diagnostics step.
- InteractiveClosureEvidence references the DriftSignal outcome when failures are found.
