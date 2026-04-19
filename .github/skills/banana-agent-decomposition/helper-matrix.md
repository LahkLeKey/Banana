# Banana Helper Matrix

## Native Helpers

- `native-core-agent`: `src/native/core`, core models, algorithms, execution context, and focused native tests
- `native-dal-agent`: `src/native/core/dal`, PostgreSQL-gated native data access, payload shaping, and `BANANA_PG_CONNECTION` behavior
- `native-wrapper-agent`: `src/native/wrapper`, ABI exports, status codes, memory ownership, and managed interop coordination

## ASP.NET Helpers

- `api-pipeline-agent`: controllers, services, middleware, DI wiring, pipeline executor, and ordered steps in `src/c-sharp/asp.net`
- `api-interop-agent`: `NativeInterop`, `DataAccess`, managed status translation, marshalling, and native contract synchronization

## Frontend Helpers

- `react-ui-agent`: `src/typescript/react/src`, UI state, forms, layouts, components, and styles
- `electron-agent`: `src/typescript/electron`, desktop runtime, preload, and native bridge code

## Runtime And Validation Helpers

- `compose-runtime-agent`: `docker-compose.yml`, `docker`, runtime scripts, local stack bring-up, and health-check behavior
- `workflow-agent`: `.github/workflows`, coverage automation, CI artifact handling, and job structure
- `test-triage-agent`: test failure isolation, harness fixes, validation expansion, and fix-owner routing

## Parent Agents

- `native-c-agent`: coordinate when core, DAL, and wrapper all move together
- `csharp-api-agent`: coordinate when pipeline and interop or data access move together
- `react-agent`: coordinate React plus Electron or broader frontend work
- `infrastructure-agent`: coordinate compose/runtime and workflow changes together
- `integration-agent`: validate across helpers and separate environment failures from product defects
- `banana-sdlc`: orchestrate multi-domain, multi-phase work
## Shared Frontend Contract

- If a task touches src/typescript/react, src/typescript/electron, or src/typescript/shared/ui, keep shared primitives in @banana/ui instead of app-local thin re-export stubs.
- Reuse @banana/ui/tailwind/preset and @banana/ui/styles/tokens.css from consuming apps.
- Install dependencies in src/typescript/shared/ui before running app-level bun check/build flows.
- Reference .github/shared-typescript-ui.md for the full contract.
