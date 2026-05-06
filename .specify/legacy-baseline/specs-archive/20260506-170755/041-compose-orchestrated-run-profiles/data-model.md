# Data Model: Compose-Orchestrated Run Profiles

## Entity: RunProfileContract

- profile_id
- profile_name
- compose_profiles (ordered list)
- required_services (service names)
- optional_services (service names)
- required_env_contracts
- startup_entrypoint
- shutdown_entrypoint
- readiness_checks
- validation_lane

## Entity: OrchestrationEntrypoint

- entrypoint_id
- script_path
- execution_context (windows-shell, ubuntu-wsl2, container shell)
- compose_invocation_pattern
- prerequisite_checks
- failure_exit_codes
- fallback_behavior

## Entity: ProfileReadinessCheck

- check_id
- profile_id
- check_type (health endpoint, compose status, service log gate)
- success_condition
- timeout_budget
- retry_policy
- failure_message_template

## Entity: ReproducibilityRun

- run_id
- profile_id
- attempt_index
- startup_timestamp
- ready_timestamp
- shutdown_timestamp
- result (pass/fail)
- drift_reason

## Entity: TroubleshootingMapping

- mapping_id
- failure_class
- detection_signal
- remediation_steps
- escalation_path

## Validation rules

- Every supported RunProfileContract maps to at least one compose profile.
- Every profile has explicit startup and shutdown entry points.
- Every profile has at least one deterministic readiness check.
- ReproducibilityRun evaluations use the same profile contract definition across attempts.
- Troubleshooting mappings exist for prerequisite, readiness timeout, and dependency-health failure classes.
