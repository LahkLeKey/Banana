# Contract: Web Account Registration and Linking Remediation

## Purpose

Define the remediation handoff between launch gate denial responses and web registration/linking completion.

## Endpoint: Create Remediation Ticket

- `POST /v1/account-link/remediation-ticket`
- Request body:
  - `attemptId` (string, required)
  - `reasonCode` (string, required)
  - `steamIdHint` (string, optional)
- Response body:
  - `remediationTicketId` (string)
  - `webFlowUrl` (string)
  - `requiredActions` (array)
  - `expiresAt` (timestamp)

## Endpoint: Remediation Status

- `GET /v1/account-link/remediation-ticket/{remediationTicketId}`
- Response body:
  - `status` (enum): `pending`, `in-progress`, `completed`, `expired`
  - `completedActions` (array)
  - `retryAllowedAfter` (timestamp)

## Endpoint: Link Completion Callback

- `POST /v1/account-link/completion`
- Request body:
  - `remediationTicketId` (string, required)
  - `linkedAccountId` (string, required)
  - `completedAt` (timestamp, required)
- Response body:
  - `accepted` (boolean)
  - `nextStep` (enum): `relaunch-and-verify`, `contact-support`

## Security and Integrity

1. Ticket creation is allowed only from a deny decision context with remediable reason codes.
2. Completion callback does not grant launch access; it only marks remediation complete.
3. Native launch must perform a fresh `/v1/launch-gate/verify` call after completion.
4. Expired tickets cannot be reused and must be regenerated.
