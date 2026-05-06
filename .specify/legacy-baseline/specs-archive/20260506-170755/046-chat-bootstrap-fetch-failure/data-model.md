# Data Model: Chat Bootstrap Fetch Failure Follow-up

## Entity: ChatBootstrapState

- Description: Frontend lifecycle state for chat session initialization.
- Fields:
  - phase: `idle | resolving_config | bootstrapping | ready | failed`
  - session_id: nullable session identifier
  - last_error_type: nullable error classification
  - chat_enabled: boolean gate for prompt/send controls
  - retry_token: monotonic token/timestamp for retry attempts
- Validation Rules:
  - `ready` requires non-empty `session_id` and `chat_enabled = true`.
  - `failed` requires `last_error_type` to be set.
  - retries must increment/refresh `retry_token` before another bootstrap attempt.

## Entity: BootstrapTransportErrorState

- Description: Error payload for endpoint-unreachable bootstrap failures.
- Fields:
  - type: `bootstrap_transport`
  - title: short user-visible label
  - message: actionable description of fetch/endpoint failure
  - remediation_steps: ordered list aligned to canonical runtime/apps startup
  - clear_condition: runtime/API reachable and bootstrap retry succeeds
- Validation Rules:
  - `type` must remain distinct from `config_resolution`.
  - remediation must reference runtime-first startup order.

## Entity: RuntimeDependencyReadiness

- Description: Runtime availability contract consumed by bootstrap flow.
- Fields:
  - api_base_url: effective resolved API base
  - runtime_ready: boolean readiness signal for runtime profile
  - apps_ready: boolean readiness signal for apps profile
  - checked_at: timestamp of latest readiness probe
- Validation Rules:
  - bootstrap may proceed only when effective `api_base_url` is present.
  - transport failure classification applies when `api_base_url` is resolved but endpoint remains unreachable.

## Relationships

- `RuntimeDependencyReadiness` gates transitions in `ChatBootstrapState`.
- `ChatBootstrapState.failed` references `BootstrapTransportErrorState` when fetch transport failures occur.
- Successful readiness restoration transitions `ChatBootstrapState` from `failed` to `bootstrapping`, then `ready`.

## State Transition Notes

- `idle -> resolving_config`: page/app bootstrap starts.
- `resolving_config -> bootstrapping`: effective API base resolved.
- `bootstrapping -> failed`: fetch/transport failure or non-recoverable bootstrap error.
- `failed -> bootstrapping`: retry/reload after remediation.
- `bootstrapping -> ready`: session created and chat controls enabled.
