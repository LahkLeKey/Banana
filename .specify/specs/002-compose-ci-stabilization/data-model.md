# Data Model: Compose CI Stabilization

## Entity: ComposeCILaneResult

Represents the terminal outcome for a single merge-gated lane.

| Field | Type | Required | Notes |
|---|---|---|---|
| lane_name | string | yes | Canonical lane key (`compose-tests`, `compose-runtime`, `compose-electron`, `e2e-smoke`, etc.) |
| profile | string | yes | Compose profile or runtime surface associated with lane |
| stage | string | yes | Stage where terminal outcome was decided (`preflight`, `compose-up`, `health-check`, `test-run`, `artifact-upload`) |
| status | enum | yes | `pass`, `fail`, `skip` |
| exit_code | integer | no | Populated for executable command terminal states |
| failing_service | string | no | Service name when failure maps to a specific compose service |
| reason_code | string | yes | Stable classification (`timeout`, `permission`, `missing_optional_surface`, `runtime_deprecation`, `unknown`) |
| started_at | datetime | yes | Lane start timestamp |
| finished_at | datetime | yes | Lane end timestamp |
| diagnostics_bundle_path | string | yes | Relative path to lane diagnostics bundle |

### Validation Rules

- `status=pass` requires `exit_code=0` when command execution occurred.
- `status=fail` requires non-empty `stage` and `reason_code`.
- `status=skip` requires `reason_code` that explains skip semantics.

## Entity: ComposeDiagnosticsBundle

Structured evidence package for one lane.

| Field | Type | Required | Notes |
|---|---|---|---|
| lane_name | string | yes | Foreign key to ComposeCILaneResult |
| compose_ps_path | string | no | Service snapshot (`docker compose ps`) |
| compose_logs_paths | string[] | no | One or more lane/service log files |
| health_payload_paths | string[] | no | Captured endpoint payloads when applicable |
| preflight_report_path | string | no | Preflight validation output (permissions, scripts, env) |
| manifest_path | string | yes | Artifact manifest file for expected/produced outputs |

### Validation Rules

- `manifest_path` must always exist.
- At least one diagnostics artifact must exist for `fail` and `skip` outcomes.

## Entity: ArtifactPublicationRecord

Tracks artifact expectations and publication outcomes.

| Field | Type | Required | Notes |
|---|---|---|---|
| artifact_name | string | yes | Upload artifact identifier |
| lane_name | string | yes | Associated lane |
| expected_paths | string[] | yes | Required or optional evidence paths |
| produced_paths | string[] | yes | Paths generated prior to upload step |
| optional_surface | boolean | yes | Whether the artifact corresponds to optional scope |
| publication_status | enum | yes | `uploaded`, `skipped`, `fallback-uploaded`, `failed` |
| skip_reason | string | no | Required when `publication_status=skipped` |

### Validation Rules

- `publication_status=failed` must include a reason and lane diagnostics pointer.
- Missing optional paths must map to `fallback-uploaded` or `skipped`, never hard failure.

## Entity: WorkflowRuntimeCompatibilityRecord

Captures action runtime compatibility for merge-gated workflows.

| Field | Type | Required | Notes |
|---|---|---|---|
| workflow_name | string | yes | Workflow being validated |
| action_ref | string | yes | Action identifier and version |
| runtime_warning_detected | boolean | yes | Whether deprecation warning appeared |
| exception_allowed | boolean | yes | Whether warning has approved temporary exception |
| exception_owner | string | no | Required when `exception_allowed=true` |
| target_remediation_date | date | no | Required when `exception_allowed=true` |
| verification_run_id | string | yes | Run used for validation |

### Validation Rules

- If `runtime_warning_detected=true`, either action is upgraded in same slice or a bounded exception is recorded with owner/date.
- Merge-gated state must not be left with unowned deprecation warnings.

## Relationships

- One `ComposeCILaneResult` has one `ComposeDiagnosticsBundle`.
- One `ComposeCILaneResult` has one or more `ArtifactPublicationRecord` entries.
- One workflow run may have multiple `WorkflowRuntimeCompatibilityRecord` entries (per action reference).

## State Transitions

`queued -> running -> pass|fail|skip`

- `queued -> running`: lane starts execution.
- `running -> fail`: command or preflight error produces terminal failure.
- `running -> skip`: optional surface absence is detected and explicitly recorded.
- `running -> pass`: lane completes and required artifacts are published.
