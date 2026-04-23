---
name: implement-banana-classifier-slice
description: Implement a focused banana-vs-not-banana classifier slice across training, inference contract, or simple frontend flow.
argument-hint: Describe the classifier improvement you want, such as corpus updates, inference behavior, API output, or frontend result UX.
agent: banana-classifier-agent
---

Implement this banana-vs-not-banana classifier slice with focused ownership.

Requirements:

- Keep corpus, training script, native inference logic, and API route behavior aligned.
- Prefer narrow helper handoffs for implementation details outside this agent's ownership.
- Keep frontend behavior simple and typed: input -> classify -> decision + confidence + error state.
- Summarize behavior changes, validation run, and helper handoffs used.

Relevant assets:

- [banana-agent-decomposition](../skills/banana-agent-decomposition/SKILL.md)
- [banana-build-and-run](../skills/banana-build-and-run/SKILL.md)
- [banana-test-and-coverage](../skills/banana-test-and-coverage/SKILL.md)
- [banana-ci-debugging](../skills/banana-ci-debugging/SKILL.md)
