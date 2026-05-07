# Regression Coverage Requirements (US3)

## Required Assertions

1. Click submit path does not navigate/reload and keeps location stable.
2. Keyboard submit path does not navigate/reload and keeps location stable.
3. Successful guarded submit still renders baseline verdict copy.
4. Escalated verdict still renders escalation cue/panel trigger.
5. Recoverable error still renders retry affordance.
6. Retry still reuses last submitted sample and remains location-stable.

## Implemented Coverage

- Source: src/typescript/react/src/App.test.tsx
- Implemented tests:
  - click submit keeps location stable and renders verdict
  - keyboard submit keeps location stable and renders verdict
  - preserves baseline verdict and escalation cues under guarded submit
  - shows retry button after error and reuses last submitted draft
  - retry path remains guarded and keeps location stable

## Command Contract

- Focused regression run:
  - bun test src/App.test.tsx

## Pass Criteria

- All listed tests pass with zero failures.
- Any future URL/navigation drift in submit or retry paths fails tests.
- Baseline verdict/escalation/retry behavior remains represented in the same test surface.

## Residual Gap

- Render-blank classification is not yet asserted via dedicated runtime-crash simulation tests.
- If this signal appears, add a targeted render-failure regression lane in a follow-on slice.
