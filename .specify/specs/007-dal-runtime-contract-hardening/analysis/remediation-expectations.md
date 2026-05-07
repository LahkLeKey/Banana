# Remediation Expectations

## Requirements

- Non-success DAL outcomes in scope include actionable remediation text where operationally meaningful.
- Remediation text is deterministic for equivalent outcome classes.
- Remediation guidance does not imply success or silent fallback.

## In-Scope Remediation Focus

- Missing configuration: mention `BANANA_PG_CONNECTION` explicitly.
- Native linkage/runtime dependency unavailable: mention native dependency path/setup context.
- Query failure: indicate DB execution failure class without exposing unsafe internals.

## Out-of-Scope

- Frontend-specific remediation UX wording.
- Multi-locale remediation strings.
