# Implementation Plan: 861 Telemetry Light Theme

**Branch**: `861-telemetry-light-theme` | **Date**: 2026-05-06 | **Spec**: `.specify/specs/861-telemetry-light-theme/spec.md`
**Input**: Feature specification from `.specify/specs/861-telemetry-light-theme/spec.md`

## Summary

Deliver a light-first telemetry dashboard presentation and enforce dashboard copy policy checks that block restricted vendor wording in user-visible telemetry dashboard text.

## Technical Context

**Language/Version**: TypeScript, React 19, Vite
**Primary Dependencies**: Bun, React Testing Library, existing UI card/table components
**Storage**: N/A (UI and policy checks only)
**Testing**: `bun test`, dashboard-focused assertions, CI copy-policy check
**Target Platform**: Browser (desktop/mobile responsive), GitHub Actions CI
**Project Type**: Frontend (React)
**Performance Goals**: Preserve current dashboard render responsiveness; no additional network calls
**Constraints**: Keep existing telemetry API contracts and test IDs stable unless explicitly revised
**Scale/Scope**: Telemetry dashboard route and CI copy-policy validation surface

## Constitution Check

*GATE: Must pass before Phase 0 research. Re-check after Phase 1 design.*

- Preserve established API contracts for telemetry hydration and event feed handling.
- Scope changes to React telemetry dashboard domain and related test/policy checks.
- Keep CI behavior deterministic with explicit failure output for policy violations.

## Project Structure

- `src/typescript/react/src/pages/TelemetryDashboardPage.tsx`
- `src/typescript/react/src/pages/TelemetryDashboardPage.test.tsx`
- `src/typescript/react/src` (copy-policy checker location to be decided)
- `.github/workflows` (if policy check requires workflow integration)

## Phases

### Phase 1: Design and Guardrails

- Finalize light-first visual criteria (contrast, readability, card/table/chip states).
- Define restricted-term list and target file scope for dashboard policy checks.
- Decide checker entry point (test script vs dedicated validation script).

### Phase 2: Implementation

- Apply light-first telemetry dashboard style refinements.
- Update dashboard copy to comply with policy rules.
- Add copy-policy checker with clear file/token diagnostics.
- Wire checker into local script and CI execution path.

### Phase 3: Validation

- Run focused telemetry dashboard tests.
- Run TypeScript checks for React app.
- Run policy checker with compliant and seeded-violation scenarios.
- Capture final evidence in PR notes.
