# 031 React Native Persistence + Retry -- Execution Tracker

**Branch**: `031-react-native-persistence-retry`
**Source SPIKE**: [022 Frontend Resilient Offline](../022-frontend-resilient-offline-spike/spec.md)
**Status**: COMPLETE.

## Phases (mapped to tasks.md)

1. Setup (T001-T002) -- complete.
2. AsyncStorage adapter (T003-T004) -- complete; 12/12 resilience tests pass.
3. RN bootstrap + adoption (T005-T008) -- complete.
4. History screen (T009-T010) -- complete.
5. Close-out (T011-T012) -- complete; tsc clean across resilience + react-native.

## Implementation summary

- `src/typescript/shared/resilience/src/adapters/async-storage.ts` --
  pure factory `createAsyncStorageAdapter(client: AsyncStorageLike)`
  taking the AsyncStorage shape as a parameter so the shared package
  never imports the native module.
- `src/typescript/shared/resilience/async-storage.test.ts` -- contract
  tests cover round-trip + `RequestQueue` dedupe-by-key drain +
  `VerdictHistory` cap=2 ordering against an in-memory AsyncStorage
  double.
- `src/typescript/react-native/lib/resilience-bootstrap.ts` --
  dynamic-`require()` of `@react-native-async-storage/async-storage`
  and `@react-native-community/netinfo` with in-memory fallback.
  Exports `getEnsembleQueue`, `getVerdictHistory`, `onOnline`,
  `ENSEMBLE_RETRY_POLICY`.
- `src/typescript/react-native/screens/CaptureScreen.tsx` -- on
  failure enqueues the sample (draft preserved by state), on success
  records to `VerdictHistory`. Also exposes a "Recent verdicts" link.
- `src/typescript/react-native/index.tsx` -- routes `capture` /
  `verdict` / `history`. NetInfo online-listener drains the queue,
  re-classifies, records to history, and routes to `VerdictScreen`.
- `src/typescript/react-native/screens/HistoryScreen.tsx` -- lists
  the last 25 verdicts via `getVerdictHistory().list(25)`.
- `src/typescript/react-native/tsconfig.json` -- adds path mapping
  `@banana/resilience` -> `../shared/resilience/src/index.ts` so tsc
  resolves the shared package without requiring `bun install`.
- `src/typescript/react-native/package.json` -- declares
  `@banana/resilience`, `@react-native-async-storage/async-storage`,
  `@react-native-community/netinfo` for runtime install.

## FR coverage

- FR-031-01: shared adapter test suite passes (12 pass / 0 fail).
- FR-031-02: failure path enqueues + draft state preserved.
- FR-031-03: NetInfo `onOnline` drains and routes to verdict.
- FR-031-04: `HistoryScreen` rehydrates from AsyncStorage on mount.
- FR-031-05: tsc clean for both `shared/resilience` and `react-native`.

## Validation lane

```
bun run --cwd src/typescript/shared/resilience tsc --noEmit
bun run --cwd src/typescript/shared/resilience test
bun run --cwd src/typescript/react-native tsc --noEmit
```

## Constraints

- Adapter passes the shared contract suite (parametric).
- Camera / iOS background-fetch / encryption all deferred.
