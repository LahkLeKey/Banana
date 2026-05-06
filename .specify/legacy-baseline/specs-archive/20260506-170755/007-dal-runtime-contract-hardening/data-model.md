# Data Model: DAL Runtime Contract Hardening

**Branch**: `feature/006-c-upstream-value-spike` | **Phase**: 1 | **Date**: 2026-04-26

## Entities

### `DalOutcomeCategory`

**Purpose**: Normalize DAL result categories in scope for hardening.

| Field | Type | Constraint |
|-------|------|------------|
| `categoryId` | `string` | Unique, non-empty |
| `nativeStatus` | `string` | Must map to a valid native status enum entry |
| `description` | `string` | Non-empty |
| `isSuccess` | `boolean` | `true` only for valid configured query success |
| `remediationHint` | `string` | Required for non-success categories |

Representative categories in this feature:

- `unconfigured`
- `dependency_unavailable`
- `query_failure`
- `success`

### `DalStatusMappingRecord`

**Purpose**: Define deterministic native-to-managed mapping expectations.

| Field | Type | Constraint |
|-------|------|------------|
| `nativeStatus` | `string` | Must reference DAL status in scope |
| `httpStatus` | `number` | Valid HTTP status code |
| `errorCode` | `string` | Non-empty for non-success |
| `remediation` | `string` | Required when status is operationally actionable |
| `mappingOwner` | `string` | Expected layer owner (`pipeline`) |

### `DalValidationEvidence`

**Purpose**: Capture required lane-level validation signals for release readiness.

| Field | Type | Constraint |
|-------|------|------------|
| `lane` | `string` | One of `tests/native`, `tests/unit`, `tests/e2e-contracts` |
| `scenario` | `string` | Non-empty, maps to acceptance intent |
| `expectedSignal` | `string` | Non-empty deterministic result |
| `observedResult` | `string` | Pass/fail plus summary evidence |

## Relationships

- One `DalOutcomeCategory` must map to one `DalStatusMappingRecord`.
- Each in-scope `DalOutcomeCategory` must be covered by at least one `DalValidationEvidence` record.

## State Notes

- Outcome flow: `configured-check -> dependency-check -> query-execution -> mapped-response`.
- Non-success outcomes are terminal and must include deterministic remediation where applicable.
