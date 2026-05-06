# Feature Specification: React Native Capture Flow

**Feature Branch**: `[025-react-native-capture-flow]`
**Created**: 2026-04-26
**Status**: GATED. DO NOT EXECUTE until SPIKE 020 readiness packet is acknowledged.
**Source**: [../020-frontend-classifier-experience-spike/analysis/followup-readiness-react-native.md](../020-frontend-classifier-experience-spike/analysis/followup-readiness-react-native.md)
## Problem Statement

## In Scope

- Feature implementation and integration
- Testing and validation of new behavior
- Documentation of feature usage and design decisions

Feature Specification: React Native Capture Flow aims to improve system capability and user experience by implementing the feature described in the specification.


## Goal

Replace today's stub `BananaBadge count={0}` + hard-coded
`RipenessLabel` with a real verdict capture flow on React Native:
text-paste entry, share-sheet ingest from another app, and the
canonical escalation cue.

## Functional requirements

- **FR-025-01**: Capture screen accepts pasted text, calls verdict
  API, navigates to verdict screen.
- **FR-025-02**: Verdict screen renders canonical baseline copy and
  escalation cue (native variant of `EscalationPanel`).
- **FR-025-03**: Share-sheet from another app delivers a text payload
  to the capture screen (Android intent-filter + iOS share extension
  registration via `app.json`).
- **FR-025-04**: Errors render baseline wording.
- **FR-025-05**: All verdict API calls go through a new
  `react-native/lib/api.ts` mirroring the React `lib/api.ts` shape.

## Out of scope

- Camera capture (D-020-02).
- Tokens / theming (SPIKE 021).
- Offline / queueing / cross-session history (SPIKE 022; ships in
  slice 031).

## Validation lane

```
bun run --cwd src/typescript/shared/ui tsc --noEmit
bun run --cwd src/typescript/react-native tsc --noEmit
```

## Success criteria

- Customer can paste text and see verdict using canonical copy.
- Share-sheet ingest delivers payload to capture screen on Android +
  iOS (best-effort iOS).
- Native escalation panel renders cue from baseline.
- tsc clean for both react-native and shared/ui.

## In-scope files

See [readiness packet](../020-frontend-classifier-experience-spike/analysis/followup-readiness-react-native.md#in-scope-files).
