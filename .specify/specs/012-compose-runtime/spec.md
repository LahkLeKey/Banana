# Feature Specification: Compose Runtime & Local Orchestration (v2)

**Feature Branch**: `012-compose-runtime`  
**Created**: 2026-04-25  
**Status**: Draft  
**Parent**: [`005-v2-regeneration`](../005-v2-regeneration/spec.md)  
**Baseline**: [`legacy-baseline/compose-runtime.md`](../../legacy-baseline/compose-runtime.md)

## Summary

Regenerate the compose runtime and local orchestration with a small, named
set of channel scripts, deduplicated Dockerfiles where the source merger
allows, and rigorous CRLF + dockerignore + env hygiene preserved from v1.

## User Scenarios & Testing

### User Story 1 — Discoverable channel entry points (Priority: P1)

As a developer, I can list all canonical channel scripts from one place and
know which one to run for which channel.

### User Story 2 — Reproducible compose builds (Priority: P1)

As CI, image builds are reproducible: lockfile installs, pinned digests where
possible, normalized line endings on copied scripts.

### User Story 3 — Profile-aware tooling (Priority: P2)

As an operator, I always pass the required `--profile` flags to compose when
inspecting profile-gated services.

### User Story 4 — Single API Dockerfile per canonical API (Priority: P2)

As a maintainer, the dual `api.Dockerfile` + `api-fastify.Dockerfile` exists
only as long as both APIs are canonical for some routes (`007/008` ownership
contract); otherwise consolidated.

### Edge Cases

- Two-phase compose with `--no-recreate` after stale containers → "network not found"; mitigation: remove target service container before bringing up.
- Windows-authored shell script with CRLF copied into a Linux container → `/usr/bin/env: 'bash\r'`; enforce `*.sh text eol=lf` in `.gitattributes` and `sed -i 's/\r$//'` in Dockerfiles.

## Requirements

### Functional Requirements

- **FR-001**: v2 MUST keep canonical channel script naming: `launch-container-channels-with-wsl2-*.sh` and `compose-*-wsl2.sh`.
- **FR-002**: v2 MUST keep `.gitattributes` enforcing `*.sh text eol=lf`.
- **FR-003**: Dockerfiles that copy repo shell scripts MUST normalize line endings (`sed -i 's/\r$//'`).
- **FR-004**: v2 MUST keep `.dockerignore` exclusion of `**/node_modules`.
- **FR-005**: v2 MUST keep env contracts `BANANA_PG_CONNECTION`, `BANANA_NATIVE_PATH`, `VITE_BANANA_API_BASE_URL`, `DATABASE_URL`, `BANANA_WSL_DISTRO`.
- **FR-006**: v2 MUST keep exit code `42` for preflight failures across channel scripts.
- **FR-007**: v2 MUST keep GH CLI orchestration entry points: `scripts/workflow-orchestrate-triaged-item-pr.sh`, `scripts/workflow-orchestrate-sdlc.sh`, `scripts/workflow-sync-wiki.sh`.
- **FR-008**: v2 SHOULD consolidate API Dockerfiles when `008-typescript-api` route ownership permits.

### Hard contracts to preserve

- All scripts named in FR-001/FR-007.
- Env vars in FR-005.
- Exit code `42`.
- `.gitattributes` and `.dockerignore` invariants.
- Profile-gated service names from `010`/`011`.

### Pain points addressed

- Overlapping launch scripts → consolidated, one canonical per channel (FR-001).
- Dual API Dockerfiles → tracked vs `008` ownership (FR-008).
- Base image highs → mitigation via pinned digests; tracked as platform risk.

## Success Criteria

- **SC-001**: A single document (this spec or quickstart) lists every canonical channel script with purpose.
- **SC-002**: All Dockerfiles copying scripts include the CRLF normalization step.
- **SC-003**: All channel scripts honor exit code `42` semantics.
