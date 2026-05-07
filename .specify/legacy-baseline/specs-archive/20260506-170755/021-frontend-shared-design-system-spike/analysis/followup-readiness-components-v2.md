# 021 -- Followup Readiness: Slice 027 (Components v2)

Bounded scope packet for the component refresh slice.

**Hard prerequisite**: slice 026 has landed (tokens are in place).

## Goal

Refactor existing `shared/ui` components to consume tokens, and add
the new components that SPIKE 020's follow-ups require:
`EscalationPanel` (web + native variants), `RetryButton`,
`ErrorText`. All new components ship token-driven from day 1.

## In-scope files

- `src/typescript/shared/ui/src/components/BananaBadge.tsx` (refactor
  to tokens; preserve public props)
- `src/typescript/shared/ui/src/components/RipenessLabel.tsx` (same)
- `src/typescript/shared/ui/src/components/ChatMessageBubble.tsx` (same)
- `src/typescript/shared/ui/src/components/EscalationPanel.tsx` (NEW)
- `src/typescript/shared/ui/src/components/RetryButton.tsx` (NEW)
- `src/typescript/shared/ui/src/components/ErrorText.tsx` (NEW)
- `src/typescript/shared/ui/src/native/BananaBadge.tsx` (refactor to
  tokens; ADD ensemble variant -- close gap from inventory)
- `src/typescript/shared/ui/src/native/RipenessLabel.tsx` (NEW;
  close gap from inventory)
- `src/typescript/shared/ui/src/native/EscalationPanel.tsx` (NEW)
- `src/typescript/shared/ui/src/native/RetryButton.tsx` (NEW)
- `src/typescript/shared/ui/src/native/ErrorText.tsx` (NEW)
- `src/typescript/shared/ui/src/index.ts` and
  `src/typescript/shared/ui/src/native/index.ts` (add exports)
- vitest coverage for each new component

## Out of scope

- Per-channel consumption (slice 028).
- Behavior changes to existing components beyond styling source.
- Dark mode.

## Validation lane

```
bun run --cwd src/typescript/shared/ui tsc --noEmit
bun run --cwd src/typescript/shared/ui test
# Visual byte-identity for refactored components:
git show HEAD~1:.../BananaBadge.test.tsx vs current rendered output
```

## Estimated task count

15-18 tasks. Buckets: setup (1) + 3 web component refactors (3) + 5
new web components (5) + 4 new native components + 2 native refactors
(6) + tests (3) + close-out (2). Spec author may split if it exceeds
20.

## Acceptance

- All existing components render visually identical (byte-equivalent
  RN style objects; pixel-equivalent web DOM with token-driven CSS
  custom properties resolving to the same hex values).
- New components render the canonical baseline copy and cues from
  SPIKE 020's `copy-and-cue-baseline.md`.
- `BananaBadge` ensemble variant exists for native (gap closed).
- `RipenessLabel` exists for native (gap closed).
- vitest covers each new component for the documented states.
