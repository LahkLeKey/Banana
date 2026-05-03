## Problem Statement

## In Scope

- Feature implementation and integration
- Testing and validation of new behavior
- Documentation of feature usage and design decisions

Feature Specification: Frontend Resilient Offline SPIKE aims to improve system capability and user experience by implementing the feature described in the specification.

# Feature Specification: Frontend Resilient Offline SPIKE

**Feature Branch**: `[022-frontend-resilient-offline-spike]`
**Created**: 2026-04-26
**Status**: Draft -- ready to investigate
**Type**: SPIKE (investigate -> plan a small family of bounded follow-up specs)
**Triggered by**: Strategic refocus to customer-facing value. Mobile
(React Native) customers cannot depend on continuous connectivity;
desktop (Electron) customers expect classifier responses to feel
instant even on flaky home networks. The current frontend channels
have no offline queueing, no optimistic verdict, and no persistence
of verdict history -- which makes the customer experience feel
fragile compared to native ML apps that handle offline natively.

## Problem

Today every customer classifier request is a synchronous round trip
to the API. Three failure modes show up in the field:

1. **Mobile offline**: RN customer pastes/captures while offline ->
   immediate error, request lost.
2. **Desktop flaky network**: Electron customer triggers verdict ->
   spinner for 5+ seconds before timeout -> request lost.
3. **No verdict history**: Closing and reopening any channel loses
   the prior verdict + escalation context, so the customer cannot
   refer back to the explanation.

The fix space is large (queueing, optimistic UI, persistence,
cross-device sync, conflict resolution), and naive implementation
would duplicate logic across three channels. We need a SPIKE to
pick the minimum viable resilience contract and decide where it
lives (shared package vs per-channel).

## Investigation goals

- **G1**: Failure-mode inventory. For React, Electron, and React
  Native: list what happens TODAY when (a) the customer is offline,
  (b) the API request times out, (c) the channel is closed and
  reopened. Output: failure-mode matrix.
- **G2**: Resilience primitive options. Evaluate three candidate
  homes for the offline / persistence logic:
  - **A**: Shared TS package (`src/typescript/shared/resilience/`)
    that exports a queueing + persistence primitive consumed by all
    three channels. Storage adapter is per-channel.
  - **B**: Per-channel implementations using each channel's native
    storage (IndexedDB for React/Electron, AsyncStorage for RN)
    with no shared abstraction.
  - **C**: Service worker (React/Electron only) + RN-specific
    queueing -- mobile and web/desktop use different mechanisms.
- **G3**: Pick exactly three follow-up implementation slices:
  - Shared queue / persistence primitive (the contract).
  - Electron persistence + retry adoption.
  - React Native persistence + retry adoption.
  (React web adoption rides on the shared primitive landing in
  slice 029 and is folded into that slice's acceptance.)
- **G4**: Produce a readiness packet for each follow-up slice
  (slice 029, 030, 031) that the implementation slices can pick up
  without re-litigation.

## Out of Scope

- Cross-device verdict sync (would require a server-side store;
  defer with explicit trigger).
- Conflict resolution between two devices submitting the same
  payload (defer with explicit trigger; depends on cross-device
  sync above).
- Customer journey or verdict copy decisions (SPIKE 020 owns those).
- Token + design system work (SPIKE 021 owns that).
- API or native changes.

## Deliverables

- `analysis/failure-mode-matrix.md` -- per-channel today-behavior on
  offline / timeout / channel-restart.
- `analysis/primitive-options.md` -- options A/B/C with pros/cons
  and recommendation.
- `analysis/storage-contract.md` -- the queue + persistence contract
  the shared primitive will expose, including the per-channel
  storage adapter shape.
- `analysis/followup-readiness-shared.md` -- bounded scope for
  slice 029 (shared primitive + React adoption).
- `analysis/followup-readiness-electron.md` -- bounded scope for
  slice 030 (Electron storage adapter + retry).
- `analysis/followup-readiness-react-native.md` -- bounded scope for
  slice 031 (RN storage adapter + retry).
- `analysis/deferred-registry.md` -- cross-device sync, conflict
  resolution, and any other deferred items with explicit triggers.

## Success Criteria

- Failure-mode matrix covers all three channels and all three
  failure modes per channel.
- Exactly one primitive option recommended with rationale tied to
  the failure-mode matrix.
- Storage contract is small enough to keep slice 029 <=20 tasks.
- Slice 030 and 031 each have a hard prerequisite on slice 029.
- No code or contract changes land in this SPIKE.

## Validation lane

```
# SPIKE-only -- no build/test required.
# Verification is human review of the seven deliverables above.
ls .specify/specs/022-frontend-resilient-offline-spike/analysis/
```

## Downstream

This SPIKE unlocks three sequenced implementation slices:

- **029 Shared Resilience Primitive + React Adoption** -- the queue
  + persistence contract, with React (web) as the first adopter.
- **030 Electron Persistence + Retry** -- Electron-specific storage
  adapter + retry policy on top of the shared primitive.
- **031 React Native Persistence + Retry** -- RN-specific storage
  adapter + retry policy on top of the shared primitive.

## In-scope files

- `src/typescript/react/**`, `src/typescript/electron/**`,
  `src/typescript/react-native/**`, `src/typescript/shared/**`
  (READ ONLY -- audit input).
- `.specify/specs/022-frontend-resilient-offline-spike/analysis/**`
  (NEW -- SPIKE outputs).
- `.specify/feature.json` (repoint to 022 during execution).
