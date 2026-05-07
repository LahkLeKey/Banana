# Feature Specification: React Verdict UX Polish

**Feature Branch**: `[023-react-verdict-ux-polish]`
**Created**: 2026-04-26
**Status**: GATED. DO NOT EXECUTE until SPIKE 020 readiness packet is acknowledged.
**Source**: [../020-frontend-classifier-experience-spike/analysis/followup-readiness-react.md](../020-frontend-classifier-experience-spike/analysis/followup-readiness-react.md)
## Problem Statement

## In Scope

- Feature implementation and integration
- Testing and validation of new behavior
- Documentation of feature usage and design decisions

Feature Specification: React Verdict UX Polish aims to improve system capability and user experience by implementing the feature described in the specification.


## Goal

Adopt the canonical verdict copy + escalation cue from
[../020-.../analysis/copy-and-cue-baseline.md](../020-frontend-classifier-experience-spike/analysis/copy-and-cue-baseline.md),
wire the escalation panel to slice 017's
`/ml/ensemble/embedding` route, and ship inline retry on the React
(web) verdict surface.

## Functional requirements

- **FR-023-01**: Render canonical verdict copy verbatim for all
  four verdict states (banana confident/escalated, not_banana
  confident/escalated) plus the empty state.
- **FR-023-02**: When `EnsembleVerdict.did_escalate=true`, render
  the escalation cue ("needs a closer look" suffix + "Why?" link)
  next to the verdict text.
- **FR-023-03**: "Why?" link opens an inline collapsible panel that
  fetches `/ml/ensemble/embedding` (slice 017) and renders an
  embedding summary. Closed by default.
- **FR-023-04**: On verdict failure, render the baseline error
  wording for the matching failure class (network / timeout / 5xx /
  4xx / unknown).
- **FR-023-05**: Render a "Try again" button after error wording
  that resubmits the **last submitted draft** (not current input).
  Submitting a new draft clears the retry button.
- **FR-023-06**: No new ensemble or embedding API contract changes.
  Slice 017's existing route is consumed as-is.

## Out of scope

- Tokens / theming (SPIKE 021 / slices 026-028).
- Offline / queueing / cross-session history (SPIKE 022 / slice 029).
- Electron and React Native channels (slices 024, 025).
- Camera / image ingest.

## Validation lane

```
bun run --cwd src/typescript/shared/ui tsc --noEmit
bun run --cwd src/typescript/react tsc --noEmit
bun run --cwd src/typescript/react test
```

## Success criteria

- All five baseline copy strings render byte-identical.
- Escalation panel opens / closes; embedding fetch succeeds on first
  open; error path surfaces baseline wording.
- Retry button preserves last submitted draft; clears on new draft.
- vitest + tsc clean.

## In-scope files

See [readiness packet](../020-frontend-classifier-experience-spike/analysis/followup-readiness-react.md#in-scope-files).
