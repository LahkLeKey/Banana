# Data Model: Agent Pulse Orchestration

## Entity: AgentIncrementDefinition

- Purpose: Defines one autonomous SDLC increment that can be executed independently.
- Fields:
  - `id`: Unique increment identifier.
  - `change_command`: Command executed to produce repository change.
  - `commit_message`: Commit title for the increment output.
  - `pr_title`: Pull request title for increment output.
  - `pr_body`: Pull request body summary.
  - `labels`: Comma-delimited labels, including exactly one `agent:*` marker.
  - `reviewers` (optional): Reviewer overrides.
  - `draft_pr` (optional): Draft PR override.
  - `base_branch` (optional): Base branch override.
  - `branch_prefix` (optional): Branch prefix override.
- Validation Rules:
  - `id` must be unique in a plan.
  - `change_command` must be non-empty.
  - `labels` must include one ownership marker for default catalog entries.

## Entity: AgentDeterministicLane

- Purpose: Native C model lane describing deterministic execution priority and pulse density per agent.
- Fields:
  - `agent`: Agent slug referenced by `agent:*` ownership labels.
  - `priority`: Strictly descending integer that controls rendered execution order.
  - `deterministic_snapshots`: Positive integer controlling how many increment slots the renderer emits for this lane.
  - `focus`: Human-readable lane descriptor used in generated intent text.
- Relationships:
  - One lane can map to one or more rendered increment definitions.

## Entity: AgentActivityRecord

- Purpose: Represents management-readable trace output for one agent increment execution.
- Fields:
  - `agent`: Agent slug derived from `agent:*` label.
  - `increment_id`: Increment that produced the record.
  - `run_id`: Workflow run identifier.
  - `run_attempt`: Workflow run attempt number.
  - `recorded_at_utc`: UTC timestamp.
  - `intent_summary`: Human-readable summary of work intent.
  - `audience_lane`: Output audience lane (`human-readable` or `ai-audit`).
  - `detail_level`: Expected verbosity (`concise-empathic` or `technical-verbose`).
  - `source_workflow`: Workflow name or file.
- Relationships:
  - Many activity records can belong to one agent.
  - One activity record belongs to one increment execution.

## Entity: AgentDeepDiveMap

- Purpose: Management entry point that maps each agent lane to documentation roots where snapshots are stored.
- Fields:
  - `agent`
  - `primary_focus`
  - `snapshot_root_path`
  - `snapshot_filename_pattern`
  - `audience_lane`
- Relationships:
  - One map references many per-agent snapshot folders.

## State Transitions

1. Base catalog loaded -> `AgentIncrementDefinition` templates parsed.
2. Native deterministic model rendered -> `AgentDeterministicLane` rows parsed from C CLI JSON.
3. Deterministic renderer emits final `AgentIncrementDefinition` plan in lane order.
4. Increment executed -> PR output status determined (`created`, `skipped`, `failed`, or `dry-run`).
5. Activity recorder runs -> immutable `AgentActivityRecord` snapshot files written under audience-specific roots.
6. Management deep-dive map in docs links managers to human-readable summaries and AI-audit roots for each agent.
