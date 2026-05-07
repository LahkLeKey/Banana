# Tasks: Swagger Runtime Hardening (181)

**Input**: Design documents from `.specify/specs/181-swagger-runtime-hardening/`
**Prerequisites**: `plan.md`, `spec.md`

## Phase 1: Diagnose and Stabilize API Spec Generation

- [x] T001 Capture and document current `/swagger/v1/swagger.json` runtime exception from production logs.
  Files: `src/c-sharp/asp.net/README.md` (or nearest runbook note)

- [x] T002 Fix Swagger generation failure in API startup/annotations and preserve backward API contract.
  Files: `src/c-sharp/asp.net/Program.cs`, `src/c-sharp/asp.net/Controllers/*.cs`, `src/c-sharp/asp.net/Pipeline/**/*.cs`

- [x] T003 Add/confirm explicit CORS policy for docs host origin and apply to spec endpoint path.
  Files: `src/c-sharp/asp.net/Program.cs`

## Phase 2: Guardrails and Regression Tests

- [x] T004 Add API integration test for OpenAPI JSON endpoint success and JSON shape sanity.
  Files: `tests/integration/**/*.cs` or `tests/e2e/**/*.cs`

- [x] T005 Add API integration test asserting docs-host CORS behavior on Swagger JSON GET.
  Files: `tests/integration/**/*.cs` or `tests/e2e/**/*.cs`

- [x] T006 Add frontend docs-page smoke assertion for successful spec load against default URL.
  Files: `src/typescript/react/src/pages/ApiDocsPage.tsx`, `tests/e2e/**/*`

## Validation

- [x] T007 Run API test surface covering Swagger and CORS.
  Command: `dotnet test Banana.sln --filter "Swagger|OpenApi|Docs|Cors"`

- [x] T008 Verify live endpoint and headers.
  Commands: `curl -i https://api.banana.engineer/swagger/v1/swagger.json`, `curl -I -H "Origin: https://banana.engineer" https://api.banana.engineer/swagger/v1/swagger.json`

- [x] T009 Verify embedded docs page is interactive in browser.
  Checks: no fetch/CORS error banner, operation groups visible, Try-it-out enabled.
