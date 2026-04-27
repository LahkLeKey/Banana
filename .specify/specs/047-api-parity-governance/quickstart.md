# Quickstart: API Parity Governance Validation

## Goal

Validate cross-API parity for overlapping ASP.NET and Fastify capability areas before merge.

## Prerequisites

- Workspace root: `c:/Github/Banana`
- Docker Desktop with Banana compose workflows available
- Runtime dependencies healthy

## Validation Flow

1. Start ASP.NET runtime dependencies:

```bash
bash scripts/compose-run-profile.sh --profile runtime --action up -- --build
bash scripts/compose-profile-ready.sh --profile runtime
```

2. Start Fastify API profile:

```bash
bash scripts/compose-run-profile.sh --profile api-fastify --action up -- --build
bash scripts/compose-profile-ready.sh --profile api-fastify
```

3. Verify both API health endpoints:

```bash
curl -sS -i http://localhost:8080/health | head -n 20
curl -sS -i http://localhost:8081/health | head -n 20
```

4. Build or refresh overlap inventory artifact from active route surfaces:

```bash
bash scripts/validate-api-parity-governance.sh --inventory-only
```

5. Detect missing-route gaps and write drift report artifact:

```bash
bash scripts/validate-api-parity-governance.sh
```

6. Execute parity comparison for each overlap route key (`METHOD + path`):
- compare route presence on both surfaces
- compare status semantics for expected scenarios
- compare required response fields

7. Evaluate gate using contract logic:
- PASS only if all findings are resolved or covered by active approved exception
- FAIL on any unresolved `missing_route`, `status_mismatch`, or `shape_mismatch`

8. Capture evidence bundle:
- inventory snapshot
- drift findings report
- exception ledger (if any)
- final gate decision summary

9. Teardown profiles when finished:

```bash
bash scripts/compose-run-profile.sh --profile api-fastify --action down
bash scripts/compose-run-profile.sh --profile runtime --action down
```

## Expected Result

- Cross-API parity gate returns explicit PASS/FAIL with actionable mismatch details.
- Any unresolved overlap drift blocks delivery until parity or approved exception is in place.
