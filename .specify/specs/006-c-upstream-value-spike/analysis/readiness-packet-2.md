# Readiness Packet 2

Candidate: CAND-001 Unified Operational JSON Contract Builder
Priority: 2

## Proposed Story Title

Introduce shared native JSON contract builder for operational batch, harvest, truck, and ripeness responses.

## Objective

Reduce payload-construction drift and malformed JSON risk by centralizing response assembly for operational native domains.

## In Scope

- Add shared helper(s) for bounded JSON payload writing in `src/native/core`.
- Refactor repeated manual payload assembly in `banana_batch.c`, `banana_truck.c`, and relevant ripeness output paths.
- Preserve existing response field names and route-level contracts.
- Add deterministic regression checks for payload shape consistency.

## Out of Scope

- Endpoint additions or removals.
- Model algorithm changes.
- ASP.NET routing/pipeline redesign.

## Acceptance Intent

- Operational responses are produced through shared builder paths, reducing duplicate formatter logic.
- Existing API payload shape remains contract-compatible for current unit/e2e assertions.
- Overflow and invalid-argument handling remain explicit and deterministic.

## Validation Expectations

- Native lane: `tests/native` with focus on `test_operational_domains.c`.
- ASP.NET lane: `dotnet test tests/unit --filter "FullyQualifiedName~HarvestController|FullyQualifiedName~TruckController|FullyQualifiedName~RipenessController"`.
- Contract lane: `dotnet test tests/e2e --filter "FullyQualifiedName~Contracts"`.

## Dependencies

- Agreement on strict backward-compatible field naming policy.
- Shared helper API shape review before broad refactor.

## Risks and Mitigations

- Risk: Silent field-name drift during helper adoption.
- Mitigation: Snapshot-style payload assertions in native/unit tests for key fields.

## Open Questions

- Should shared helper live in a new `src/native/core/domain` utility module or alongside operational files initially?
- Do we codify property ordering for deterministic string comparisons in current tests?
