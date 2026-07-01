# ADR-0003: Keycloak Per-Environment Fly Apps

- Status: accepted
- Date: 2026-06-30
- Context: global

## Problem

Banana is introducing Keycloak-backed login across frontend and API, with deployment on Fly.io. A shared Keycloak service across environments would reduce app count but increases cross-environment coupling, operational blast radius, and secret management complexity.

## Decision

Deploy Keycloak as one Fly app per environment (for example dev, staging, prod). Each environment has:

- its own Keycloak Fly app
- its own Keycloak database
- environment-local secrets and client credentials
- environment-local issuer URL consumed by frontend and API

Steam authentication is deferred for now to avoid migration blockers and focus on a single Keycloak-first authority.

## Consequences

- Positive:
  - Lower blast radius for auth incidents and misconfiguration
  - Clearer secret rotation and access boundaries per environment
  - Simpler rollback and environment-specific change control
- Negative:
  - More infrastructure objects to maintain
  - Higher baseline operational overhead for provisioning and updates
- Follow-up actions:
  - Define naming convention and secret matrix for all environments
  - Add runbook for credential rotation and issuer cutover
  - Add smoke checks that validate issuer and JWKS reachability per environment

## Alternatives considered

- One shared Keycloak app with realm-per-environment: rejected due to tighter coupling and higher cross-environment incident risk.
- Keep Steam as primary and postpone Keycloak: rejected for this phase because Keycloak-first reduces branching in migration work.
