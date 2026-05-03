# Feature Specification: Uptime Monitoring (139)

**Feature Branch**: `feature/139-uptime-monitoring`
**Created**: 2026-05-02
**Status**: Draft (pending spike 120 go verdict)
**Input**: Scaffolded from spike `120-observability-analytics-spike`

## In Scope

- Configure free-tier uptime monitoring (UptimeRobot or Better Uptime) for `https://banana.engineer` and `https://api.banana.engineer/health`.
- Set up email alerts to the team when either endpoint is down for > 5 minutes.
- Document the monitor configuration and alert routing in `docs/runbooks/uptime-monitoring.md`.

## Out of Scope

- Status page (separate future spec).
- PagerDuty or on-call rotation tooling.

## Success Criteria

```bash
ls docs/runbooks/uptime-monitoring.md
python scripts/validate-spec-tasks-parity.py .specify/specs/139-uptime-monitoring
```
