# Feature Specification: i18n & Localization Scaffold

**Feature Branch**: `058-frontend-i18n-localization-scaffold`
**Created**: 2026-05-01
**Status**: Stub (scaffolded by feature 051 frontend-robustness spike)
**Wave**: third
**Domain**: shared/ui / react
**Depends on**: #053

## Problem Statement

All copy is hardcoded English in `src/typescript/react/src/lib/copy.ts`. No locale negotiation, no message-format support, no RTL.

## In Scope *(mandatory)*

- Adopt FormatJS (ICU MessageFormat) for the React app and shared/ui copy.
- Refactor `lib/copy.ts` to a message catalog seeded with `en-US`.
- Add a stub `es-MX` catalog as a smoke test of the pipeline.
- Wire locale negotiation via Accept-Language with a user override stored in Zustand (depends on 053).

## Out of Scope *(mandatory)*

- Translating all copy in v1 (only `en-US` ships fully populated).
- RTL audit (deferred).

## Notes for the planner

- Run `speckit.specify` against this stub to expand User Scenarios, Functional Requirements, and Success Criteria before `speckit.plan`.
- Cross-layer dependencies (API or native changes) MUST be enumerated in the plan artifact and parity-governed (feature 047) when applicable.
- This stub was generated as part of the 15-feature follow-up inventory in [`.specify/specs/051-frontend-robustness-spike/spec.md`](../051-frontend-robustness-spike/spec.md).
