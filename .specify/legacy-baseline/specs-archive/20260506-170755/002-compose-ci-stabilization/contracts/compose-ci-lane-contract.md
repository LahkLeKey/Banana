# Contract: Compose CI Lane Stabilization

## Purpose

Define deterministic terminal behavior, diagnostics evidence, and artifact publication rules for merge-gated Compose lanes.

## Lane Inventory

| Lane | Primary Entry | Required Terminal States |
|---|---|---|
| compose-tests | `docker compose --profile tests up ... test-all` | pass, fail |
| compose-runtime | `docker compose --profile runtime up ...` + API health check | pass, fail |
| compose-electron | `docker compose --profile electron ... electron-example` | pass, fail |
| e2e-smoke | `scripts/compose-e2e-bootstrap.sh` + optional e2e project execution | pass, fail, skip |

## Terminal State Contract

Each lane must emit a terminal record with:

- `lane_name`
- `status` in {`pass`, `fail`, `skip`}
- `stage`
- `reason_code`
- `exit_code` (when command exited)
- `diagnostics_bundle_path`

### Rules

- `fail` must include failing stage and reason code.
- `skip` is only valid for explicitly optional surfaces and must include skip reason.
- Ambiguous command exits without classification are contract violations.

## Diagnostics Bundle Contract

For each lane, diagnostics must be published through stable paths and available for both fail and skip states.

### Minimum required files

- lane summary (`summary.txt` or structured JSON)
- compose/service snapshot (`compose-ps.txt` when compose is involved)
- lane logs (service or script logs)
- artifact manifest (`manifest.json`)

### Optional files

- health payload captures (`health.json`, domain payload JSON)
- preflight checks (`preflight.txt`)

## Artifact Publication Contract

Each upload step must classify outputs as required or optional.

### Required behavior

- Required outputs: missing paths fail lane and point to diagnostics root cause.
- Optional outputs: missing paths create skip/fallback artifact and keep lane deterministic.
- Upload steps must not fail with unresolved missing-path conditions for optional surfaces.

## Optional Surface Skip Contract

Optional project detection (for example missing integration/e2e csproj) must emit:

- explicit machine-readable record (`*.json` gate or manifest flag)
- explicit human-readable note (`*.skipped.txt`)

Silent skip by condition checks alone does not satisfy this contract.

## Runtime Compatibility Contract

Merge-gated workflows must not carry unowned Node runtime deprecation warnings.

- Preferred: upgrade action references to runtime-compatible versions.
- Temporary exception path: add owner + target remediation date and attach to workflow evidence.

## Validation Mapping

- FR-001, FR-002, FR-006, FR-009 map to terminal + diagnostics contracts.
- FR-003, FR-004, FR-005 map to artifact publication and skip contracts.
- FR-008 maps to runtime compatibility contract.
- SC-001 through SC-004 map to measurable checks in workflow verification windows.
