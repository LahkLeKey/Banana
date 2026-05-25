# Data Model: Steam Launch Gating

## Overview

This model defines native-launch verification, account linkage status, environment-mode controls, and remediation state needed to gate gameplay admission safely.

## Entity: EnvironmentModePolicy

| Field | Type | Description |
|-------|------|-------------|
| `mode` | enum | `development`, `staging-uat`, `production-steam-package` |
| `strictness_level` | integer | Monotonic strictness rank (higher is stricter) |
| `allow_non_steam_startup` | boolean | Whether startup without Steam context is allowed |
| `allow_cached_verification` | boolean | Whether cached evidence can be considered |
| `override_sources` | jsonb | Approved config sources and precedence |
| `created_at` | timestamp | Policy creation time |
| `updated_at` | timestamp | Last policy update |

**Validation rules**:
- `production-steam-package` MUST set `allow_non_steam_startup=false` and `allow_cached_verification=false`.
- Mode transitions cannot reduce strictness for deployed channels.
- Unknown mode values are invalid and default to deny in launch decisioning.

## Entity: SteamIdentityAssertion

| Field | Type | Description |
|-------|------|-------------|
| `assertion_id` | uuid | Assertion record identity |
| `steam_id` | string | Steam subject identifier |
| `session_ticket_hash` | string | Hash of launch ticket/session proof |
| `issued_at` | timestamp | Assertion issue time |
| `expires_at` | timestamp | Assertion expiration |
| `validation_status` | enum | `valid`, `invalid`, `expired`, `indeterminate` |
| `source_channel` | enum | `steam-client`, `steam-deck`, `test-harness` |

**Validation rules**:
- `steam_id` is required for any non-development allow decision.
- `expires_at` must be later than `issued_at`.
- `invalid`, `expired`, or `indeterminate` assertions cannot produce production allow.

## Entity: LinkedProductionAccountStatus

| Field | Type | Description |
|-------|------|-------------|
| `account_id` | uuid | Canonical production account ID |
| `steam_id` | string | Linked Steam identifier |
| `link_status` | enum | `linked`, `unlinked`, `pending-link`, `link-conflict` |
| `standing_status` | enum | `good-standing`, `restricted`, `suspended`, `closed` |
| `verification_source` | enum | `account-service`, `manual-ops` |
| `version` | bigint | Optimistic concurrency/version marker |
| `verified_at` | timestamp | Last authoritative verification time |

**Validation rules**:
- Production allow requires `link_status=linked` and `standing_status=good-standing`.
- `suspended`, `restricted`, `closed`, and `unlinked` states always deny.
- Verification version must increase on status changes.

## Entity: VerificationSessionContext

| Field | Type | Description |
|-------|------|-------------|
| `attempt_id` | uuid | Launch attempt ID |
| `mode` | enum | Effective mode used for decisioning |
| `steam_assertion_id` | uuid | Link to assertion record |
| `account_status_version` | bigint | Version of linked account status used |
| `attempt_count` | integer | Retry count in current interaction |
| `last_failure_reason` | string | Last deny reason code |
| `freshness_expires_at` | timestamp | Session freshness boundary |
| `created_at` | timestamp | Attempt creation |

**Validation rules**:
- Each retry increments `attempt_count`.
- Context cannot reuse expired freshness windows.
- Production retries require renewed authoritative verification after remediations.

## Entity: LaunchVerificationDecision

| Field | Type | Description |
|-------|------|-------------|
| `decision_id` | uuid | Decision record identity |
| `attempt_id` | uuid | Launch attempt reference |
| `decision` | enum | `allow`, `deny` |
| `reason_code` | enum | `OK`, `UNLINKED_ACCOUNT`, `SUSPENDED_OR_RESTRICTED`, `STALE_SESSION`, `STEAM_UNAVAILABLE`, `API_UNAVAILABLE`, `OFFLINE_UNVERIFIABLE`, `UNKNOWN_MODE` |
| `is_remediable` | boolean | Whether user can self-remediate |
| `remediation_ticket_id` | uuid | Optional remediation action reference |
| `mode` | enum | Effective mode at decision time |
| `decided_at` | timestamp | Decision timestamp |

**Validation rules**:
- `decision=allow` requires `reason_code=OK`.
- `decision=deny` requires non-`OK` reason code.
- In `production-steam-package`, fallback decisions cannot convert deny to allow.

## Entity: RemediationActionTicket

| Field | Type | Description |
|-------|------|-------------|
| `remediation_ticket_id` | uuid | Remediation ticket identity |
| `reason_code` | string | Triggering deny reason |
| `web_flow_url` | string | Registration/linking URL |
| `required_actions` | jsonb | Ordered remediation tasks |
| `status` | enum | `pending`, `in-progress`, `completed`, `expired` |
| `completed_at` | timestamp | Completion time |
| `retry_allowed_after` | timestamp | Earliest retry timestamp |

**Validation rules**:
- Must exist when `is_remediable=true`.
- `completed` does not grant access by itself; next launch must re-verify.
- Expired tickets require regeneration before retry guidance is shown.

## Relationships

- `EnvironmentModePolicy` 1..* `VerificationSessionContext`
- `SteamIdentityAssertion` 1..* `VerificationSessionContext`
- `LinkedProductionAccountStatus` 1..* `VerificationSessionContext`
- `VerificationSessionContext` 1..* `LaunchVerificationDecision`
- `LaunchVerificationDecision` 0..1 `RemediationActionTicket`

## State Transition Notes

- Remediation: `pending -> in-progress -> completed` or `expired`.
- Launch progression: each attempt yields exactly one terminal decision (`allow` or `deny`).
- Production policy transitions are forward-only in strictness unless approved by governance change.
