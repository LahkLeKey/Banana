# Issue Breakdown: Swagger Runtime Hardening (181)

Repository issue order derived from [tasks.md](tasks.md).

## Issue 1

Title: [181] Capture and document Swagger JSON 500 root cause

Body:
Capture and document the runtime exception causing `/swagger/v1/swagger.json` HTTP 500 in production. Record the failing symbol or schema path in runbook notes.

- Source Task: T001
- Depends On: None
- Files: src/c-sharp/asp.net/README.md

CLI:
`gh issue create --title "[181] Capture and document Swagger JSON 500 root cause" --body "Capture and document the runtime exception causing /swagger/v1/swagger.json HTTP 500 in production. Record the failing symbol or schema path in runbook notes.\n\nSource Task: T001\nDepends On: None\nFiles: src/c-sharp/asp.net/README.md"`

## Issue 2

Title: [181] Fix Swagger generation failure in API annotations

Body:
Patch Swagger configuration, controller annotations, or DTO XML doc comments causing the OpenAPI JSON generation to fail at runtime. Preserve existing API contracts.

- Source Task: T002
- Depends On: Issue 1
- Files: src/c-sharp/asp.net/Program.cs, src/c-sharp/asp.net/Controllers/*.cs, src/c-sharp/asp.net/Pipeline/**/*.cs

CLI:
`gh issue create --title "[181] Fix Swagger generation failure in API annotations" --body "Patch Swagger configuration, controller annotations, or DTO XML doc comments causing the OpenAPI JSON generation to fail at runtime. Preserve existing API contracts.\n\nSource Task: T002\nDepends On: [181] Capture and document Swagger JSON 500 root cause\nFiles: src/c-sharp/asp.net/Program.cs, src/c-sharp/asp.net/Controllers/*.cs, src/c-sharp/asp.net/Pipeline/**/*.cs"`

## Issue 3

Title: [181] Enforce CORS allow-list for banana.engineer docs origin

Body:
Add or confirm CORS policy in Program.cs that explicitly permits https://banana.engineer to make GET requests to /swagger/v1/swagger.json so the embedded API Docs page can fetch the spec.

- Source Task: T003
- Depends On: Issue 2
- Files: src/c-sharp/asp.net/Program.cs

CLI:
`gh issue create --title "[181] Enforce CORS allow-list for banana.engineer docs origin" --body "Add or confirm CORS policy in Program.cs that explicitly permits https://banana.engineer to make GET requests to /swagger/v1/swagger.json so the embedded API Docs page can fetch the spec.\n\nSource Task: T003\nDepends On: [181] Fix Swagger generation failure in API annotations\nFiles: src/c-sharp/asp.net/Program.cs"`

## Issue 4

Title: [181] Add integration test for OpenAPI JSON endpoint availability

Body:
Add an integration test asserting /swagger/v1/swagger.json returns HTTP 200 and parses as a valid JSON document with basic OpenAPI shape sanity.

- Source Task: T004
- Depends On: Issue 2
- Files: tests/integration/**/*.cs or tests/e2e/**/*.cs

CLI:
`gh issue create --title "[181] Add integration test for OpenAPI JSON endpoint availability" --body "Add an integration test asserting /swagger/v1/swagger.json returns HTTP 200 and parses as a valid JSON document with basic OpenAPI shape sanity.\n\nSource Task: T004\nDepends On: [181] Fix Swagger generation failure in API annotations\nFiles: tests/integration/**/*.cs or tests/e2e/**/*.cs"`

## Issue 5

Title: [181] Add CORS assertion test for docs-host origin on Swagger GET

Body:
Add integration test confirming Access-Control-Allow-Origin header is returned for the banana.engineer origin when requesting the Swagger JSON spec endpoint.

- Source Task: T005
- Depends On: Issues 3, 4
- Files: tests/integration/**/*.cs or tests/e2e/**/*.cs

CLI:
`gh issue create --title "[181] Add CORS assertion test for docs-host origin on Swagger GET" --body "Add integration test confirming Access-Control-Allow-Origin header is returned for the banana.engineer origin when requesting the Swagger JSON spec endpoint.\n\nSource Task: T005\nDepends On: [181] Enforce CORS allow-list for banana.engineer docs origin, [181] Add integration test for OpenAPI JSON endpoint availability\nFiles: tests/integration/**/*.cs or tests/e2e/**/*.cs"`

## Issue 6

Title: [181] Add frontend smoke check for API Docs page spec load

Body:
Add a smoke assertion that loads /api-docs and verifies the Swagger UI component renders an operation list rather than a fetch/CORS error banner.

- Source Task: T006
- Depends On: Issues 3, 4
- Files: src/typescript/react/src/pages/ApiDocsPage.tsx, tests/e2e/**/*

CLI:
`gh issue create --title "[181] Add frontend smoke check for API Docs page spec load" --body "Add a smoke assertion that loads /api-docs and verifies the Swagger UI component renders an operation list rather than a fetch/CORS error banner.\n\nSource Task: T006\nDepends On: [181] Enforce CORS allow-list for banana.engineer docs origin, [181] Add integration test for OpenAPI JSON endpoint availability\nFiles: src/typescript/react/src/pages/ApiDocsPage.tsx, tests/e2e/**/*"`

## Issue 7

Title: [181] Verify Swagger endpoint and headers in production

Body:
Run validation commands post-deploy: curl -i https://api.banana.engineer/swagger/v1/swagger.json must return 200; curl -I with Origin: https://banana.engineer must return CORS allow headers. Gate deploy on this check passing.

- Source Task: T008
- Depends On: Issues 2, 3
- Files: scripts or CI validation step

CLI:
`gh issue create --title "[181] Verify Swagger endpoint and headers in production" --body "Run validation commands post-deploy: curl -i https://api.banana.engineer/swagger/v1/swagger.json must return 200; curl -I with Origin: https://banana.engineer must return CORS allow headers. Gate deploy on this check passing.\n\nSource Task: T008\nDepends On: [181] Fix Swagger generation failure in API annotations, [181] Enforce CORS allow-list for banana.engineer docs origin\nFiles: scripts or CI validation step"`

## Closure Status (2026-05-03)

- #838 closed — implemented in `tests/unit/SwaggerIntegrationTests.cs` (`OpenApiJsonEndpoint_Returns200_WithValidDocumentShape`)
- #839 closed — implemented in `tests/unit/SwaggerIntegrationTests.cs` (`OpenApiJsonEndpoint_ReturnsCorsHeader_ForDocsOrigin`)
- #840 closed — implemented in `tests/e2e/Contracts/ApiDocsFrontendSmokeContractTests.cs`
