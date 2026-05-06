# 020 -- Followup Readiness: Slice 025 (React Native Capture Flow)

Bounded scope packet for the React Native (mobile) implementation slice.

## Goal

Replace today's stub `BananaBadge count={0}` + hard-coded
`RipenessLabel` with a real verdict capture flow: text-paste entry,
share-sheet ingest, and the canonical escalation cue. Camera capture
deferred to a follow-up slice if customer demand surfaces (out of
scope here to keep slice <=20 tasks).

## In-scope files

- `src/typescript/react-native/index.tsx` (rewire to verdict flow;
  remove stub `BananaBadge`/`RipenessLabel` literals)
- `src/typescript/react-native/screens/CaptureScreen.tsx` (NEW)
- `src/typescript/react-native/screens/VerdictScreen.tsx` (NEW)
- `src/typescript/react-native/lib/api.ts` (NEW; fetch helpers
  mirroring the React `lib/api.ts` shape)
- `src/typescript/react-native/share-handler.ts` (NEW; share-sheet
  ingest registration)
- `src/typescript/shared/ui/src/native/EscalationPanel.tsx` (NEW;
  RN-flavored escalation panel mirroring slice 023's web variant)
- `src/typescript/react-native/app.json` (register share-sheet
  intents)

## Out of scope

- Camera capture (deferred with explicit trigger: requires native
  permissions wiring + image upload pipeline; a separate spec will
  pick this up if customer demand surfaces).
- Tokens / theming (SPIKE 021).
- Offline / queueing (SPIKE 022).
- iOS-specific share-sheet polish vs Android intent-filter polish
  (both ship; OS-specific UX is best-effort).

## Validation lane

```
bun run --cwd src/typescript/react-native tsc --noEmit
bun run --cwd src/typescript/shared/ui tsc --noEmit
```

## Estimated task count

15-18 tasks. Task buckets: setup (2) + api lib (2) + capture screen
(3) + verdict screen (3) + escalation panel native variant (2) +
share-sheet ingest (3) + close-out (2).

## Acceptance

- Customer can paste text -> see verdict using canonical copy.
- Share-sheet ingest from another app delivers text payload to the
  capture screen.
- Escalation cue + panel match baseline (token-free until slice 028
  adopts shared design tokens).
- Errors render baseline wording.
- tsc clean for both react-native and shared/ui.
