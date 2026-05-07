# Contract: Ensemble Predict Non-Reload UX Guardrail

## Purpose

Define the frontend contract that prevents "Predict ensemble" from causing blank-page outcomes while preserving current baseline verdict UX behavior.

## Scope

- Applies to ensemble submit interactions in the React surface.
- Covers click and keyboard-triggered submits.
- Covers success and recoverable error outcomes.

## Interaction Contract

1. Submitting ensemble prediction MUST stay in the current app view.
2. Submit MUST NOT cause full-page navigation or reload.
3. Submit MUST preserve in-page state transitions (`idle -> pending -> success|error`).
4. Duplicate submit behavior during pending MUST be deterministic and non-destructive.

## Baseline Preservation Contract

1. Existing verdict copy rendering remains available on success.
2. Escalation cue/panel behavior remains available when applicable.
3. Recoverable error path preserves retry affordance.
4. Retry continues to use last submitted sample context.

## Failure Classification Contract

All observed blank outcomes MUST be classified as one of:
- `navigation_reload`
- `render_blank`

Each classification must include:
- trigger type
- observed signals/evidence
- impacted baseline behavior

## Regression Validation Contract

Validation must include explicit checks that:
- click submit path does not reload,
- Enter-key submit path does not reload,
- success path still renders verdict surface,
- recoverable error path still renders retry surface.

## Evidence Expectations

- Repro steps for current bug.
- Root-cause hypothesis with confidence level.
- Guardrail checklist mapped to submit flow.
- Regression tests identified/updated for ongoing enforcement.

## Observed Findings Alignment (2026-04-26)

- Primary observed regression class: `navigation_reload` risk in submit semantics.
- Current implemented guardrails:
	- explicit submit navigation suppression,
	- unified guarded submit entry for click/keyboard/retry,
	- location stability classification around submit lifecycle.
- Current validation evidence:
	- focused React regression tests pass for click and keyboard no-reload paths,
	- baseline verdict/escalation/retry behavior remains intact under guarded flow.
- Remaining residual risk:
	- `render_blank` remains a tracked class for follow-on diagnostics if runtime-crash signals appear.
