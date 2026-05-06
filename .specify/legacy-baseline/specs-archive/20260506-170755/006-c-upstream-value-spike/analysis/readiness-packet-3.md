# Readiness Packet 3

Candidate: CAND-005 ABI/Interop Surface Drift Guard Automation
Priority: 3

## Proposed Story Title

Add automated parity checks for native wrapper exports and managed interop surface alignment.

## Objective

Prevent CI-time and runtime breakage when wrapper ABI symbols and managed interop contracts drift out of sync.

## In Scope

- Define parity assertions among `src/native/wrapper/banana_wrapper.h`, native export implementation, and `src/c-sharp/asp.net/NativeInterop` interfaces.
- Add CI-verifiable checks that fail with actionable diagnostics when contract drift occurs.
- Cover test doubles impacted by interop seam growth (`tests/unit/TestDoubles/FakeNativeBananaClient.cs`).

## Out of Scope

- Replacing current interop architecture.
- Adding unrelated API endpoint behavior.
- Broad rewrite of test harnesses.

## Acceptance Intent

- If a wrapper symbol is added/removed without matching interop updates, validation fails with explicit mismatch details.
- If `INativeBananaClient` changes without aligned test-double updates, CI reports deterministic failure before merge.
- Existing happy-path interop tests remain green when contracts are aligned.

## Validation Expectations

- Unit lane: `dotnet test tests/unit --filter "FullyQualifiedName~NativeBananaClient|FullyQualifiedName~StatusMapping"`.
- Contract lane: `dotnet test tests/e2e --filter "FullyQualifiedName~Contracts"`.
- Optional native parity lane: targeted symbol/export check in native validation scripts.

## Dependencies

- Decision on implementation mechanism (scripted static check vs compile-time generated contract test).
- Agreed source-of-truth ordering for symbols/method signatures.

## Risks and Mitigations

- Risk: False positives if check is too strict on formatting/non-semantic differences.
- Mitigation: Compare normalized signatures rather than raw text.

## Open Questions

- Should parity enforcement live in a standalone script or in existing workflow validation scripts?
- Do we enforce exact ordering or only set equality for exported operations?
