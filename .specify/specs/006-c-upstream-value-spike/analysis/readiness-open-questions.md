# Readiness Open Questions and Mitigations

Date: 2026-04-26

## Cross-Packet Open Questions

1. DAL status taxonomy: reuse existing status codes or introduce a distinct linkage-failure code?
2. JSON contract strictness: should property ordering be standardized where tests compare serialized strings?
3. Interop parity policy: should enforcement compare signature sets only, or include deterministic ordering?
4. Validation ownership: which lane owner signs off final parity checks when native and ASP.NET both change?

## Candidate-Specific Open Questions

- CAND-003:
  - How should "libpq not linked" be represented to avoid ambiguous `Ok` outcomes?
  - Is additional remediation messaging required in ASP.NET responses?

- CAND-001:
  - Where should shared JSON helper primitives live for minimal module churn?
  - Do we normalize numeric formatting precision across operational payloads?

- CAND-005:
  - Which existing workflow/script path is preferred for parity checks to avoid CI duplication?
  - What is the minimal deterministic representation for method/symbol comparison?

## Mitigations

- Resolve status taxonomy in a short design note before coding CAND-003.
- Add payload-shape snapshots early in CAND-001 implementation to catch drift during refactor.
- Prototype parity checker against current surfaces and tune normalization rules before hard-fail rollout.
- Assign explicit validation owner per follow-up story in planning kickoff.
