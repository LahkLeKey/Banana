# Data Model: Specify-Driven Drift Realignment

## Entity: FeatureIntakeRecord

- Purpose: Canonical input record for a new planning slice.
- Fields:
  - issue_number
  - issue_title
  - issue_labels
  - issue_url
  - intake_summary
  - intake_date_utc
  - requested_outcomes

## Entity: ScopeBoundary

- Purpose: Defines what the current slice includes and excludes.
- Fields:
  - feature_id
  - in_scope_outcomes
  - out_of_scope_outcomes
  - boundary_rationale
  - boundary_owner
  - boundary_approved_at_utc

## Entity: DriftFinding

- Purpose: Captures a mismatch between proposed work and approved specification scope.
- Fields:
  - finding_id
  - feature_id
  - source_artifact
  - proposed_item
  - matched_requirement_ids
  - drift_status
  - drift_reason
  - detected_at_utc

## Entity: RealignmentDecision

- Purpose: Stores decision outcomes for each drift finding.
- Fields:
  - decision_id
  - finding_id
  - decision_type
  - rationale
  - follow_up_destination
  - decided_by
  - decided_at_utc

## Entity: TraceabilityMap

- Purpose: Links user stories and requirements to planning and delivery slices.
- Fields:
  - feature_id
  - user_story_id
  - requirement_id
  - planned_task_id
  - owner_lane
  - validation_owner
  - status

## State Transitions

1. Intake captured -> create FeatureIntakeRecord.
2. Scope agreed -> create ScopeBoundary.
3. Planning proposals compared to requirements -> create DriftFinding entries.
4. Drift reviewed -> create RealignmentDecision entries.
5. Approved work packaged for execution -> update TraceabilityMap and mark deferred/split follow-ups.
