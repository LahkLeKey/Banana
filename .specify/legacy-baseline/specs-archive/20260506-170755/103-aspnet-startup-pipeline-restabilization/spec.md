# Feature Specification: ASP.NET Startup and Pipeline Restabilization

**Feature Branch**: `103-aspnet-startup-pipeline-restabilization`
**Created**: 2026-05-01
**Status**: Follow-up stabilization stub
**Wave**: stabilization
**Domain**: api / c-sharp
**Depends on**: #054, #068, #069, #078, #080

## Problem Statement

Post-bulk merges introduced ASP.NET compile instability in startup wiring. The current build fails with `CS0128` due to duplicate `FrontendCorsPolicy` declaration in Program startup, which blocks CI confidence and delivery reliability.

## In Scope *(mandatory)*

- Remove duplicate startup declarations and normalize Program startup ordering.
- Validate middleware registration order for auth, rate limiting, CORS, and pipeline steps.
- Verify new pipeline steps added during bulk delivery (drift and A/B steps) resolve cleanly.
- Add a compile-only guard lane that fails fast on startup wiring regressions.

## Out of Scope *(mandatory)*

- New endpoint features unrelated to startup reliability.
- Auth policy redesign beyond compile and startup integrity fixes.

## Success Criteria

- `dotnet build src/c-sharp/asp.net/Banana.Api.csproj` succeeds in CI and local runs.
- No duplicate constants or conflicting startup declarations remain in Program startup.
- Pipeline step ordering is documented and validated by tests.

## Notes for the planner

- Prioritize minimal, low-risk changes in startup composition.
- Add regression tests for startup invariants where practical.
- Treat this as a stability blocker for all downstream API follow-ups.
