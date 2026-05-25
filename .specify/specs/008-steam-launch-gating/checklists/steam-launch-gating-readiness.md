# Steam Launch Gating Readiness Checklist

## Contract Readiness

- [ ] Launch reason-code constants match launch-gate API contract enum.
- [ ] Native launch policy header is synced with API mode taxonomy.
- [ ] React blocked-launch types match API response payload shape.

## Implementation Readiness

- [ ] Verify endpoint and status endpoint are registered in player-facing v1 routes.
- [ ] Runtime launch boundary enforces allow/deny before gameplay startup.
- [ ] Remediation ticket flow is wired for remediable deny reason codes.

## Validation Readiness

- [ ] API contract tests cover allow/deny taxonomy and status lookups.
- [ ] Native launch matrix tests cover production deny-safe behavior.
- [ ] Integration evidence is captured in artifacts/api and artifacts/native.

## Release Readiness

- [ ] Production mode override bypass is blocked and audited.
- [ ] Retry and stale session behavior is validated against policy.
- [ ] Quickstart commands and evidence references are updated.
