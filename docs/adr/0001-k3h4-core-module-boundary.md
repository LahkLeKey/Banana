# ADR-0001: Extract K3H4 Core Into modules/k3h4

- Status: accepted
- Date: 2026-06-25
- Context: global

## Problem

K3H4 core code is currently distributed across native runtime paths, making ownership unclear, increasing drill-down depth, and weakening architecture boundaries.

## Decision

Adopt a one-shot core-only extraction of K3H4 into modules/k3h4 with these rules:

- Scope includes K3H4 native core source, K3H4 runtime netcode source, and K3H4 native tests.
- Scope excludes API and React feature ownership, which remain in place and are rewired to consume the new module boundary.
- Module layout uses native-focused structure:
  - modules/k3h4/native/src
  - modules/k3h4/native/include
  - modules/k3h4/native/tests
- Public target name is banana_k3h4_core.
- Include style moves to module-prefixed public includes now.
- External consumers are restricted to public boundary consumption only (no direct internal includes).
- Migration gate excludes war suite checks and requires:
  - native build
  - K3H4 native tests
  - API smoke
  - UI smoke

## Consequences

- Positive:
  - Clear ownership and shallower navigation for K3H4 core work
  - Stronger module boundaries and reduced architecture drift
  - Easier future extraction into separate repository if desired
- Negative:
  - Higher one-time migration risk due to one-shot path and include rewrites
  - Broad build and test rewiring in one change
- Follow-up actions:
  - Add module-local ADR folder at modules/k3h4/docs/adr
  - Add module CMake target wiring for banana_k3h4_core
  - Update include and link consumers to public boundary only

## Alternatives considered

- Keep K3H4 under src/native with partial cleanup: rejected because it preserves mixed ownership and deep navigation.
- Staged migration with compatibility shims: rejected for this effort; team chose one-shot cleanup.
