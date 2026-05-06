# 022 -- Followup Readiness: Slice 029 (Shared Resilience Primitive + React Adoption)

Bounded scope packet for the foundational resilience slice.

## Goal

Land `src/typescript/shared/resilience/` per the storage contract,
ship the IndexedDB adapter, and adopt it in the React channel
(verdict + chat). React adoption is the acceptance test for the
primitive.

## In-scope files

- `src/typescript/shared/resilience/src/{index,queue,history,retry,types}.ts` (NEW)
- `src/typescript/shared/resilience/src/adapters/indexeddb.ts` (NEW)
- `src/typescript/shared/resilience/package.json` (NEW; `@banana/resilience`)
- `src/typescript/shared/resilience/tsconfig.json` (NEW)
- `src/typescript/shared/resilience/queue.test.ts` (NEW; vitest)
- `src/typescript/shared/resilience/history.test.ts` (NEW; vitest)
- `src/typescript/react/src/lib/resilience-bootstrap.ts` (NEW;
  factory wiring)
- `src/typescript/react/src/App.tsx` (consume queue + history;
  add "last verdicts" surface; add online-listener drain trigger;
  preserve draft on failure)
- `src/typescript/react/package.json` (depend on `@banana/resilience`)

## Out of scope

- Electron adapter (slice 030).
- React Native adapter (slice 031).
- Cross-device sync.
- Encryption at rest.

## Validation lane

```
bun run --cwd src/typescript/shared/resilience tsc --noEmit
bun run --cwd src/typescript/shared/resilience test
bun run --cwd src/typescript/react tsc --noEmit
bun run --cwd src/typescript/react test
```

## Estimated task count

15-18 tasks. Buckets: setup (2) + types + retry (2) + queue (3) +
history (2) + IndexedDB adapter (2) + tests (3) + React bootstrap +
adoption (3) + close-out (2).

## Acceptance

- vitest covers happy path + retry exhaustion + dedupe-by-key.
- Submitting while offline in React preserves the draft and queues
  the request; coming back online drains the queue and renders the
  verdict.
- Closing and reopening the React tab (same browser profile) shows
  the last-N verdicts.
- Existing React behavior is preserved when online (no perf
  regression).
