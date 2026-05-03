# Feature Specification: API Production Host (125)

**Feature Branch**: `feature/125-api-production-host`
**Created**: 2026-05-02
**Status**: Ready for implementation
**Input**: Scaffolded from spike `117-api-public-hosting-spike`

## In Scope

- Deploy the Fastify API (`src/typescript/api`) to the hosting platform recommended by spike 117.
- Configure `BANANA_PG_CONNECTION` as a runtime secret on the chosen host.
- Confirm the API responds to `GET /health` with HTTP 200 from a public URL.
- Document the deployment command and any required `Procfile`, `fly.toml`, or equivalent config.

## Out of Scope

- CORS changes (spec 126). API subdomain DNS (spec 127). Database migrations (spec 128).

## Success Criteria

```bash
curl -s https://api.banana.engineer/health | grep -i "ok"
python scripts/validate-spec-tasks-parity.py .specify/specs/125-api-production-host
```
