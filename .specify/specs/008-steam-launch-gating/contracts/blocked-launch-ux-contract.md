# Contract: Blocked Launch UX State Model

## Purpose

Standardize player-visible blocked-launch states, required actions, and retry semantics.

## Canonical States

| State | Trigger Reason Code | Primary Message Intent | Required User Action |
|-------|---------------------|------------------------|----------------------|
| `blocked-unlinked` | `UNLINKED_ACCOUNT` | Account is not linked to Steam for production access | Open web linking flow and complete account link |
| `blocked-standing` | `SUSPENDED_OR_RESTRICTED` | Account is restricted from gameplay | Review account status guidance; contact support if needed |
| `blocked-stale-session` | `STALE_SESSION` | Verification session expired | Re-authenticate Steam and retry launch |
| `blocked-steam-unavailable` | `STEAM_UNAVAILABLE` | Steam identity service unavailable | Retry after backoff window |
| `blocked-api-unavailable` | `API_UNAVAILABLE` | Account verification service unavailable | Retry after backoff window |
| `blocked-offline` | `OFFLINE_UNVERIFIABLE` | Online verification required for production launch | Reconnect network and retry |

## UX Payload Requirements

- `headline` (short reason-aware title)
- `details` (clear non-technical explanation)
- `nextStepLabel` (action button text)
- `nextStepTarget` (URL/deep-link/inline action)
- `retryAllowed` (boolean)
- `retryAfterMs` (integer)
- `supportCode` (stable machine-readable reason)

## Retry Rules

1. Retry is user-initiated only.
2. Retry must trigger a fresh verification call.
3. Completing remediation flow does not bypass fresh verification.

## Accessibility/Clarity Rules

- Messages must avoid ambiguous terms like "unknown error" when a known reason code exists.
- Action labels must be explicit (for example, "Link Account", "Retry Verification").
- State changes should preserve prior reason context for support diagnostics.
