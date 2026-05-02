# Feature Specification: Mobile Parity Sprint (RN screen depth + share/upload)

**Feature Branch**: `066-mobile-parity-sprint`
**Created**: 2026-05-01
**Status**: Stub (scaffolded by feature 051 frontend-robustness spike)
**Wave**: fifth
**Domain**: react-native
**Depends on**: #053, #057

## Problem Statement

React-Native ships only Capture/History/Verdict screens with no offline cache, no share-target polish, no biometric auth, and no push notifications.

## In Scope *(mandatory)*

- Polish the iOS + Android share extension; route shared images through `share-handler.ts` with retry + queue.
- Add an offline cache for history using Expo SQLite (or MMKV).
- Adopt expo-secure-store for credential persistence (depends on 063 once shipped).
- Add Expo Notifications hooks for verdict-ready and operator-alert events.
- Match the React app's a11y baseline (depends on 057).

## Out of Scope *(mandatory)*

- Native module rewrites.
- Replacing Expo with bare React Native.

## Notes for the planner

- Run `speckit.specify` against this stub to expand User Scenarios, Functional Requirements, and Success Criteria before `speckit.plan`.
- Cross-layer dependencies (API or native changes) MUST be enumerated in the plan artifact and parity-governed (feature 047) when applicable.
- This stub was generated as part of the 15-feature follow-up inventory in [`.specify/specs/051-frontend-robustness-spike/spec.md`](../051-frontend-robustness-spike/spec.md).
