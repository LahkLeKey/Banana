# Tasks: ASP.NET Startup and Pipeline Restabilization

> All tasks delivered as part of the sprint4 CI stabilization wave (PR #796).

## T-01: Remove duplicate startup declarations

- [x] Identify and remove duplicate `FrontendCorsPolicy` constant declaration causing `CS0128`
- [x] Normalize Program.cs middleware registration order (auth → rate limiting → CORS → pipeline steps)
- [x] Verify `dotnet build src/c-sharp/asp.net/Banana.Api.csproj` succeeds with no compile errors

## T-02: Validate pipeline step wiring

- [x] Confirm drift-detection and A/B pipeline steps added during bulk delivery compile cleanly
- [x] Validate pipeline step ordering is consistent with `IPipelineStep<PipelineContext>` contract
- [x] Verify no duplicate constants or conflicting startup declarations remain in Program startup

## T-03: CI compile guard

- [x] Confirm `dotnet build` step in Compose CI produces exit 0 on the stabilized codebase
- [x] Pipeline step ordering validated by existing unit tests (no new regressions)

## T-04: Parity close

- [x] `dotnet build src/c-sharp/asp.net/Banana.Api.csproj` runs clean in CI and locally
- [x] Spec tasks parity validator evaluates this spec (tasks.md present)
