# 022 -- Followup Readiness: Slice 031 (React Native Persistence + Retry)

Bounded scope packet for the RN-specific resilience slice.

**Hard prerequisite**: slice 029 has landed.

## Goal

Adopt the shared resilience primitive on React Native using the
AsyncStorage adapter, plus an online-listener drain trigger driven
by `@react-native-community/netinfo`.

## In-scope files

- `src/typescript/shared/resilience/src/adapters/async-storage.ts` (NEW;
  lives in shared package because RN-specific implementations of the
  shared interface still belong with the contract)
- `src/typescript/react-native/lib/resilience-bootstrap.ts` (NEW)
- `src/typescript/react-native/index.tsx` (consume queue + history;
  preserve draft on failure)
- `src/typescript/react-native/screens/HistoryScreen.tsx` (NEW;
  surfaces last-N verdicts)
- `src/typescript/react-native/package.json` (depend on
  `@banana/resilience`, `@react-native-async-storage/async-storage`,
  `@react-native-community/netinfo`)
- `src/typescript/react-native/app.json` (declare permissions if any
  -- AsyncStorage and NetInfo do not require new permissions on
  Android/iOS)
- vitest coverage for the AsyncStorage adapter (mock storage)

## Out of scope

- Camera / image upload (deferred from SPIKE 020).
- iOS background-fetch tuning (best-effort; explicit deferred item).
- Encryption at rest.

## Validation lane

```
bun run --cwd src/typescript/shared/resilience tsc --noEmit
bun run --cwd src/typescript/shared/resilience test
bun run --cwd src/typescript/react-native tsc --noEmit
```

## Estimated task count

12-15 tasks. Buckets: setup (1) + AsyncStorage adapter + tests (3) +
RN bootstrap (2) + index.tsx adoption (2) + HistoryScreen (2) +
NetInfo wiring (2) + close-out (2).

## Acceptance

- Submitting while offline preserves the draft and queues the
  request; coming back online drains and surfaces the verdict.
- Killing and reopening the app shows the last-N stored verdicts.
- AsyncStorage adapter passes shared test contract (same suite that
  the IndexedDB adapter passes; only the storage backend changes).
- tsc clean.
