# Validation Lane Plan

Date: 2026-04-26

## Per-Candidate Lane Expectations

### CAND-003 DAL Runtime Contract Hardening

- `tests/native`
  - Purpose: Validate explicit DAL outcome behavior under configured, unconfigured, and failure states.
- `tests/unit`
  - Command: `dotnet test tests/unit --filter "FullyQualifiedName~StatusMapping|FullyQualifiedName~NativeBananaClient"`
  - Purpose: Verify status translation and interop behavior remain deterministic.
- `tests/e2e` contracts
  - Command: `dotnet test tests/e2e --filter "FullyQualifiedName~Contracts"`
  - Purpose: Ensure contract-lane behavior remains stable for API consumers.

### CAND-001 Unified Operational JSON Contract Builder

- `tests/native`
  - Purpose: Assert deterministic payload field presence and status transitions in operational domains.
- `tests/unit`
  - Command: `dotnet test tests/unit --filter "FullyQualifiedName~HarvestController|FullyQualifiedName~TruckController|FullyQualifiedName~RipenessController"`
  - Purpose: Verify controller deserialization/mapping remains valid.
- `tests/e2e` contracts
  - Command: `dotnet test tests/e2e --filter "FullyQualifiedName~Contracts"`
  - Purpose: Catch route-level contract regressions.

### CAND-005 ABI/Interop Surface Drift Guard Automation

- `tests/unit`
  - Command: `dotnet test tests/unit --filter "FullyQualifiedName~NativeBananaClient|FullyQualifiedName~StatusMapping"`
  - Purpose: Verify interop and status behaviors under aligned contracts.
- Contract parity check
  - Purpose: Fail fast when wrapper exports and managed interop surface diverge.
- `tests/e2e` contracts
  - Command: `dotnet test tests/e2e --filter "FullyQualifiedName~Contracts"`
  - Purpose: Ensure consumer-visible API stability after contract alignment updates.

## Residual Gaps to Monitor

- Targeted TypeScript API tests may be needed if upstream response shapes/status payloads change.
- Native symbol parity checks should be tuned to avoid formatting-only false positives.
