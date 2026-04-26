# Coverage 80 Exceptions Governance

## Purpose

This page defines the governance rules for temporary coverage exceptions introduced by feature 003.

## Required Metadata

Every exception record must include:

- exception_id
- domain
- layer (unit, integration, e2e)
- owner
- rationale
- remediation_plan
- expires_on (ISO date)

## Policy Rules

1. Exceptions are temporary and must include an expiry date.
2. Expired exceptions are treated as invalid and do not suppress baseline failures.
3. Malformed exceptions fail validation and do not suppress baseline failures.
4. Active exceptions are surfaced in lane and aggregate summaries for visibility.
5. Exception files are auditable artifacts and should be reviewed in PRs.

## Operator Workflow

1. Add or update exception records in the designated exceptions JSON file.
2. Run exception validation:

```bash
bash scripts/coverage-validate-exceptions.sh --exceptions <path> --output .artifacts/coverage/exceptions-validation.json
```

3. Confirm validation output reports `valid: true` and zero expired entries.
4. Trigger coverage normalization and verify exceptions appear in lane and aggregate artifacts.
5. Remove exception records immediately after remediation is merged.

## Escalation

If an exception must be extended, include:

- link to incident or blocker
- updated remediation plan with target date
- reviewer approval from delivery owner
