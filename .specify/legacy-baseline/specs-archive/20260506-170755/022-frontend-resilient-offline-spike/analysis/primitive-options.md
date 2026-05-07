# 022 -- Primitive Options

Three candidate homes for the offline / persistence logic, scored
against the failure-mode matrix.

## Option A -- Shared TS package + per-channel storage adapter

Single primitive at `src/typescript/shared/resilience/` exposes:

- A `RequestQueue` that accepts `{ key, payload, retryPolicy }` jobs.
- A `VerdictHistory` that persists verdict outcomes.
- A `StorageAdapter` interface that each channel implements
  (IndexedDB for React/Electron, AsyncStorage for RN).

| Score axis | Rating | Notes |
|------------|--------|-------|
| Shared-code reuse | HIGH | All retry / dedupe / coalescing logic ships once. |
| Channel-native fit | MEDIUM | Each channel still picks its native storage; the queue does not assume a backend. |
| Retro-fit cost | MEDIUM | Three channels each adopt the primitive in a single cutover slice. |
| Deferred-item exposure | LOW | Cross-device sync would slot in as a new `StorageAdapter`. |

## Option B -- Per-channel implementations, no shared abstraction

Each channel writes its own queue + persistence using native APIs
directly.

| Score axis | Rating | Notes |
|------------|--------|-------|
| Shared-code reuse | LOW | Three implementations of the same retry policy. |
| Channel-native fit | HIGH | Each channel uses idiomatic native APIs. |
| Retro-fit cost | HIGH | Three slices, each with full retry/dedupe logic. |
| Deferred-item exposure | HIGH | Cross-device sync would require touching all three implementations. |

## Option C -- Service worker (web/desktop) + RN-specific queueing

Web and Electron use a service worker to intercept fetches and
background-queue them. RN uses its own AsyncStorage-backed queue
(service workers do not exist on RN).

| Score axis | Rating | Notes |
|------------|--------|-------|
| Shared-code reuse | LOW-MEDIUM | Service worker logic is web-only; RN duplicates it. |
| Channel-native fit | MEDIUM | Service worker is the canonical web offline tool but adds an install dance + lifecycle complexity. Electron sometimes blocks service workers. |
| Retro-fit cost | HIGH | Two unrelated implementations. |
| Deferred-item exposure | MEDIUM | Cross-device sync still needs a separate primitive. |

## Recommendation: Option A

Recommended on the strength of the failure-mode matrix: the three
channels exhibit identical failure classes, so identical retry /
dedupe / coalescing logic is the right abstraction. Per-channel
storage adapters preserve native fit at the only seam where it
actually matters (the storage backend).

### Why not B?

The matrix shows three identical bug classes. Three separate fixes
guarantee three separate regressions over time.

### Why not C?

Service worker complexity (lifecycle, install scope, Electron
caveats) outweighs the benefit, given that we already need an
RN-specific queue. Option A's queue is the same design without the
worker indirection.
