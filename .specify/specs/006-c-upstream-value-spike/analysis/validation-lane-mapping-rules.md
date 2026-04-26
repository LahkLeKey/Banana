# Validation Lane Mapping Rules

## Purpose

Define how follow-up candidates map to validation expectations before implementation.

## Required Mapping Inputs

For each top candidate, identify:

- changed native surfaces
- impacted managed API surfaces
- impacted TS API/UI surfaces
- contract surfaces touched

## Lane Selection Rules

1. Native behavior change -> include `tests/native`.
2. ASP.NET contract/status/endpoint impact -> include `tests/unit` for affected controllers/services.
3. Contract lane behavior/critical journeys impact -> include `tests/e2e`.
4. TS API route/proxy impact -> include `bun run --cwd src/typescript/api test` (targeted or full).
5. If no production behavior change in a follow-up slice, still include minimally sufficient regression lane for impacted surfaces.

## Output Format

Each candidate must include:

- lanes_required
- minimum command set
- rationale for each lane
- residual test gaps (if any)
