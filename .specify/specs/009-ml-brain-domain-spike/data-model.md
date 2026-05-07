# Data Model: ML Brain Domain SPIKE

**Branch**: `009-ml-brain-domain-spike` | **Phase**: 1 | **Date**: 2026-04-26

## Entities

### `BrainDomainDefinition`

**Purpose**: Defines the canonical domain role and boundaries.

| Field | Type | Constraint |
|-------|------|------------|
| `domainId` | `string` | One of `left`, `right`, `full` |
| `primaryReasoningStyle` | `string` | Non-empty |
| `primaryOutputType` | `string` | Non-empty |
| `boundaryNotes` | `string` | Must include out-of-domain conditions |
| `knownRisks` | `string[]` | At least one risk statement |

### `ModelFamilyMappingRecord`

**Purpose**: Maps each model family to primary domain with rationale.

| Field | Type | Constraint |
|-------|------|------------|
| `modelFamily` | `string` | One of `regression`, `binary`, `transformer` |
| `primaryDomainId` | `string` | Must reference valid `BrainDomainDefinition` |
| `secondaryInteractions` | `string[]` | Optional but bounded |
| `mappingRationale` | `string` | Non-empty, evidence-backed |

### `DomainComparisonMatrixRow`

**Purpose**: Stores normalized comparison scores by model family.

| Field | Type | Constraint |
|-------|------|------------|
| `modelFamily` | `string` | Unique per row |
| `outputDeterminism` | `number` | Normalized scale |
| `calibrationFidelity` | `number` | Normalized scale |
| `contextReasoningDepth` | `number` | Normalized scale |
| `validationComplexity` | `number` | Normalized scale |
| `notes` | `string` | Required |

### `DomainReadinessPacket`

**Purpose**: Converts SPIKE outputs into execution-ready follow-up slices.

| Field | Type | Constraint |
|-------|------|------------|
| `domainId` | `string` | Must reference valid domain |
| `objective` | `string` | Non-empty |
| `boundedScope` | `string` | Explicitly in/out listed |
| `requiredLanes` | `string[]` | Includes minimum validation surfaces |
| `contractRisks` | `string[]` | At least one risk entry |

## Relationships

- One `BrainDomainDefinition` may map to multiple `ModelFamilyMappingRecord` secondary interactions but only one primary mapping per family.
- Each `ModelFamilyMappingRecord` must have one corresponding `DomainComparisonMatrixRow`.
- Each `BrainDomainDefinition` must produce at least one `DomainReadinessPacket`.

## State Notes

- Expected flow: `define domains -> map families -> score matrix -> produce readiness packets -> prioritize follow-up slices`.
- Uncertain mapping confidence must be captured explicitly in packet risk notes.
