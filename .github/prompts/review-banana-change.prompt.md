---
name: review-banana-change
description: Review Banana changes for bugs, regressions, missing tests, and release blockers.
argument-hint: Describe the changes to review or ask for a review of the current worktree.
agent: banana-reviewer
---

Review this Banana change in code-review mode.

Requirements:

- Prioritize findings over summaries.
- Focus on correctness, runtime risk, contract breaks, missing tests, and delivery issues.
- Order findings by severity and include concrete file references.
- Note when the work should have been split into narrower helper-owned slices but was not.
- If no findings are present, say so and note any residual validation gaps.

Relevant assets:

- [banana-agent-decomposition](../skills/banana-agent-decomposition/SKILL.md)
- [banana-release-readiness](../skills/banana-release-readiness/SKILL.md)
- [banana-test-and-coverage](../skills/banana-test-and-coverage/SKILL.md)
