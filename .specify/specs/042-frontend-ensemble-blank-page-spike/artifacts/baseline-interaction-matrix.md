# Baseline Interaction Matrix

| Interaction | Trigger | Expected Result | Validation Surface |
|---|---|---|---|
| Predict submit (primary) | Button click | No navigation/reload, in-page verdict or error | App.test.tsx + integrated browser |
| Predict submit (alternate) | Ctrl+Enter in textarea | No navigation/reload, in-page verdict or error | App.test.tsx + integrated browser |
| Escalated verdict | Successful predict with did_escalate=true | Escalation cue and panel trigger visible | App.test.tsx |
| Recoverable error | Predict returns 5xx/network failure | Inline error + retry affordance shown | App.test.tsx |
| Retry action | Retry button click | Reuses last submitted sample and remains in-page | App.test.tsx |
| Edit after error | Textarea change with retry state present | Retry affordance clears for new draft | App.test.tsx |
