# 022 -- Failure-Mode Matrix

Today-behavior of each frontend channel under three failure modes.
Source: code reading of `src/typescript/{react,electron,react-native}/`
as of 2026-04-26.

## React (web) -- `src/typescript/react/src/App.tsx` + `lib/api.ts`

| Failure | Today behavior | Customer impact |
|---------|---------------|-----------------|
| Offline submit (chat / verdict) | `fetch` throws `TypeError: Failed to fetch` -> caught, written as `chatError` / `ensembleError` text below the form. Draft is set to empty before the request fires (`setDraft("")`), so the customer's text is **lost**. | Low data preservation. Customer must retype. |
| API timeout | No client-side timeout configured. Browser eventually aborts (~300s). Spinner stays on the button (`isSending=true`). | Looks frozen. |
| Channel close / reopen | Component unmount discards `messages`, `ensembleVerdict`, `ripenessResult`. Bootstrap creates a NEW chat session on next mount. | All history lost. |

## Electron (desktop) -- thin wrapper around React renderer

| Failure | Today behavior | Customer impact |
|---------|---------------|-----------------|
| Offline submit | Inherits React behavior. Additionally, if `BANANA_ELECTRON_RENDERER_URL` is unreachable on app start, the BrowserWindow shows a Chrome offline page with no Banana branding. | Worse than React: app appears broken at launch. |
| API timeout | Inherits React behavior. | Looks frozen. |
| App close / reopen | Window state not persisted (size, position). All in-memory React state lost. New session created on re-launch. | All history lost; layout reset. |

## React Native -- `src/typescript/react-native/index.tsx`

| Failure | Today behavior | Customer impact |
|---------|---------------|-----------------|
| Offline submit | `fetch` rejects -> caught, written as `chatError` text. Draft also cleared before request (`setDraft('')`). | Low data preservation. |
| API timeout | No timeout. RN fetch has a default ~60s. Spinner stays. | Looks frozen. |
| App background / foreground / kill | RN persists nothing across kill. Foregrounding from background keeps in-memory state if the OS hasn't reclaimed. Kill -> all state lost; new session on relaunch. | History lost more aggressively than web due to OS-driven kills. |

## Cross-channel summary

| Failure | React | Electron | RN |
|---------|-------|----------|----|
| Submit while offline | LOST | LOST | LOST |
| Timeout (no client cap) | FROZEN | FROZEN | FROZEN |
| Channel restart | NO HISTORY | NO HISTORY | NO HISTORY (worse) |
| Draft preserved on failure | NO | NO | NO |
| Retry affordance | NO | NO | NO |

The three channels exhibit the **same three failure classes** with
the **same root cause** (no client-side queue, no persistence, no
client timeout policy). A shared resilience primitive will pay back
across all three.
