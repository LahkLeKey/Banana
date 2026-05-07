# Baseline Preservation Report (US2)

## Scope

- Feature: 042 frontend ensemble blank-page spike
- Story: US2 (preserve baseline verdict/escalation/retry UX)
- Date: 2026-04-26

## Baseline Contract Checks

1. Verdict copy continues to render on successful guarded submit.
2. Escalation cue/panel trigger remains visible for escalated verdicts.
3. Retry affordance appears on recoverable error.
4. Retry continues using last submitted sample context.
5. Guarded retry path maintains stable location (no navigation/reload).

## Test Evidence

- Command: bun test src/App.test.tsx
- Focused assertions added:
  - preserves baseline verdict and escalation cues under guarded submit
  - retry path remains guarded and keeps location stable

## Behavior Notes

- Retry now routes through the same guarded submit entry (`submitEnsemble`) used by click/keyboard paths.
- This keeps submit semantics consistent for baseline and regression safety.

## Result

- Baseline verdict/escalation/retry behaviors remain intact under guarded submit flow.
- No navigation/reload regression observed in guarded retry path validation.
