# Readiness Packet 1

Candidate: CAND-003 DAL Runtime Contract Hardening
Priority: 1

## Proposed Story Title

Harden native DAL runtime contract outcomes across configured, linked, and query-failure states.

## Objective

Remove ambiguous DAL success behavior so API consumers receive explicit, typed outcomes for misconfiguration, missing linkage, and query failures.

## In Scope

- Tighten DAL return behavior in `src/native/core/dal/banana_dal.c`.
- Align status taxonomy in `src/native/core/banana_status.c` and `src/native/wrapper/banana_wrapper.h`.
- Preserve controller-pipeline mapping behavior via `src/c-sharp/asp.net/Pipeline/StatusMapping.cs` updates as needed.
- Add/adjust tests to validate explicit failure/success semantics.

## Out of Scope

- New database feature endpoints.
- Broad schema changes in Postgres.
- Frontend UX redesign.

## Acceptance Intent

- Given `BANANA_PG_CONNECTION` is unset, DAL calls return explicit non-success status and remediation.
- Given libpq is unavailable at runtime/build, DAL calls do not return synthetic success payloads.
- Given query execution fails, API mapping returns deterministic status/body contract.
- Existing successful DAL query path remains backward compatible in shape.

## Validation Expectations

- Native lane: `tests/native` (DAL behavior coverage).
- ASP.NET lane: `dotnet test tests/unit --filter "FullyQualifiedName~StatusMapping|FullyQualifiedName~NativeBananaClient"`.
- Contract lane: `dotnet test tests/e2e --filter "FullyQualifiedName~Contracts"`.

## Dependencies

- Agreement on final status-code mapping semantics (native to ASP.NET).
- Confirmation whether any new status code is required or existing codes can be reused.

## Risks and Mitigations

- Risk: Existing callers depend on synthetic success payload behavior.
- Mitigation: Explicit migration note + contract test updates before merge.

## Open Questions

- Should linkage failure map to existing `DbError` or a new typed status?
- Do we need a dedicated remediation hint for link-time vs runtime DAL failures?
