# Data Model: Coverage 80 Rehydration

## Entity: CoverageLaneResult

Represents normalized coverage outcome for a single merge-gated lane.

| Field | Type | Required | Notes |
|---|---|---|---|
| lane_id | string | yes | Canonical lane key (`native-unit`, `dotnet-integration`, `api-react-e2e`, etc.) |
| domain | enum | yes | `native`, `aspnet`, `typescript-api`, `react`, `electron`, `react-native`, `shared-ui` |
| layer | enum | yes | `unit`, `integration`, `e2e` |
| status | enum | yes | `pass`, `fail`, `skip`, `not-applicable` |
| failure_class | enum | no | `threshold_violation`, `coverage_contract_violation`, `preflight_contract_violation`, `flake_contract_violation` |
| measured_percent | number | no | Normalized measured coverage percent |
| threshold_percent | number | yes | Expected threshold (80.0) |
| deficit_percent | number | no | `threshold_percent - measured_percent` when below threshold |
| denominator_policy_version | string | yes | Version identifier for exclusion/not-applicable rules |
| started_at | datetime | no | Lane start timestamp |
| finished_at | datetime | no | Lane finish timestamp |
| evidence_bundle_path | string | yes | Repository-relative path to coverage evidence bundle |

### Validation Rules

- `status=pass` requires `measured_percent >= threshold_percent` and no `failure_class`.
- `status=fail` requires `failure_class` and either `deficit_percent > 0` or contract/preflight/flake failure evidence.
- `status=skip` requires explicit skip reason and must not be used for expected merge-gated tuple surfaces unless policy allows.
- `status=not-applicable` requires policy-backed rationale and tuple-level applicability metadata.
- If both timestamps exist, `finished_at >= started_at`.
- `evidence_bundle_path` must be workspace-relative and path-safe per 002 conventions.

## Entity: CoverageTupleStatus

Represents one normalized `(domain, layer)` summary across one or more lanes.

| Field | Type | Required | Notes |
|---|---|---|---|
| domain | enum | yes | Same domain taxonomy as lane result |
| layer | enum | yes | `unit`, `integration`, `e2e` |
| applicable | boolean | yes | Whether tuple is in scope for enforcement |
| status | enum | yes | `pass`, `fail`, `not-applicable` |
| measured_percent | number | no | Tuple-level normalized value when applicable |
| threshold_percent | number | no | Usually `80.0` when applicable |
| failing_lanes | string[] | no | Lane IDs causing tuple failure |
| rationale | string | no | Required when `applicable=false` |

### Validation Rules

- `applicable=false` requires `status=not-applicable` and non-empty rationale.
- `applicable=true` requires status in `pass|fail` and threshold value.
- `failing_lanes` must be non-empty when `status=fail`.

## Entity: CoverageAggregateSummary

Run-level rollup that supports first-pass triage.

| Field | Type | Required | Notes |
|---|---|---|---|
| run_id | string | yes | Workflow run identifier |
| commit_sha | string | yes | Commit being evaluated |
| tuple_statuses | CoverageTupleStatus[] | yes | Full set of evaluated tuples |
| failed_tuple_count | integer | yes | Count of failed tuples |
| failure_counts_by_layer | object | yes | Map from layer to failure count |
| overall_status | enum | yes | `pass` or `fail` |
| generated_at | datetime | yes | Summary generation timestamp |

### Validation Rules

- Must include all applicable tuples for the run.
- Failed tuples must be listed first in human-readable form.
- `overall_status=pass` only when all applicable tuples pass or are not-applicable by policy.

## Entity: CoverageDenominatorPolicy

Defines denominator inclusion/exclusion rules.

| Field | Type | Required | Notes |
|---|---|---|---|
| policy_version | string | yes | Semantic version or date-based policy id |
| include_roots | string[] | yes | Paths eligible for coverage denominator |
| exclude_globs | string[] | yes | Excluded patterns (generated/vendor/third-party/build) |
| not_applicable_rules | object[] | yes | Rule definitions for inapplicable tuples |
| rounding_rule | string | yes | Canonical threshold comparison rule |
| owner | string | yes | Maintaining team or owner |

### Validation Rules

- `exclude_globs` must be applied consistently by all lane normalizers.
- `rounding_rule` must be stable across all lane comparisons.
- Any policy change requires evidence snapshots to retain auditability.

## Entity: E2EFlowManifest

Declares e2e flow denominator for a runtime channel.

| Field | Type | Required | Notes |
|---|---|---|---|
| channel | enum | yes | `api-react`, `electron`, `mobile` |
| manifest_version | string | yes | Versioned manifest identifier |
| declared_flows | object[] | yes | Flow IDs and descriptions |
| required_flow_count | integer | yes | Number of flows expected |
| exercised_flow_ids | string[] | no | Populated from run evidence |
| exercised_percent | number | no | Computed flow coverage percentage |

### Validation Rules

- `required_flow_count` must equal count of `declared_flows`.
- `exercised_percent` must derive from exercised/declaration ratio using policy rounding.

## Entity: CoverageExceptionRecord

Temporary exception metadata for one targeted tuple.

| Field | Type | Required | Notes |
|---|---|---|---|
| exception_id | string | yes | Unique id |
| domain | enum | yes | Target domain |
| layer | enum | yes | Target layer |
| lane_id | string | no | Optional specific lane target |
| owner | string | yes | Accountable owner |
| rationale | string | yes | Business/technical reason |
| remediation_plan | string | yes | Planned remediation steps |
| expires_on | date | yes | Expiration date |
| created_at | datetime | yes | Creation timestamp |

### Validation Rules

- Missing required fields invalidate the exception.
- Expired exceptions are ignored for suppression and surfaced as policy violations.
- Exception must map to known tuple and optional known lane.

## Entity: CoverageContractEvidenceBundle

Lane-scoped evidence set used for diagnostics and audits.

| Field | Type | Required | Notes |
|---|---|---|---|
| lane_id | string | yes | Foreign key to lane result |
| raw_coverage_paths | string[] | yes | Raw coverage files produced by lane tooling |
| normalized_result_path | string | yes | Normalized lane result artifact |
| denominator_policy_snapshot_path | string | yes | Policy snapshot used during run |
| exception_snapshot_path | string | no | Exception state snapshot when exceptions are in scope |
| diagnostics_paths | string[] | no | Logs/manifests needed for triage |

### Validation Rules

- All paths must be workspace-relative and path-safe.
- `normalized_result_path` must parse and validate against lane contract schema.

## Relationships

- One `CoverageLaneResult` has one `CoverageContractEvidenceBundle`.
- One `CoverageAggregateSummary` includes many `CoverageTupleStatus` entries.
- One `CoverageDenominatorPolicy` governs many lane and aggregate evaluations.
- One `CoverageExceptionRecord` targets one tuple (optionally one lane).
- One `E2EFlowManifest` is consumed by one or more e2e lane results.

## State Transitions

### Lane lifecycle

`queued -> running -> pass|fail|skip|not-applicable`

- `queued -> running`: lane begins execution.
- `running -> pass`: threshold met and contract artifacts valid.
- `running -> fail`: threshold or contract/preflight/flake classification triggers failure.
- `running -> skip`: explicit policy-approved skip path.
- `running -> not-applicable`: tuple/lane applicability rule resolves false.

### Exception lifecycle

`draft -> active -> expired -> removed`

- `draft -> active`: required metadata validated.
- `active -> expired`: expiration date reached.
- `expired -> removed`: cleanup after remediation completion.
