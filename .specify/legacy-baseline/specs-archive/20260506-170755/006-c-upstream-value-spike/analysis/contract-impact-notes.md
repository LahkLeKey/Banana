# Contract Impact Notes

Date: 2026-04-26

## CAND-003 DAL Runtime Contract Hardening

Impacted contracts:

- Native status contract in `src/native/wrapper/banana_wrapper.h` and remediation text in `src/native/core/banana_status.c`.
- ASP.NET status-to-HTTP contract in `src/c-sharp/asp.net/Pipeline/StatusMapping.cs`.
- Runtime environment assumption for `BANANA_PG_CONNECTION` and DAL linkage state handling.

Contract guidance:

- Preserve typed, deterministic status handling.
- Avoid returning `Ok` for non-viable DAL paths (misconfigured/unlinked/failure).

## CAND-001 Unified Operational JSON Contract Builder

Impacted contracts:

- Operational payload field shapes produced by `banana_batch.c`, `banana_truck.c`, and `banana_ripeness.c`.
- Controller deserialization expectations in batch/harvest/truck/ripeness API flows.

Contract guidance:

- Keep existing field names and essential semantics stable.
- If field changes become necessary, require contract-lane updates in the same PR.

## CAND-005 ABI/Interop Surface Drift Guard Automation

Impacted contracts:

- Exported function set in `src/native/wrapper/banana_wrapper.h` and implementation parity in `banana_wrapper.c`.
- Managed interop parity in `src/c-sharp/asp.net/NativeInterop/NativeMethods.cs` and `INativeBananaClient.cs`.
- Test-double parity in `tests/unit/TestDoubles/FakeNativeBananaClient.cs`.

Contract guidance:

- Treat symbol/method parity as a release gate for native-facing stories.
- Emit actionable diagnostics (missing symbol, stale method, signature mismatch) to keep remediation fast.
