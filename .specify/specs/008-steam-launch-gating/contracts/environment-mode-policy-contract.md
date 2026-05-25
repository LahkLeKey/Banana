# Contract: Environment Mode Policy

## Purpose

Define explicit launch environment semantics and fallback/override boundaries so development workflows remain practical while production gating is never weakened.

## Modes

| Mode | Intended Channel | Steam Required | Account Verification Required | Fallback Allowance |
|------|------------------|----------------|-------------------------------|--------------------|
| `development` | local developer run | Optional | Optional by policy | Allowed only with explicit local flag and audit event |
| `staging-uat` | internal test package | Required by default | Required | Limited fallback may be enabled for controlled UAT tests |
| `production-steam-package` | public deployed Steam package | Mandatory | Mandatory (`linked + good-standing`) | No allow fallback |

## Precedence Rules

1. Runtime mode is resolved from trusted build/runtime channel metadata before environment variables.
2. Environment variable overrides are ignored for `production-steam-package`.
3. Unknown mode resolves to deny-safe behavior.

## Override Contract

- Allowed override sources in non-production:
  - Local config file under repository-controlled path.
  - Explicit environment variable for dev shell runs.
- Prohibited override sources in production:
  - Runtime environment flags
  - Client command-line bypass switches
  - Cached prior-success markers

## Fallback Policy

- Development: optional startup fallback allowed for non-Steam iteration, must emit audit event with bypass reason.
- Staging/UAT: fallback allowed only via explicit test policy and tagged build metadata.
- Production: fallback MAY return user guidance but MUST NOT return launch allow.

## Required Audit Fields

- `attemptId`
- `resolvedMode`
- `overrideApplied` (boolean)
- `overrideSource`
- `decision`
- `reasonCode`
- `timestamp`
