# Data Model: V3 Runtime Baseline Reset

## Entities

### 1. Baseline Authority

| Field | Type | Description |
|---|---|---|
| `id` | string | Stable authority identifier (`spec-052-reset`) |
| `governing_spec_path` | path | Active spec path that defines scope |
| `active_source_paths` | path[] | Current repository paths allowed to define runtime truth |
| `lineage_archive_paths` | path[] | Archived reference-only paths retained for history |
| `scope_rule` | enum | `gameplay_customer_facing_only` |
| `status` | enum | `active`, `superseded` |

**Validation rules**

- Exactly one baseline authority may be `active`.
- Active authority must point to a live spec folder and live source paths.
- Archived lineage may inform rationale but cannot override active source evidence.

### 2. Retained Capability

| Field | Type | Description |
|---|---|---|
| `id` | string | Stable capability identifier |
| `name` | string | Human-readable retained behavior or surface |
| `lane_id` | string | Owning execution lane |
| `surface_type` | enum | `native`, `api`, `frontend`, `shared-contract` |
| `customer_facing` | boolean | Whether the capability affects observable runtime behavior |
| `gameplay_relevant` | boolean | Whether the capability is gameplay-related |
| `runtime_entry_paths` | path[] | Live source paths that currently expose the retained capability |
| `evidence_record_ids` | string[] | Evidence proving the capability is retained |
| `stale_dependency_ids` | string[] | Archived/stale pathways that still need migration notes |
| `status` | enum | `observed`, `retained`, `excluded`, `planned` |

**Validation rules**

- A retained capability must be both `customer_facing=true` and `gameplay_relevant=true`.
- A retained capability must map to exactly one `lane_id`.
- A retained capability must cite at least one live `runtime_entry_path`.
- A retained capability must cite at least one authoritative evidence record.

### 3. Evidence Record

| Field | Type | Description |
|---|---|---|
| `id` | string | Stable evidence identifier |
| `path` | path | Repository or archive path |
| `kind` | enum | `source`, `spec`, `heartbeat`, `archive`, `workflow-output` |
| `authoritative` | boolean | Whether the record can govern current V3 decisions |
| `summary` | string | Short explanation of what the path proves |
| `notes` | string | Migration or stale-path commentary |

**Validation rules**

- Source/spec records in the active tree may be authoritative.
- Archive records are always `authoritative=false`.
- Heartbeat/workflow records are authoritative only when they match live source reality.

### 4. Execution Lane

| Field | Type | Description |
|---|---|---|
| `id` | string | `lane-a`, `lane-b`, `lane-c`, `lane-d` |
| `name` | string | Native gameplay, API gameplay, frontend gameplay, shared gameplay contracts |
| `owned_paths` | path[] | Repository surfaces the lane can change without cross-lane approval |
| `entry_capabilities` | string[] | Retained capabilities starting in the lane |
| `stitch_dependencies` | string[] | Cross-lane joins required later |
| `acceptance_evidence` | string[] | Minimum evidence required before merge |

**Validation rules**

- Owned paths may not overlap except at explicit stitch points.
- Every retained capability must belong to one and only one lane.
- Stitch work may depend on multiple lanes, but the pre-stitch capability record still has one owner.

### 5. Archived Lineage Reference

| Field | Type | Description |
|---|---|---|
| `id` | string | Stable lineage identifier |
| `archive_path` | path | Archived location in `.specify/legacy-baseline/` or `.legacy/` |
| `source_spec_ids` | string[] | Superseded specs or archived sources it represents |
| `usable_for` | enum[] | `history`, `migration-notes`, `comparison` |
| `active_authority` | boolean | Always `false` |

**Validation rules**

- Archived lineage must remain accessible.
- Archived lineage cannot be cited as sole evidence for a retained capability.

### 6. Planning Surface

| Field | Type | Description |
|---|---|---|
| `id` | string | Stable planning artifact identifier |
| `path` | path | Feature artifact path |
| `purpose` | enum | `plan`, `research`, `data-model`, `contract`, `quickstart`, `tasks`, `heartbeat` |
| `depends_on` | string[] | Other planning surfaces or evidence records |
| `consumers` | string[] | Agents, planners, or future specs that read the artifact |

**Validation rules**

- Every planning surface must point back to the active baseline authority.
- Contracts and quickstart guidance must remain consistent with the plan and data model.

## Relationships

- `Baseline Authority` owns many `Evidence Record` entries.
- `Retained Capability` requires one or more `Evidence Record` entries.
- `Retained Capability` belongs to exactly one `Execution Lane`.
- `Retained Capability` may reference zero or more `Archived Lineage Reference` entries for migration notes.
- `Planning Surface` depends on `Baseline Authority` and selected `Evidence Record` entries.

## State Transitions

### Retained Capability

`observed` -> `retained` -> `planned`

`observed` -> `excluded`

**Transition rules**

- `observed` -> `retained`: capability passes the gameplay/customer-facing scope gate and has live evidence.
- `observed` -> `excluded`: capability fails the scope gate or only exists in lineage.
- `retained` -> `planned`: capability is assigned to one lane and referenced by downstream plan/tasks artifacts.

### Baseline Authority

`active` -> `superseded`

**Transition rule**

- A baseline authority may become `superseded` only when a newer active spec explicitly archives it and updates active pointers.
