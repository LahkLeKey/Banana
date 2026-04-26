# Contract: Monorepo Coverage 80 Lane Contract

## Purpose

Define deterministic coverage gate behavior, normalized lane payload shape, denominator governance, and aggregate reporting requirements for feature `003-coverage-80-rehydration`.

## Coverage Lane Inventory (Merge-Gated)

| Layer | Domain Surface | Example Lane IDs | Expected Terminal States |
|---|---|---|---|
| unit | native, aspnet, typescript-api, react, electron, react-native, shared-ui | `native-unit`, `dotnet-unit`, `ts-api-unit`, `react-unit`, `electron-unit`, `mobile-unit`, `shared-ui-unit` | pass, fail, not-applicable |
| integration | native, aspnet, typescript-api, cross-layer interop | `native-integration`, `dotnet-integration`, `ts-api-integration`, `interop-integration` | pass, fail, not-applicable |
| e2e | api-react channel, electron channel, mobile channel | `api-react-e2e`, `electron-e2e`, `mobile-e2e` | pass, fail, not-applicable |

## Terminal State Contract

Each evaluated lane must publish a normalized lane result with fields:

- `lane_id`
- `domain`
- `layer`
- `status` in `{pass, fail, skip, not-applicable}`
- `threshold_percent`
- `measured_percent` (when applicable)
- `failure_class` (required when fail)
- `evidence_bundle_path`

### Rules

- `pass`: requires measured value at or above threshold and valid evidence bundle.
- `fail`: requires one failure class and attributable deficit or contract evidence.
- `skip`: only valid for policy-sanctioned temporary behavior; not valid as silent substitute for required lane coverage.
- `not-applicable`: requires tuple applicability rationale via denominator policy.
- Missing or invalid normalized payload is a `coverage_contract_violation`.

## Failure Classification Contract

Failing lanes must use one of these classes:

- `threshold_violation`
- `coverage_contract_violation`
- `preflight_contract_violation`
- `flake_contract_violation`

Free-form failure classes are contract violations.

## Denominator Policy Contract

Coverage percentages are valid only when policy contract is attached.

### Required policy dimensions

- include roots
- exclude globs (generated, vendored, third-party, build artifacts)
- not-applicable tuple rules
- rounding rule
- policy version

### Rules

- All lane evaluators must apply the same policy version for one run.
- Policy snapshot path must be present in every evidence bundle.
- Policy drift across lanes in one run is a contract violation.

## E2E Flow Manifest Contract

E2e lanes must compute denominator using a machine-readable flow manifest.

### Machine-readable format

```json
{
  "channel": "api-react",
  "manifest_version": "v1",
  "declared_flows": [
    { "flow_id": "api-health", "description": "api-health" },
    { "flow_id": "api-banana", "description": "api-banana" }
  ],
  "required_flow_count": 2,
  "exercised_flow_ids": ["api-health", "api-banana"],
  "exercised_percent": 100.0
}
```

### Required fields

- `channel` in `{api-react, electron, mobile}`
- `manifest_version`
- `declared_flows` (array of `{flow_id, description}`)
- `required_flow_count`
- `exercised_flow_ids`
- `exercised_percent`

### Rules

- `required_flow_count` must equal declared flow count.
- `exercised_percent` must use canonical rounding rule.
- Missing manifest for required e2e lane is a `coverage_contract_violation`.
- E2e lane status `pass` requires `exercised_percent >= threshold_percent`.
- E2e lane status `fail` with `exercised_percent < threshold_percent` must use `threshold_violation`.

## Exception Contract

Exceptions allow temporary suppression only with full accountability metadata.

### Required fields

- `exception_id`
- `domain`
- `layer`
- `owner`
- `rationale`
- `remediation_plan`
- `expires_on`

### Rules

- Malformed or incomplete exception records are invalid.
- Expired exceptions never suppress failures.
- Exception suppression applies only to targeted tuple/lane.
- Active exceptions must appear in lane and aggregate summaries.

## Aggregate Summary Contract

Each run must publish one aggregate summary that includes all applicable tuples.

### Required fields

- `run_id`
- `commit_sha`
- `tuple_statuses`
- `failed_tuple_count`
- `failure_counts_by_layer`
- `overall_status`
- `generated_at`

### Rules

- Failed tuples listed first in human-readable output.
- Every applicable tuple must appear exactly once.
- Aggregate pass is valid only when all applicable tuples pass or are policy-valid not-applicable.

## Evidence Bundle Contract

Every lane must emit one path-safe bundle under repository artifact root.

### Minimum required bundle contents

- normalized lane result artifact
- raw coverage files or raw source references
- denominator policy snapshot
- lane diagnostics (when fail)
- exception snapshot (when in scope)

### Portability rules

- All paths are workspace-relative.
- Machine-specific absolute paths are prohibited.
- Path layout must remain compatible with 002 artifact conventions.

## Validation Mapping

- FR-001..FR-007: baseline and determinism contract
- FR-008..FR-011: denominator governance
- FR-012..FR-028: layer-specific enforcement and e2e denominator manifest
- FR-029..FR-032: failure taxonomy and reporting contract
- FR-033..FR-037: exception governance
- FR-038..FR-040: portability and parity with 002
