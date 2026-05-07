# Feature Specification: React Native Persistence + Retry

**Feature Branch**: `[031-react-native-persistence-retry]`
**Created**: 2026-04-26
**Status**: GATED. Hard prerequisite: slice 029.
**Source**: [../022-frontend-resilient-offline-spike/analysis/followup-readiness-react-native.md](../022-frontend-resilient-offline-spike/analysis/followup-readiness-react-native.md)
## Problem Statement

## In Scope

- Feature implementation and integration
- Testing and validation of new behavior
- Documentation of feature usage and design decisions

Feature Specification: React Native Persistence + Retry aims to improve system capability and user experience by implementing the feature described in the specification.


## Goal

Adopt the shared resilience primitive on React Native using an
AsyncStorage adapter, plus an online-listener drain trigger driven
by `@react-native-community/netinfo`.

## Functional requirements

- **FR-031-01**: `AsyncStorageAdapter` implements `StorageAdapter`
  in `@banana/resilience` and passes the same vitest contract suite
  as the IndexedDB adapter.
- **FR-031-02**: Submitting RN verdict while offline preserves the
  draft and queues the request.
- **FR-031-03**: Coming back online (NetInfo signal) drains and
  renders the verdict.
- **FR-031-04**: Killing and reopening the app shows the last-N
  stored verdicts on a `HistoryScreen`.
- **FR-031-05**: tsc clean.

## Out of scope

- Camera (D-020-02).
- iOS background-fetch tuning (D-022-04).
- Encryption at rest (D-022-03).

## Validation lane

```
bun run --cwd src/typescript/shared/resilience tsc --noEmit
bun run --cwd src/typescript/shared/resilience test
bun run --cwd src/typescript/react-native tsc --noEmit
```

## Success criteria

- All FR-031-* hold.

## In-scope files

See [readiness packet](../022-frontend-resilient-offline-spike/analysis/followup-readiness-react-native.md#in-scope-files).
