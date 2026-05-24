# Data Model: Native Runtime Render Modularization

## Overview

This feature models architectural boundaries and validation artifacts rather than product data. The entities below define how the refactor is planned, validated, and governed.

## Entity: TickPhase

| Field | Type | Description |
|-------|------|-------------|
| `name` | string | Stable phase identifier such as `input`, `simulation`, `budget`, or `post` |
| `owner_module` | path | Native runtime file that implements the phase |
| `inputs` | list | Context and function pointers consumed by the phase |
| `outputs` | list | Side effects or callback invocations emitted by the phase |
| `parity_gate` | string | Focused test command proving behavior parity |

**Validation rules**:
- Must own one bounded reason to change.
- Must not introduce direct reverse dependencies from render adapters back into runtime orchestration.
- Must preserve existing tick order and observable behavior.

**State transitions**:
- `planned` -> `implemented` -> `validated`

## Entity: PolicyService

| Field | Type | Description |
|-------|------|-------------|
| `name` | string | Focused runtime or render policy service name |
| `domain` | enum | `runtime`, `render`, or `poc` |
| `source_file` | path | Owning implementation file |
| `callers` | list | Allowed upstream callers |
| `forbidden_dependencies` | list | Dependency directions blocked by guard tests |

**Validation rules**:
- Must expose one focused policy concern.
- Must be callable from an explicit owner boundary.
- Must keep render transport and orchestration concerns separated.

## Entity: DiagnosticsBoundary

| Field | Type | Description |
|-------|------|-------------|
| `name` | string | Backend diagnostic ownership surface |
| `emitter` | path | Module that emits the status/telemetry |
| `status_contract` | string | Human-readable diagnostic string or status mapping |
| `failure_modes` | list | Expected error categories surfaced through the boundary |
| `tests` | list | Smoke/projection/flow tests that validate the boundary |

**Validation rules**:
- Diagnostic ownership must map to one runtime or render service boundary.
- Failure output must remain attributable after file splits.
- Diagnostics hardening must not change gameplay or rendering features.

## Entity: ArchitectureGuardRule

| Field | Type | Description |
|-------|------|-------------|
| `target_path` | path | Source file checked by the guard |
| `rule_description` | string | Intent expressed by the guard |
| `forbidden_patterns` | list | Includes or string patterns that fail the test |
| `gate_test` | string | Native CTest target enforcing the rule |
| `exception_ledger_ref` | string | Optional link to a temporary documented exception |

**Validation rules**:
- Every new runtime/render seam introduced by the feature should either have a guard rule or be covered by an existing rule.
- Exceptions must be explicit, time-bound, and tied to a follow-up slice.

## Entity: ConcernSplitChecklistItem

| Field | Type | Description |
|-------|------|-------------|
| `file_path` | path | Oversized or multi-concern file under review |
| `current_role` | string | Current combined responsibilities |
| `target_outcome` | enum | `split`, `thin-coordinator`, or `temporary-exception` |
| `evidence` | list | Tests, build logs, or notes proving the outcome |
| `owner_slice` | string | Mergeable slice responsible for the item |

**Validation rules**:
- Every in-scope oversized runtime/render implementation file must have one checklist item.
- Evidence must include at least one focused validation command.
- `temporary-exception` requires a specific follow-up slice.

## Relationships

- `TickPhase` uses one or more `PolicyService` instances.
- `DiagnosticsBoundary` is owned by a `PolicyService` or backend transport helper.
- `ArchitectureGuardRule` constrains dependencies between `TickPhase` and `PolicyService` implementations.
- `ConcernSplitChecklistItem` references the slice and evidence that validate a `TickPhase`, `PolicyService`, or `DiagnosticsBoundary` change.