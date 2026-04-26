# Data Model: CMake Preset Activation Fix

**Branch**: `008-cmake-preset-activation-fix` | **Phase**: 1 | **Date**: 2026-04-26

## Entities

### `PresetCatalog`

**Purpose**: Represents expected preset definitions for configure/build/test flows.

| Field | Type | Constraint |
|-------|------|------------|
| `configurePresetName` | `string` | Non-empty and resolvable in preset file |
| `buildPresetName` | `string` | Must map to valid configure preset |
| `testPresetName` | `string` | Must map to valid configure preset |
| `sourcePath` | `string` | Must reference root preset file |

### `WorkspaceProjectBinding`

**Purpose**: Captures mapping from workspace context to configured project root.

| Field | Type | Constraint |
|-------|------|------------|
| `workspaceContext` | `string` | One of `monorepo-root`, `tests-folder` |
| `cmakeSourceDirectory` | `string` | Must resolve to root CMake project |
| `presetActivationMode` | `string` | Must indicate preset-based activation |
| `bindingStatus` | `string` | `valid` or `invalid` |

### `ConfigureAttemptEvidence`

**Purpose**: Records validation result for configuration/discovery execution.

| Field | Type | Constraint |
|-------|------|------------|
| `lane` | `string` | `cmake-tools` or `preset-cli` |
| `result` | `string` | `pass` or `fail` |
| `targetsDiscovered` | `boolean` | True when build targets visible |
| `testsDiscovered` | `boolean` | True when tests visible |
| `diagnosticSummary` | `string` | Required for fail cases |

## Relationships

- One `PresetCatalog` is referenced by many `WorkspaceProjectBinding` records.
- Each `WorkspaceProjectBinding` must produce at least one `ConfigureAttemptEvidence` record.
- A feature closure decision requires at least one pass evidence record for `cmake-tools` lane.

## State Notes

- Expected state flow: `catalog valid -> binding valid -> configure pass -> discovery pass -> native lane executable`.
- Failure state must include deterministic diagnostic classification.
