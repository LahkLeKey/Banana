# Feature Specification: Shared UI Components v2

**Feature Branch**: `[027-shared-ui-components-v2]`
**Created**: 2026-04-26
**Status**: GATED. Hard prerequisite: slice 026 has landed.
**Source**: [../021-frontend-shared-design-system-spike/analysis/followup-readiness-components-v2.md](../021-frontend-shared-design-system-spike/analysis/followup-readiness-components-v2.md)

## Goal

Refactor existing `shared/ui` components to consume tokens, and add
the new components SPIKE 020's follow-ups require:
`EscalationPanel`, `RetryButton`, `ErrorText`. All new components
ship token-driven from day 1; both web and native variants.

## Functional requirements

- **FR-027-01**: `BananaBadge`, `RipenessLabel`, `ChatMessageBubble`
  render byte-identical (web DOM with CSS variables resolving to
  same hex; RN style objects with same hex literals from tokens).
- **FR-027-02**: `BananaBadge` ensemble variant exists for native
  (closes inventory gap).
- **FR-027-03**: `RipenessLabel` exists for native (closes
  inventory gap).
- **FR-027-04**: New components: `EscalationPanel` (web + native),
  `RetryButton` (web + native), `ErrorText` (web + native).
- **FR-027-05**: vitest covers each new component for the
  documented states from SPIKE 020 baseline.

## Out of scope

- Per-channel consumption (slice 028).
- Behavior changes beyond styling source.
- Dark mode (D-021-01).

## Validation lane

```
bun run --cwd src/typescript/shared/ui tsc --noEmit
bun run --cwd src/typescript/shared/ui test
```

## Success criteria

- Refactored components visually identical.
- New components match baseline copy/cue.
- Native gaps closed.
- vitest coverage on each new component.

## In-scope files

See [readiness packet](../021-frontend-shared-design-system-spike/analysis/followup-readiness-components-v2.md#in-scope-files).
