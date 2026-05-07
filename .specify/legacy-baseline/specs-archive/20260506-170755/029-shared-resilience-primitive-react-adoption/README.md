# Slice 029 — Shared Resilience Primitive + React Adoption — COMPLETE

Branch: `feature/029-shared-resilience-primitive-react-adoption`

## What landed

- **New workspace package** `src/typescript/shared/resilience/`
  (`@banana/resilience`) per the storage contract from
  [SPIKE 022 storage-contract.md](../022-frontend-resilient-offline-spike/analysis/storage-contract.md):
  - `RetryPolicy`, `QueuedJob`, `RequestQueue`, `StoredVerdict`,
    `VerdictHistory`, `StorageAdapter` types.
  - `createRequestQueue` — persistent dedupe-by-key queue with
    single-pass drain, retry accounting, and exhaustion drop.
  - `createVerdictHistory` — last-N store, newest-first.
  - `createIndexedDbAdapter` — browser/desktop adapter with an
    automatic in-memory fallback when `globalThis.indexedDB` is
    unavailable (used by `bun:test`).
  - `createInMemoryAdapter` — exported for tests + degraded
    contexts.
- **Tests** (`bun test` — 9 pass / 0 fail):
  - `queue.test.ts`: happy-path drain, dedupe-by-key, retry
    exhaustion (3-attempt policy → 3 calls then drop), `clear(key)`
    / `clear()`.
  - `history.test.ts`: ordering, `list(limit)`, `maxEntries` trim,
    `clear`.
- **React adoption** (`src/typescript/react/`):
  - `src/lib/resilience-bootstrap.ts` instantiates singleton queue +
    history scoped to namespace `react-ensemble`.
  - `App.tsx`:
    - On successful ensemble verdict, records into `VerdictHistory`
      and refreshes the `Recent verdicts` surface.
    - On submit failure when `navigator.onLine === false`, enqueues
      the sample; the existing draft + retry affordance are
      preserved (FR-029-04).
    - On mount, hydrates the `Recent verdicts` surface from
      persistent storage so a reopened tab shows last-N (FR-029-05).
    - `window.addEventListener("online")` triggers `queue.drain` and
      renders whichever verdict resolves last (FR-029-04).
    - Online behavior is unchanged; the queue path only runs when
      offline (FR-029-06).
  - `package.json` depends on `@banana/resilience`.
  - `tsconfig.json` paths: `"@banana/resilience"` →
    `../shared/resilience/src/index.ts`.

## Validation lane

- `bunx tsc --noEmit` — clean across `shared/resilience`, `react`,
  `react-native`.
- `bun test` — `shared/resilience` 9/0; `react` 25/0;
  `shared/ui` 13/0.
- `bash scripts/validate-spec-tasks-parity.sh --all` — OK 005-031.

## Out of scope (deferred to dependent slices)

- Slice 030 — Electron persistence + retry (reuses
  `@banana/resilience` with the IndexedDB adapter in the renderer).
- Slice 031 — React Native persistence + retry (will ship an
  `AsyncStorageAdapter` implementation of `StorageAdapter`).
- Cross-device sync, encryption at rest, RN background sync — kept
  deferred per the SPIKE 022 contract.
