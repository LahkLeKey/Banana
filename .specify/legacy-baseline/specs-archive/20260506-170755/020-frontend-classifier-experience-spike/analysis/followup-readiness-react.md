# 020 -- Followup Readiness: Slice 023 (React Verdict UX Polish)

Bounded scope packet for the React (web) implementation slice.

## Goal

Adopt the canonical copy + escalation cue from
[copy-and-cue-baseline.md](./copy-and-cue-baseline.md), wire the
escalation panel to slice 017's `/ml/ensemble/embedding` route, and
ship the inline retry affordance.

## In-scope files

- `src/typescript/react/src/App.tsx` (rewire ensemble result rendering)
- `src/typescript/react/src/lib/api.ts` (add embedding fetch helper)
- `src/typescript/shared/ui/src/components/BananaBadge.tsx` (add
  escalation cue rendering; web only until SPIKE 021 lands tokens)
- `src/typescript/shared/ui/src/components/EscalationPanel.tsx` (NEW)
- `src/typescript/react/src/App.test.tsx` (NEW; vitest)
- `src/typescript/react/src/lib/api.test.ts` (extend for embedding)

## Out of scope

- Tokens / theming (SPIKE 021).
- Offline / queueing (SPIKE 022).
- Electron or RN consumption.
- Mobile-only escalation panel; this slice ships web-only escalation
  panel; RN gets its own in slice 025.

## Validation lane

```
bun run --cwd src/typescript/shared/ui tsc --noEmit
bun run --cwd src/typescript/react tsc --noEmit
bun run --cwd src/typescript/react test
```

## Estimated task count

12-15 tasks. Task buckets: setup (2) + escalation panel component (3)
+ App rewire (3) + retry affordance (2) + tests (3) + close-out (2).

## Acceptance

- All five copy strings from the baseline render byte-identical.
- Escalation panel opens / closes; fetches embedding summary on first
  open; surfaces error wording from baseline on failure.
- Retry button preserves last-submitted draft and clears on new draft.
- vitest + tsc clean.
