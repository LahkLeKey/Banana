# Data Model: Backlog Risk-Value Prioritization

## Entity: BacklogIssueSnapshot

- Purpose: Normalized input record for one candidate backlog issue.
- Fields:
  - `issue_number`
  - `title`
  - `state`
  - `labels`
  - `priority_label`
  - `phase_label`
  - `status_label`
  - `owner_agent_label`
  - `created_at_utc`
  - `updated_at_utc`
  - `dependency_refs`
  - `blocker_refs`

## Entity: PrioritizationPolicy

- Purpose: Versioned scoring rule set for ranking logic.
- Fields:
  - `policy_version`
  - `weights.value_gain`
  - `weights.risk_reduction`
  - `weights.dependency_unlock`
  - `weights.urgency`
  - `weights.effort_penalty`
  - `weights.delivery_risk_penalty`
  - `tie_break_order`
  - `minimum_ready_labels`

## Entity: RiskValueScorecard

- Purpose: Per-issue computed score breakdown.
- Fields:
  - `issue_number`
  - `value_gain_score`
  - `risk_reduction_score`
  - `dependency_unlock_score`
  - `urgency_score`
  - `effort_penalty_score`
  - `delivery_risk_penalty_score`
  - `confidence_score`
  - `composite_priority_score`
  - `blocked`
  - `blocker_issue_numbers`
  - `score_rationale`

## Entity: PrioritizedBacklogSnapshot

- Purpose: Run-scoped output containing execution recommendations.
- Fields:
  - `snapshot_id`
  - `generated_at_utc`
  - `policy_version`
  - `input_issue_count`
  - `immediate_queue`
  - `deferred_queue`
  - `metadata_gaps`
  - `top_recommendations`
  - `human_summary_ref`
  - `ai_audit_ref`

## State Transitions

1. Load issue metadata -> create `BacklogIssueSnapshot` entries.
2. Load scoring policy -> validate `PrioritizationPolicy`.
3. Compute per-issue metrics -> emit `RiskValueScorecard` rows.
4. Evaluate dependency blockers -> split immediate and deferred queues.
5. Publish ranked outputs -> persist `PrioritizedBacklogSnapshot` in human and AI lanes.
