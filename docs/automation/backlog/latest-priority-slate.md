# Backlog Priority Slate (2026-04-25)

## Immediate Execution (High Value, Low Blocker Risk)

1. Issue #306 - Coverage denominator and scope contract
   - Value: Unlocks stable denominator for all downstream coverage gates.
   - Risk reduction: Prevents false coverage regressions from scope drift.
   - Dependency unlock: Directly unblocks reliable enforcement in #307 and later coverage stories.

2. Issue #307 - Enforce 100% unit fail-under gate
   - Value: Converts reporting into a hard quality gate at the earliest stage.
   - Risk reduction: Stops merges with hidden unit regressions.
   - Dependency unlock: Establishes policy pattern reused by integration and E2E gates.

3. Issue #308 - Integration coverage gap map
   - Value: Produces objective backlog for integration closure work.
   - Risk reduction: Prevents blind spending across broad integration surface.
   - Dependency unlock: Guides ordered execution for #309, #310, and #311.

## Near-Term Follow-On (After Immediate Queue)

4. Issue #309 - Integration tranche 1 (happy-path + validation)
5. Issue #310 - Integration tranche 2 (error paths and interop boundaries)
6. Issue #311 - Integration tranche 3 + integration gate at 100%

## Deferred Until Foundations Are Stable

7. Issue #312 and #313 - E2E harness and critical journey matrix
8. Issue #314 - Merge-blocking E2E required check
9. Issue #315 - Flake budget, quarantine, and retry stabilization policy

## Rationale Summary

- The current epic chain (#305) is value-dense but strongly dependency-bound.
- Foundation contracts and strict unit gates provide maximum near-term unlock and reduce wasted effort in downstream integration and E2E slices.
- E2E enforcement should not become merge-blocking before integration gate behavior and denominator contracts are stable.
