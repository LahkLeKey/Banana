# Residual Risk Summary

Date: 2026-04-26

## Candidate Residual Risks

### CAND-003 DAL Runtime Contract Hardening

- Risk: Existing automation or callers may assume synthetic DAL success in unlinked scenarios.
- Risk level: High.
- Mitigation: Pair status-contract change with explicit unit/e2e updates and migration notes.

### CAND-001 Unified Operational JSON Contract Builder

- Risk: Refactor can accidentally alter payload fields/precision and break deserialization assumptions.
- Risk level: Medium.
- Mitigation: Add deterministic payload-shape assertions in native and controller tests before broad refactor.

### CAND-005 ABI/Interop Surface Drift Guard Automation

- Risk: Overly strict parity checks can introduce noisy CI failures.
- Risk level: Medium.
- Mitigation: Normalize signatures and run advisory mode before hard enforcement.

## Program-Level Risks

- Multi-layer contract changes can drift if updates are not landed atomically.
- Validation ownership gaps can delay sign-off when native and ASP.NET both change.

## Program-Level Mitigations

- Require same-PR updates for native status contract + ASP.NET mapping when statuses change.
- Assign explicit lane owner (`native`, `unit`, `e2e`) at follow-up story kickoff.
- Keep top-candidate stories bounded to one primary contract surface each.
