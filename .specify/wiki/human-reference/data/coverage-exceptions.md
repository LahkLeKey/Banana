<!-- breadcrumb: Data > Coverage Exceptions -->

# Coverage Exceptions

> [Home](../Home.md) › [Data](README.md) › Coverage Exceptions

Coverage exceptions are temporary safety valves, not permanent bypasses.

## Before You Add One

- Make sure there is a concrete remediation plan.
- Assign a clear owner.
- Set a real expiry date.

## What Must Be Included

- `exception_id`
- `domain`
- `layer`
- `owner`
- `rationale`
- `remediation_plan`
- `expires_on`

## Quick Validation Command

```bash
bash scripts/coverage-validate-exceptions.sh --exceptions <path> --output .artifacts/coverage/exceptions-validation.json
```

If validation fails, fix the record instead of bypassing the failure.

## Operational Expectations

1. Expired exceptions are treated as invalid and should cause follow-up action.
2. Invalid exception records never override the 80 percent baseline gates.
3. Active exceptions should be visible in lane and aggregate artifacts.
4. Remove exceptions as soon as remediation lands.

## Team Habit

During triage, always read exceptions together with lane summaries. This keeps accountability visible and avoids silent quality drift.
