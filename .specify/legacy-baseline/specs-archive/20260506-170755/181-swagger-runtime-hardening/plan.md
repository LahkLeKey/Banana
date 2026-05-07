# Implementation Plan: Swagger Runtime Hardening (181)

**Branch**: `181-swagger-runtime-hardening` | **Date**: 2026-05-03 | **Spec**: [spec.md](spec.md)
**Input**: Feature specification from `.specify/specs/181-swagger-runtime-hardening/spec.md`

## Overview

Stabilize Swagger/OpenAPI runtime generation for production API, enforce docs CORS contract, and add regression tests that guard the embedded banana.engineer docs experience.

## Technical Context

**Language/Version**: C#/.NET 8 (API), TypeScript/React (docs host)
**Primary Dependencies**: Swashbuckle.AspNetCore, ASP.NET middleware pipeline, swagger-ui-react
**Storage**: N/A
**Testing**: ASP.NET integration tests, focused browser/runtime checks, endpoint smoke checks
**Target Platform**: Fly-hosted API + Vercel-hosted React app
**Project Type**: Cross-layer reliability hardening
**Constraints**: Preserve existing controller/service contracts; maintain controller -> service -> pipeline -> native interop flow
**Scale/Scope**: `src/c-sharp/asp.net`, `tests`, and docs page behavior validation

## Constitution Check

- Domain layering preserved: PASS.
- Spec-first workflow preserved: PASS.
- Validation path defined: PASS.

## Workstreams

1. Capture exact runtime exception and failing symbol/schema path.
2. Patch Swagger configuration or DTO annotations causing runtime generation failure.
3. Ensure CORS policy explicitly supports `https://banana.engineer` -> `https://api.banana.engineer` docs fetch.
4. Add endpoint and integration tests for Swagger JSON availability.
5. Validate API Docs page consumes default spec URL successfully.

## Validation Approach

1. Run API build and relevant test suite locally.
2. Assert `/swagger/v1/swagger.json` returns HTTP 200 in local and production runtime.
3. Verify `Access-Control-Allow-Origin: https://banana.engineer` on spec fetch path.
4. Verify banana.engineer `/api-docs` renders operation list with no fetch error.
