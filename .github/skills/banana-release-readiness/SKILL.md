---
name: banana-release-readiness
description: Review Banana changes for merge and release readiness across code, tests, docs, runtime contracts, and automation.
argument-hint: Describe the work to assess, or ask for a release-readiness checklist for the current changes.
---

# Banana Release Readiness

Use this skill near the end of a task to make sure the delivered change is actually ready to land.

## Review Procedure

1. Confirm the change stayed within the intended domain boundaries.
2. Confirm the broad agent delegated to narrower helpers when the scope allowed it.
3. Verify that affected build, test, and runtime entry points were exercised.
4. Check whether environment contracts or public behavior changed.
5. Update the nearest docs, prompts, or skill references when workflows changed.
6. Call out unresolved risks, deferred tests, or rollout concerns explicitly.

## Resources

- Release checklist: [release-checklist.md](./release-checklist.md)
- Helper routing: [../banana-agent-decomposition/helper-matrix.md](../banana-agent-decomposition/helper-matrix.md)
