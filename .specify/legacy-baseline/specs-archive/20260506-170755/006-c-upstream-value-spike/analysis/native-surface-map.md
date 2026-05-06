# Native Surface Map

Date: 2026-04-26
Spec: 006-c-upstream-value-spike

## Candidate-to-Surface Mapping

| Candidate | Core Native Surfaces | Wrapper/Interop Surfaces | Upstream Touchpoints |
|---|---|---|---|
| CAND-001 | `src/native/core/banana_batch.c`, `src/native/core/banana_truck.c`, `src/native/core/banana_ripeness.c` | `src/native/wrapper/banana_wrapper.c` (pass-through stability) | `src/c-sharp/asp.net/Controllers/BatchController.cs`, `src/c-sharp/asp.net/Controllers/HarvestController.cs`, `src/c-sharp/asp.net/Controllers/TruckController.cs` |
| CAND-002 | `src/native/core/banana_batch.c`, `src/native/core/banana_truck.c` | `src/native/wrapper/banana_wrapper.h`, `src/native/wrapper/banana_wrapper.c` (if reset/status export needed) | `src/c-sharp/asp.net/NativeInterop/INativeBananaClient.cs`, `tests/native/test_operational_domains.c` |
| CAND-003 | `src/native/core/dal/banana_dal.c`, `src/native/core/banana_status.c` | `src/native/wrapper/banana_wrapper.h` (status/code contract) | `src/c-sharp/asp.net/Pipeline/StatusMapping.cs`, DAL-related controller flows |
| CAND-004 | `src/native/core/domain/banana_ml_models.c`, `src/native/core/domain/ml/*`, `src/native/core/banana_ripeness.c` | `src/native/wrapper/domain/ml/*`, `src/native/wrapper/banana_wrapper.h` | `src/c-sharp/asp.net/Controllers/BananaMlController.cs`, `src/c-sharp/asp.net/Controllers/RipenessController.cs`, `src/typescript/api/src/routes/ripeness.ts` |
| CAND-005 | `src/native/wrapper/banana_wrapper.h`, `src/native/wrapper/banana_wrapper.c` | `src/c-sharp/asp.net/NativeInterop/NativeMethods.cs`, `src/c-sharp/asp.net/NativeInterop/NativeBananaClient.cs`, `src/c-sharp/asp.net/NativeInterop/INativeBananaClient.cs` | `tests/unit/TestDoubles/FakeNativeBananaClient.cs`, `tests/unit/NativeBananaClientTests.cs` |
| CAND-006 | `src/native/core/banana_batch.c`, `src/native/core/banana_truck.c` | `src/native/wrapper/banana_wrapper.c` | `src/c-sharp/asp.net/Controllers/BatchController.cs`, `src/c-sharp/asp.net/Controllers/TruckController.cs`, `tests/e2e/Contracts/ApiContractLaneTests.cs` |

## Validation Lanes (minimum by candidate)

- CAND-001: `tests/native`, `tests/unit` (batch/harvest/truck controllers), `tests/e2e` contract lane.
- CAND-002: `tests/native`, `tests/unit` interop/controller tests, `tests/e2e` contract lane if response semantics shift.
- CAND-003: `tests/native` DAL behavior, `tests/unit` status mapping/controller behavior, targeted integration-equivalent DAL contract checks.
- CAND-004: `tests/native` ML tests, `tests/unit` ML/ripeness controllers, `src/typescript/api` route tests when payload contracts change.
- CAND-005: `tests/unit` interop + fake contract checks, `tests/e2e` contract lane for route stability, optionally `tests/native` symbol parity check.
- CAND-006: `tests/native` operational deterministic tests, `tests/unit` controller payload tests, `tests/e2e` API contract lane.
