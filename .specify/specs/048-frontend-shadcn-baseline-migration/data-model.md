# Data Model: Frontend shadcn Baseline Migration (048)

## SubmitState (discriminated union)

```ts
export type SubmitState<TResult> =
  | { kind: "idle"; lastResult: TResult | null }
  | { kind: "submitting"; requestId: string; lastResult: TResult | null }
  | { kind: "success"; result: TResult; lastResult: TResult }
  | { kind: "error"; message: string; lastResult: TResult | null };
```

Invariants:
- `lastResult` is monotonically non-decreasing in information: once a `success` lands, all subsequent states keep `lastResult` populated (so the UI can keep showing the prior verdict during the next submit or after an error).
- `requestId` is required only in `submitting` and is used for in-flight de-duplication.

## SubmitAction

```ts
export type SubmitAction<TResult> =
  | { type: "submit"; requestId: string }
  | { type: "resolve"; requestId: string; result: TResult }
  | { type: "reject"; requestId: string; message: string }
  | { type: "reset" };
```

Guards:
- `resolve`/`reject` are ignored if `requestId` does not match the in-flight `submitting.requestId` (stale-response dropping).
- `submit` while `kind === "submitting"` is ignored (idempotency — protects against double-click).

## submitReducer transition table

| From `kind` | Action `type` | To `kind` | Notes |
|---|---|---|---|
| idle | submit | submitting | record `requestId`, carry `lastResult` |
| submitting | resolve (matching id) | success | promote `result` to `lastResult` |
| submitting | reject (matching id) | error | preserve `lastResult` |
| submitting | submit | submitting | ignored (idempotency guard) |
| success | submit | submitting | carry `lastResult = result` |
| error | submit | submitting | carry `lastResult` |
| any | reset | idle | clear `lastResult` |
| any | resolve/reject (non-matching id) | unchanged | stale-response drop |

## useSubmitMachine hook

```ts
export function useSubmitMachine<TResult>(): {
  state: SubmitState<TResult>;
  submit: (run: () => Promise<TResult>) => Promise<void>;
  reset: () => void;
};
```

`submit` generates a `requestId`, dispatches `submit`, awaits `run()`, and dispatches `resolve`/`reject` with the same id. The hook owns request-id minting so callers cannot mismatch.

## AppView (root selector)

```ts
type AppView =
  | { kind: "shell" }
  | { kind: "error-fallback"; error: Error };
```

`App` selects `AppView` and renders one of two non-null branches. The error branch is reached only via the top-level `ErrorBoundary`'s render fallback — `App` itself returns `{ kind: "shell" }` for every reachable `SubmitState`.

## UI primitive contracts

All primitives live in `src/typescript/react/src/components/ui/` and follow shadcn conventions:
- Forward refs to underlying DOM elements.
- Accept `className` merged via `cn(...)`.
- Use `class-variance-authority` for variant + size where applicable.
- Render visible content even with empty children (e.g., `Card` always renders its border).
