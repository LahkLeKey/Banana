# Implementation Plan: Ensemble Predict Blank Page UX Spike

**Branch**: `feature/037-training-launch-surface-expansion` | **Date**: 2026-04-26 | **Spec**: [.specify/specs/042-frontend-ensemble-blank-page-spike/spec.md](.specify/specs/042-frontend-ensemble-blank-page-spike/spec.md)
**Input**: Feature specification from `.specify/specs/042-frontend-ensemble-blank-page-spike/spec.md`

## Summary

Investigate and eliminate the React UX regression where triggering ensemble prediction can reload into a blank page, while preserving the currently working verdict/escalation/retry baseline. The spike produces a bounded frontend implementation packet: verified root-cause hypotheses, explicit submit/state guardrails, and regression coverage requirements before coding tasks are generated.

## Technical Context

**Language/Version**: TypeScript 5.x, React 18.x, Bun runtime for workspace tests
**Primary Dependencies**: React app runtime in `src/typescript/react`, shared components in `@banana/ui`, frontend API client in `src/typescript/react/src/lib/api.ts`
**Storage**: Browser local/session state and in-memory React state; no new datastore in scope
**Testing**: Bun unit/component tests (`bun test`) plus focused DOM behavior checks in `src/typescript/react/src/App.test.tsx`
**Target Platform**: Web runtime (Vite/Bun) with parity considerations for Electron bridge behavior
**Project Type**: Frontend web application spike (React)
**Performance Goals**: No perceptible regression in ensemble submit responsiveness versus current baseline
**Constraints**: Must preserve existing verdict/escalation/retry UX contracts; no backend model contract changes; keep `VITE_BANANA_API_BASE_URL` contract intact
**Scale/Scope**: Ensemble predict interaction path and related frontend state/submit wiring only

## Constitution Check

*GATE: Must pass before Phase 0 research. Re-check after Phase 1 design.*

### Pre-Design Gate Check

- Principle I (Domain Core First): PASS. Spike focuses on frontend submit UX and does not relocate native business rules.
- Principle II (Layered Interop Contract): PASS. No controller/service/pipeline/native interop contract mutation in this planning slice.
- Principle III (Spec First Delivery): PASS. Feature spec and plan artifacts are created before implementation.
- Principle IV (Verifiable Quality Gates): PASS. Plan includes explicit regression test updates and reproducible validation path.
- Principle V (Documentation and Wiki Parity): PASS. Spike includes runbook/spec artifacts and ties to existing wiki sync contract.

### Post-Design Gate Check

- Principle I: PASS after design remains frontend-only and bounded.
- Principle II: PASS after contracts avoid backend/native interface changes.
- Principle III: PASS with research/data-model/contracts/quickstart prepared for tasks phase.
- Principle IV: PASS with measurable criteria and repeatable test commands.
- Principle V: PASS with spec artifacts aligned for downstream documentation updates.

No constitution violations are introduced by this planning scope.

## Project Structure

### Documentation (this feature)

```text
.specify/specs/042-frontend-ensemble-blank-page-spike/
├── plan.md
├── research.md
├── data-model.md
├── quickstart.md
├── contracts/
└── tasks.md
```

### Source Code (repository root)

```text
src/typescript/react/
├── src/
│   ├── App.tsx
│   ├── App.test.tsx
│   └── lib/
│       ├── api.ts
│       └── resilience-bootstrap.ts

src/typescript/shared/ui/
└── src/

scripts/
└── compose-run-profile.sh
```

**Structure Decision**: Use existing React app and test surfaces only. No new app package or runtime channel is introduced in this spike.

## Phase 0 Research Plan

- Confirm whether blank-page outcome is browser-level reload versus render-level blank state.
- Compare submit triggers (button click vs Enter key submit) and default form behavior.
- Audit pending-state and duplicate-submit paths for unintended navigation/state reset.
- Validate Electron bridge interactions as non-primary but potentially contributing context.

## Phase 1 Design Plan

- Define bounded frontend interaction model and state transitions for ensemble submit.
- Define contract for non-reload submit behavior and baseline-preservation checks.
- Define regression coverage expectations for tests that fail if blank-page behavior returns.

## Complexity Tracking

No constitution exceptions or complexity justifications are required for this spike plan.
