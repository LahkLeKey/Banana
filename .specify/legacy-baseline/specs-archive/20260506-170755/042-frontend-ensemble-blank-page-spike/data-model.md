# Data Model: Ensemble Predict Blank Page UX Spike

## Overview

This spike is interaction-centric and does not introduce a new persistent datastore. The model below describes behavioral entities used to reason about submit flow, failure classes, and regression checks.

## Entities

### EnsembleSubmitInteraction

- Description: A single user-initiated attempt to classify an ensemble sample.
- Fields:
  - trigger_type: `button_click | keyboard_submit`
  - input_text_present: boolean
  - submitted_at: timestamp
  - pending_before_submit: boolean
  - request_id: string (logical correlation only)
- Validation rules:
  - input_text_present must be true for valid submission path.
  - trigger_type must be one of the supported submit channels.

### BlankPageRegressionEvent

- Description: Observed outcome where user sees blank page after submit action.
- Fields:
  - failure_class: `navigation_reload | render_blank`
  - observed_at: timestamp
  - route_after_submit: string
  - had_runtime_error_signal: boolean
  - had_navigation_signal: boolean
- Validation rules:
  - failure_class must map to at least one supporting signal.
  - Exactly one primary class is recorded per observed event.

### BaselineUXContract

- Description: Existing expected UX outputs that must be preserved.
- Fields:
  - verdict_copy_present: boolean
  - escalation_cue_present_when_expected: boolean
  - retry_affordance_present_on_recoverable_error: boolean
  - retry_uses_last_submitted_sample: boolean
- Validation rules:
  - All fields must be true for baseline-preservation pass.

### FrontendSpikeFinding

- Description: Evidence-backed planning output used by implementation tasks.
- Fields:
  - finding_id: string
  - category: `root_cause_hypothesis | guardrail | regression_test_requirement`
  - evidence_source: string
  - confidence: `high | medium | low`
  - action_required: string
- Validation rules:
  - category must be one of the listed values.
  - action_required must be explicit and implementation-actionable.

## Relationships

- An `EnsembleSubmitInteraction` may produce zero or one `BlankPageRegressionEvent`.
- A set of `EnsembleSubmitInteraction` outcomes validates the `BaselineUXContract`.
- One or more `FrontendSpikeFinding` records must reference the observed interactions/events.

## State Transitions

### Ensemble Submit Lifecycle

1. idle
2. submit_requested
3. request_in_flight
4. completed_success | completed_recoverable_error | completed_regression_blank_page

### Regression Classification Lifecycle

1. event_observed
2. evidence_collected
3. class_assigned
4. guardrail_defined
5. regression_test_defined
