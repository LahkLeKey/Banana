# Feature Specification: Shared Resilience Primitive + React Adoption

**Feature Branch**: `[029-shared-resilience-primitive-react-adoption]`
**Created**: 2026-04-26
**Status**: GATED. Foundational for slices 030 + 031.
**Source**: [../022-frontend-resilient-offline-spike/analysis/followup-readiness-shared.md](../022-frontend-resilient-offline-spike/analysis/followup-readiness-shared.md)
## Problem Statement

## In Scope

- Feature implementation and integration
- Testing and validation of new behavior
- Documentation of feature usage and design decisions

Feature Specification: Shared Resilience Primitive + React Adoption aims to improve system capability and user experience by implementing the feature described in the specification.


## Goal

Land `src/typescript/shared/resilience/` per the storage contract
from SPIKE 022, ship the IndexedDB adapter, and adopt it in the
React channel (verdict + chat + last-N history surface).

## Functional requirements

- **FR-029-01**: `@banana/resilience` package exports `RequestQueue`,
  `VerdictHistory`, `RetryPolicy` per
  [storage-contract.md](../022-frontend-resilient-offline-spike/analysis/storage-contract.md).
- **FR-029-02**: `IndexedDbAdapter` implements `StorageAdapter`.
- **FR-029-03**: vitest covers happy path + retry exhaustion +
  dedupe-by-key.
- **FR-029-04**: Submitting React verdict while offline preserves
  the draft and queues the request; coming back online drains the
  queue and renders the verdict.
- **FR-029-05**: Closing and reopening React tab (same browser
  profile) shows last-N verdicts.
- **FR-029-06**: Online behavior unchanged (no perf regression).

## Out of scope

- Electron adapter (slice 030; reuses IndexedDB but adds desktop UX).
- React Native adapter (slice 031).
- Cross-device sync (D-022-01).
- Encryption at rest (D-022-03).

## Validation lane

```
bun run --cwd src/typescript/shared/resilience tsc --noEmit
bun run --cwd src/typescript/shared/resilience test
bun run --cwd src/typescript/react tsc --noEmit
bun run --cwd src/typescript/react test
```

## Success criteria

- All FR-029-* hold.
- vitest covers documented contract states.

## In-scope files

See [readiness packet](../022-frontend-resilient-offline-spike/analysis/followup-readiness-shared.md#in-scope-files).
