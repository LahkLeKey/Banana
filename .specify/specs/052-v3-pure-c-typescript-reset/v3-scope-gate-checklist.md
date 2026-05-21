# V3 Scope Gate Checklist

Date: 2026-05-17
Authority: Spec 052

## Gate rule

A slice is in-scope for V3 only if both conditions are true:

1. It affects customer-facing gameplay behavior or the currently retained gameplay runtime scaffold.
2. It lives in an active runtime lane: native, API, frontend, or shared gameplay contracts.

If either condition is false, the slice is out-of-scope for the current baseline.

## Inclusion checks

- [x] The candidate points to a live runtime entry path.
- [x] The candidate belongs to exactly one execution lane.
- [x] The candidate has at least one lane-local validation expectation.
- [x] The candidate does not require archived viewport-first behavior for baseline correctness.

## Exclusion checks

- [x] Workflow-only, training, workbench, and control-plane changes remain out of scope.
- [x] Placeholder support surfaces (`scripts`, `tests`, `docker`, `docs`, `data`) remain non-authoritative unless a future active slice promotes them with live runtime evidence.
- [x] Historical heartbeat claims and archived specs do not qualify as retained capabilities by themselves.

## Sampled scope-gate decisions

| Candidate | Lane | Decision | Evidence |
|---|---|---|---|
| Native ABI scaffold | A | IN | `src/native/CMakeLists.txt`, `src/native/include/banana_native_v3.h`, `src/native/scaffold/native_entry.c` |
| API package scaffold | B | IN | `src/typescript/api/package.json`, `src/typescript/api/src/index.ts` |
| React/Electron/React Native shells | C | IN | `src/typescript/react/src/index.ts`, `src/typescript/electron/main.js`, `src/typescript/react-native/index.ts` |
| Shared UI export scaffold | D | IN | `src/typescript/shared/ui/package.json`, `src/typescript/shared/ui/src/index.ts` |
| `scripts/README.md` | N/A | OUT | Delivery placeholder only |
| Historical `startSession` / `normalizeMovement` claims | N/A | OUT | No matching live source path |

## Review template

- Slice ID:
- Lane:
- Runtime entry paths:
- Customer-facing gameplay effect:
- Validation expectation:
- Scope-gate decision:
- Reviewer/date:
