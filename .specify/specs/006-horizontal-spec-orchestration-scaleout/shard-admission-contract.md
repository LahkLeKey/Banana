# Shard Admission Contract

## Admission Gates

1. Extension health:
- Run `.specify/scripts/bash/spec-extension-preflight.sh --update-first --json`.
- Shard admission requires preflight status `pass` or approved `deferred` record.

2. Confidence gate:
- Run `.specify/scripts/bash/spec-confidence-gate.sh --confidence <n> --step <label>` before shard execution.
- Exit code `42` indicates below-threshold human checkpoint required.

3. Conflict gate:
- Candidate must not conflict with active parallel shard write set.
- Conflicting candidates route to sequential shard lane.

## Deferment Contract

If extension health cannot be remediated immediately, deferment file must include:

- `OWNER=<name>`
- `EXPIRY=<YYYY-MM-DD>`
- `REASON=<summary>`

Without these fields, shard admission fails.
