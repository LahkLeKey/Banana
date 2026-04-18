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
- If no findings are present, say so and note any residual validation gaps.

Relevant assets:

- [banana-release-readiness](../skills/banana-release-readiness/SKILL.md)
- [banana-test-and-coverage](../skills/banana-test-and-coverage/SKILL.md)
