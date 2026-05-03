# Feature Specification: Swagger Runtime Hardening (181)

**Feature Branch**: `181-swagger-runtime-hardening`
**Created**: 2026-05-03
**Status**: Draft
**Input**: Production failure on `https://api.banana.engineer/swagger/v1/swagger.json` returning HTTP 500 and blocking embedded API docs on banana.engineer.

## Problem Statement

Swagger UI shell is reachable, but OpenAPI JSON generation fails at runtime. This breaks developer self-serve testing and causes API Docs page regressions.

## In Scope

- E1: Resolve runtime exception path causing Swagger JSON HTTP 500.
- E2: Add safe OpenAPI generation guardrails for schema/doc comment failures.
- E3: Add explicit production CORS allow-list coverage for docs consumption from `https://banana.engineer`.
- E4: Add diagnostics and health assertions for Swagger JSON readiness.
- E5: Add regression tests for spec endpoint and embedded docs fetch path.

## Out of Scope

- Major redesign of API surface area.
- Generated client SDK distribution.
- AuthN/AuthZ model changes.

## User Scenarios & Testing

### User Story 1 - Open API Docs Reliably (Priority: P1)

As a developer, I can load and execute API docs from banana.engineer without fetch failures.

**Why this priority**: This is the primary DX entry point and currently blocked.

**Independent Test**: Load `/api-docs`, verify spec fetch success and visible operation list.

**Acceptance Scenarios**:

1. **Given** API host is healthy, **When** `/swagger/v1/swagger.json` is requested, **Then** it returns HTTP 200 with valid OpenAPI JSON.
2. **Given** docs page is opened from `https://banana.engineer`, **When** Swagger UI requests the spec, **Then** CORS allows request and docs render operations.

---

### User Story 2 - Diagnose Failures Quickly (Priority: P2)

As an operator, I can identify Swagger generation failures from structured logs and health diagnostics.

**Why this priority**: Lowers MTTR when docs regress.

**Independent Test**: Trigger known-invalid doc annotation in non-prod test harness and verify clear error classification in logs.

**Acceptance Scenarios**:

1. **Given** schema/doc generation failure, **When** request is made, **Then** logs include actionable exception details with route context.
2. **Given** health checks run, **When** Swagger JSON endpoint degrades, **Then** diagnostics indicate docs-not-ready state.

## Requirements

### Functional Requirements

- FR-001: `/swagger/v1/swagger.json` MUST return HTTP 200 and parse as JSON in production.
- FR-002: API MUST return CORS headers permitting `https://banana.engineer` for Swagger JSON GET requests.
- FR-003: Swagger generation path MUST emit structured error logs on failure with endpoint context.
- FR-004: API docs route in React app MUST default to production API spec URL and show operation list on success.
- FR-005: CI/local validation MUST include at least one test that asserts OpenAPI document generation and endpoint availability.

## Success Criteria

- SC-001: Production `curl https://api.banana.engineer/swagger/v1/swagger.json` returns HTTP 200.
- SC-002: API Docs page no longer shows fetch/CORS error banner for default URL.
- SC-003: New tests fail if Swagger JSON endpoint regresses to non-200.
