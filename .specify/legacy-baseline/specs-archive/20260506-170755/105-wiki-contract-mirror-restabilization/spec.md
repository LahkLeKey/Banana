# Feature Specification: Wiki Contract and Mirror Restabilization

**Feature Branch**: `105-wiki-contract-mirror-restabilization`
**Created**: 2026-05-01
**Status**: Planned for bulk stabilization execution
**Wave**: stabilization
**Domain**: docs / workflow
**Depends on**: #102

## Problem Statement

The AI contract validator currently reports wiki allowlist and mirror drift. The section-based wiki structure and `.specify/wiki/human-reference` mirror are out of sync with the allowlist contract, causing deterministic validation failures.

## In Scope *(mandatory)*

- Reconcile `.wiki` markdown set with `.specify/wiki/human-reference-allowlist.txt`.
- Ensure mirror parity by re-running and hardening consume/scaffold scripts.
- Add explicit CI guardrails for section-based wiki path contracts.
- Document recovery workflow for wiki drift remediation.
- Add a rate-limit aware workflow path so wiki contract checks classify transient GitHub API exhaustion separately from real wiki drift.

## Out of Scope *(mandatory)*

- New wiki information architecture changes beyond contract parity.
- Rewriting content unrelated to validator compliance.

## Success Criteria

- `python scripts/validate-ai-contracts.py` returns zero issues.
- Allowlist, source wiki set, and human-reference mirror remain parity aligned in CI.
- Drift can be repaired by a single documented command path.
- Wiki-related gates publish deterministic rerun guidance when failures are due to installation/API rate limits.

## Notes for the planner

- Treat section paths as canonical and avoid flattening mismatches.
- Keep `_templates` and AI-only surfaces excluded per policy.
- Preserve actionable operator diagnostics for drift vs platform rate-limit failures.
