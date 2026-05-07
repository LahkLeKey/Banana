# Data Model: C Upstream Value Reassessment SPIKE

**Branch**: `feature/006-c-upstream-value-spike` | **Phase**: 1 | **Date**: 2026-04-26

## Entities

### `NativeCandidateOpportunity`

**Purpose**: Represents a potential native C feature investment candidate.

| Field | Type | Constraint |
|-------|------|------------|
| `candidateId` | `string` | Unique, non-empty |
| `title` | `string` | Non-empty |
| `problemStatement` | `string` | Non-empty |
| `nativeSurface` | `string[]` | At least one relevant native path/component |
| `expectedUpstreamValue` | `string` | Non-empty |
| `assumptions` | `string[]` | Optional but recommended |

### `CandidateScorecard`

**Purpose**: Captures normalized ranking dimensions for one candidate.

| Field | Type | Constraint |
|-------|------|------------|
| `candidateId` | `string` | Must reference an existing `NativeCandidateOpportunity` |
| `valueGain` | `number` | Integer 0..5 |
| `riskReduction` | `number` | Integer 0..5 |
| `dependencyUnlock` | `number` | Integer 0..5 |
| `deliveryEffort` | `number` | Integer 0..5 |
| `confidence` | `number` | Integer 0..5 |
| `compositeScore` | `number` | Computed using agreed weighting model |
| `rankingNote` | `string` | Non-empty rationale |

### `UpstreamImpactMap`

**Purpose**: Defines cross-domain and contract impact for top candidates.

| Field | Type | Constraint |
|-------|------|------------|
| `candidateId` | `string` | Must reference top-ranked candidate |
| `impactedDomains` | `string[]` | Subset of approved domain labels |
| `contractSurfaces` | `string[]` | Explicitly names affected contracts or runtime env assumptions |
| `validationLanes` | `string[]` | Lists required validation lanes for follow-up implementation |
| `riskFlags` | `string[]` | Optional known risks |

### `FollowUpReadinessPacket`

**Purpose**: Handoff artifact for immediate follow-up story drafting.

| Field | Type | Constraint |
|-------|------|------------|
| `candidateId` | `string` | Must reference top-ranked candidate |
| `proposedStoryTitle` | `string` | Non-empty |
| `scopeIn` | `string[]` | At least one in-scope item |
| `scopeOut` | `string[]` | Explicit exclusions |
| `acceptanceIntent` | `string[]` | At least one testable acceptance statement |
| `validationPlan` | `string[]` | At least one required validation command/lane |
| `openQuestions` | `string[]` | Optional unresolved items |

## Relationships

- One `NativeCandidateOpportunity` has one `CandidateScorecard`.
- Top-ranked `NativeCandidateOpportunity` entries must have one `UpstreamImpactMap`.
- Top 3 `NativeCandidateOpportunity` entries must each have one `FollowUpReadinessPacket`.

## State/Status Notes

- Candidate lifecycle: `identified -> scored -> ranked -> packetized`.
- Only `packetized` candidates are considered ready for follow-up story conversion.
