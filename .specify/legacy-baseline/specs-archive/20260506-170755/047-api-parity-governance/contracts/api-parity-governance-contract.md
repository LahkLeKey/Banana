# Contract: Cross-API Feature Parity Governance

## Purpose

Define the enforceable governance contract that keeps overlapping ASP.NET (`:8080`) and Fastify (`:8081`) capability areas behaviorally aligned.

## Scope

- Applies to overlapping capability areas only.
- Covers route presence, status semantics, and response-shape parity.
- Excludes intentionally single-surface routes unless later promoted to overlap scope.

## Parity Inventory Contract

1. Inventory key is `METHOD + path`.
2. Each overlap record must map both surfaces:
   - ASP.NET source reference
   - Fastify source reference
3. Each overlap record must include expected statuses and required response fields.
4. Inventory must be versioned and auditable in feature artifacts.

## Drift Finding Taxonomy

1. `missing_route`: route exists on one surface but not the other.
2. `status_mismatch`: route exists on both but status semantics differ.
3. `shape_mismatch`: route exists on both and status aligns, but required response fields differ.

Each finding must include route key, mismatch type, observed values on both surfaces, and remediation guidance.

## Exception Contract

1. Exceptions are allowed only for bounded temporary divergence.
2. Required fields:
   - route_key
   - mismatch_type
   - owner
   - rationale
   - approved_at
   - expires_at
3. Expired or revoked exceptions immediately restore blocking behavior.
4. Exception scope is exact-match and must not suppress unrelated findings.

## Gate Logic (Explicit Pass/Fail)

- PASS when all overlap routes satisfy parity expectations OR each unresolved finding has an active, non-expired approved exception.
- FAIL when any of the following is true:
  - at least one `missing_route` finding has no active approved exception
  - at least one `status_mismatch` finding has no active approved exception
  - at least one `shape_mismatch` finding has no active approved exception
  - any exception is missing required governance fields
  - any exception is expired or revoked

## Constitution Alignment

- Principle VII (Cross-API Feature Parity): enforced by required overlap inventory + fail-closed drift gate.
- Principle IV (Verifiable Quality Gates): enforced by blocking gate semantics and actionable findings.
- Principle V (Documentation and Wiki Parity): enforced by storing parity contract and validation expectations in feature artifacts.

## Evidence Expectations

- Inventory snapshot with overlap count and route keys.
- Drift report with findings (if any) and mismatch taxonomy.
- Exception list with active/expired status.
- Gate decision output (`PASS` or `FAIL`) with explicit reason summary.
