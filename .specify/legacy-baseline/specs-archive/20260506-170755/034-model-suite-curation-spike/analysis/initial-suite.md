# Initial Suite (G2)

The chatbot orchestrator's initial model suite.

| Model       | Status     | Purpose                                      | Trigger to add later                      |
| ----------- | ---------- | -------------------------------------------- | ----------------------------------------- |
| not-banana  | EXISTS     | Binary banana vs not-banana classifier       | -                                         |
| banana      | NEW (slice)| Positive-class corpus + trainer (mirrors not-banana shape) | Slice = SPIKE 034 follow-up               |
| ripeness    | NEW (slice)| Multi-class ripeness (unripe / ripe / overripe / spoiled) | Slice = SPIKE 034 follow-up               |
| escalation  | DEFERRED   | Free-text escalation explainer               | Trigger: chatbot scaffold needs explanations beyond `did_escalate` flag |
| intent      | DEFERRED   | Chatbot intent router (greeting / classify / history / help) | Trigger: chatbot scaffold spec exists     |

## Why exactly three

- Each new model is a slice <=20 tasks (the SPIKE 022 / 029 sizing
  rule) -- bigger initial suites blow that budget.
- Banana + ripeness exercise the corpus schema generalization
  (binary vs multi-class) without committing to free-text
  generation, which would bring tokenizer + decoder concerns we
  do not need yet.
- The chatbot orchestrator can land on top of these three with a
  rules-based intent router; the trained intent model is a
  later-stage optimization.
