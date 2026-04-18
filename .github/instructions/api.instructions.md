---
name: ASP.NET Pipeline Standards
description: Guidance for Banana ASP.NET controllers, services, pipeline steps, middleware, and interop code.
applyTo: "src/c-sharp/asp.net/**/*.cs"
---

# ASP.NET Pipeline Standards

- Preserve the ordered pipeline model centered on `PipelineExecutor<PipelineContext>` and `IPipelineStep<PipelineContext>`.
- Register new pipeline steps in `Program.cs`, assign deterministic `Order` values, and add tests for ordering or branching behavior.
- Keep HTTP concerns in controllers, orchestration in services/pipeline steps, and native boundary logic in `NativeInterop`.
- Route error translation through `ErrorHandlingMiddleware` rather than ad-hoc controller handling.
- Prefer DI + options binding patterns already used by `DbAccessOptions` instead of static configuration reads.
