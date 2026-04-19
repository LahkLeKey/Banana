---
name: ASP.NET Pipeline Standards
description: Guidance for Banana ASP.NET controllers, services, pipeline steps, middleware, and interop code.
applyTo: "src/c-sharp/asp.net/**/*.cs"
---

# ASP.NET Pipeline Standards

- Use `api-pipeline-agent` for controllers, services, middleware, `Program.cs`, and ordered pipeline work.
- Use `api-interop-agent` for `NativeInterop`, `DataAccess`, status translation, and managed/native contract synchronization.
- Use `csharp-api-agent` only when both helper-owned surfaces need coordination.
- Preserve the ordered pipeline model centered on `PipelineExecutor<PipelineContext>` and `IPipelineStep<PipelineContext>`.
- Register new pipeline steps in `Program.cs`, assign deterministic `Order` values, and add tests for ordering or branching behavior.
- Keep HTTP concerns in controllers, orchestration in services/pipeline steps, and native boundary logic in `NativeInterop`.
- Route error translation through `ErrorHandlingMiddleware` rather than ad-hoc controller handling.
- Prefer DI + options binding patterns already used by `DbAccessOptions` instead of static configuration reads.

## Shared Frontend Contract

- If a task touches src/typescript/react, src/typescript/electron, or src/typescript/shared/ui, keep shared primitives in @banana/ui instead of app-local thin re-export stubs.
- Reuse @banana/ui/tailwind/preset and @banana/ui/styles/tokens.css from consuming apps.
- Install dependencies in src/typescript/shared/ui before running app-level bun check/build flows.
- Reference .github/shared-typescript-ui.md for the full contract.
