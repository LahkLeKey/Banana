# Upstream Impact Matrix

Date: 2026-04-26
Top candidates: CAND-003, CAND-001, CAND-005

| Candidate | Impacted Domains | Contract/Env Surfaces | Required Validation Lanes | Residual Risks |
|---|---|---|---|---|
| CAND-003 DAL Runtime Contract Hardening | native, aspnet, tests/automation, typescript-api (error propagation only) | Native status semantics; ASP.NET status mapping; `BANANA_PG_CONNECTION` remediation contract | `tests/native`, `tests/unit`, `tests/e2e` contract lane, targeted `src/typescript/api` route tests if status/body changes surface upstream | Migration risk for consumers expecting synthetic success payload under missing libpq; potential status-code mapping churn |
| CAND-001 Unified Operational JSON Contract Builder | native, aspnet, tests/automation, typescript-api (payload passthrough expectations) | Operational JSON payload field contract for batch/harvest/truck/ripeness | `tests/native`, `tests/unit` controller tests, `tests/e2e` contract lane | Backward-compatibility drift during refactor; inadvertent field/precision changes |
| CAND-005 ABI/Interop Surface Drift Guard Automation | native, aspnet, tests/automation, delivery/runtime CI | Wrapper ABI symbol contract; interop seam parity contract (`banana_wrapper.h` vs `NativeMethods`/`INativeBananaClient`) | `tests/unit`, `tests/e2e` contract lane, optional targeted native symbol parity check | False positives if parity normalization is too strict; initial tooling adoption friction |

## Impact Summary

- Highest immediate operational risk reduction: CAND-003.
- Highest payload stability gain across operational endpoints: CAND-001.
- Highest future delivery unlock and regression prevention: CAND-005.
