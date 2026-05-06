# 025 React Native Capture Flow -- Execution Tracker

**Branch**: `025-react-native-capture-flow`
**Source SPIKE**: [020 Frontend Classifier Experience](../020-frontend-classifier-experience-spike/spec.md)
**Status**: COMPLETE (2026-04-26).

## Outcome

- React Native entry refactored from chat scaffold to a two-screen
  customer verdict flow:
  [CaptureScreen](../../../src/typescript/react-native/screens/CaptureScreen.tsx)
  -> [VerdictScreen](../../../src/typescript/react-native/screens/VerdictScreen.tsx).
- New mobile API client
  [lib/api.ts](../../../src/typescript/react-native/lib/api.ts) +
  copy helpers [lib/copy.ts](../../../src/typescript/react-native/lib/copy.ts)
  mirror the React web counterparts so any contract drift fails tsc on
  both surfaces.
- New native escalation panel at
  [shared/ui/src/native/EscalationPanel.tsx](../../../src/typescript/shared/ui/src/native/EscalationPanel.tsx)
  mirrors slice 023's web variant using React Native primitives only.
  Re-exported from `@banana/ui/native`.
- Share-sheet ingest seam
  [share-handler.ts](../../../src/typescript/react-native/share-handler.ts)
  routes Android `ACTION_SEND` text/plain and the iOS share extension
  payload (via `banana://share?text=...`) into `CaptureScreen`.
- [app.json](../../../src/typescript/react-native/app.json) declares
  the `banana` URL scheme, the iOS bundle id + share-extension
  activation rule, and Android intent-filters for both share + deep
  link.
- Pre-existing `RipenessLabel value="yellow"` tsc error in
  `index.tsx` was eliminated as part of T006 by replacing the chat
  scaffold with the new screen flow.

## Validation lane

```
bun install --cwd src/typescript/shared/ui
bun install --cwd src/typescript/react-native
cd src/typescript/shared/ui && bunx tsc --noEmit
cd src/typescript/react-native && bunx tsc --noEmit
```

Both tsc lanes exit 0. The Expo `web` smoke is deferred to slice 028
(per-channel design system adoption) since it depends on token wiring.

## Constraints

- Camera ingest deferred (D-020-02).
- Tokens deferred to SPIKE 021 / slices 026-028.
- Persistence + retry/offline deferred to slice 031.
