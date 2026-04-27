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

## Recorded Evidence (2026-04-26)

### Command: inventory refresh

```bash
bash scripts/validate-api-parity-governance.sh --inventory-only
```

Observed outcome:
- command completed successfully
- overlapping-routes inventory regenerated
- exception ledger validation passed

### Command: parity gate execution (non-strict)

```bash
bash scripts/validate-api-parity-governance.sh
```

Observed outcome:
- command completed and wrote parity artifacts
- parity gate decision was FAIL with unresolved findings
- current drift report unresolved total: 29

### Command: parity gate execution (strict with approved temporary exceptions)

```bash
bash scripts/validate-api-parity-governance.sh --strict
```

Observed outcome:
- command completed successfully when active approved exceptions covered existing drift baseline
- parity gate decision was PASS
- strict mode still fails immediately when unresolved findings remain

### Command: parity script regression tests

```bash
python -m pytest tests/scripts/test_validate_api_parity_governance.py
```

Observed outcome:
- 3 tests passed

### Command: parity e2e contract tests

```bash
dotnet test tests/e2e/Banana.E2eTests.csproj --filter FullyQualifiedName~ApiParity
```

Observed outcome:
- 7 tests passed

### Notes

- Strict mode remains fail-closed by design when unresolved parity drift exists.
- Compose CI now enforces strict parity via the api-parity lane and publishes gate artifacts for triage.
