# Contract: Backlog Risk-Value Scorecard

## Purpose

Define deterministic inputs, scoring fields, and outputs for Banana backlog prioritization.

## Input Schema

Each issue candidate must provide:

- `issue_number` (integer, required)
- `title` (string, required)
- `labels` (array[string], required)
- `updated_at_utc` (string, required)
- `dependency_refs` (array[integer], optional)
- `blocker_refs` (array[integer], optional)

## Policy Schema

- `policy_version` (string, required)
- `weights.value_gain` (number, required)
- `weights.risk_reduction` (number, required)
- `weights.dependency_unlock` (number, required)
- `weights.urgency` (number, required)
- `weights.effort_penalty` (number, required)
- `weights.delivery_risk_penalty` (number, required)
- `tie_break_order` (array[string], required)

## Scorecard Output Schema

- `issue_number` (integer, required)
- `value_gain_score` (number, required)
- `risk_reduction_score` (number, required)
- `dependency_unlock_score` (number, required)
- `urgency_score` (number, required)
- `effort_penalty_score` (number, required)
- `delivery_risk_penalty_score` (number, required)
- `confidence_score` (number, required)
- `composite_priority_score` (number, required)
- `blocked` (boolean, required)
- `blocker_issue_numbers` (array[integer], required)
- `owner_agent` (string, required)
- `score_rationale` (string, required)

## Prioritized Snapshot Schema

- `snapshot_id` (string, required)
- `generated_at_utc` (string, required)
- `policy_version` (string, required)
- `immediate_queue` (array[Scorecard], required)
- `deferred_queue` (array[Scorecard], required)
- `metadata_gaps` (array[string], required)
- `top_recommendations` (array[integer], required)
- `human_summary_ref` (string, required)
- `ai_audit_ref` (string, required)

## Behavioral Guarantees

- Ranking is deterministic for identical input and policy.
- Dependency-blocked items cannot appear in immediate queue unless blocker-first ordering is satisfied.
- Every recommendation includes owner agent and rationale.
- Output is published in both human-readable and AI-audit lanes.
