# Data Model: Frontend Runtime Unset Follow-up

## Entity: ApiBaseResolutionState

- Description: Effective API-base state consumed by frontend runtime.
- Fields:
  - raw_sources: candidate inputs from web env and preload bridge.
  - normalized_value: trimmed candidate value after normalization.
  - status: resolved, unresolved, or invalid.
  - reason: explanation for unresolved/invalid status.
- Validation Rules:
  - `resolved` requires non-empty normalized URL-like value.
  - whitespace-only values are invalid.
  - unresolved must include a remediation-oriented reason.

## Entity: FrontendRuntimeErrorState

- Description: User-visible error state rendered in Banana v2.
- Fields:
  - error_type: config_resolution, bootstrap_transport, bootstrap_api, or unknown.
  - title: short heading shown in UI.
  - message: main explanatory text.
  - remediation: deterministic next step(s).
  - clear_condition: condition under which error is removed.
- Validation Rules:
  - `config_resolution` errors must map to API-base resolution failure.
  - bootstrap failures must not be mislabeled as config errors.

## Entity: BootstrapSessionState

- Description: Session lifecycle for chat readiness.
- Fields:
  - phase: idle, resolving_config, bootstrapping, ready, failed.
  - session_id: current session identifier when available.
  - chat_enabled: whether input/send controls are enabled.
  - last_failure_type: failure classification used for UI messaging.
- Validation Rules:
  - `ready` requires non-empty session_id and chat_enabled true.
  - after config remediation and successful bootstrap, state must transition to `ready`.

## Relationships

- ApiBaseResolutionState drives whether FrontendRuntimeErrorState enters `config_resolution`.
- BootstrapSessionState depends on ApiBaseResolutionState being `resolved`.
- BootstrapSessionState failure classification selects FrontendRuntimeErrorState error_type.
