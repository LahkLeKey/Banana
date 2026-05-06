# Cross-Domain Sequencing

Date: 2026-04-26

## Recommended Execution Sequence

1. CAND-003 (DAL Runtime Contract Hardening)
2. CAND-001 (Unified Operational JSON Contract Builder)
3. CAND-005 (ABI/Interop Drift Guard Automation)

## Sequencing Rationale

- CAND-003 first:
  - Establishes explicit runtime failure semantics and removes ambiguous DAL outcomes.
  - Reduces downstream uncertainty for API and automation behavior.

- CAND-001 second:
  - Refactors payload construction once status semantics are clearer.
  - Keeps operational contract work focused and bounded before adding guard automation complexity.

- CAND-005 third (or partially in parallel):
  - Best applied after near-term contract-changing work lands so the guard captures settled signatures/symbols.
  - Can start with non-blocking mode, then flip to blocking gate after initial tuning.

## Potential Parallelization

- CAND-005 parity checker prototype can begin during CAND-001 implementation in advisory mode.
- Final blocking enforcement for CAND-005 should wait until CAND-003 and CAND-001 contract updates stabilize.

## Dependencies

- CAND-003 depends on agreement for DAL status taxonomy.
- CAND-001 depends on payload compatibility policy and helper-module placement decision.
- CAND-005 depends on a canonical normalized signature model for symbols/methods.
