# Banana

Pass the Banana from native C into API, desktop, web, and mobile channels.

This repo is a practical playground for one core idea: keep domain behavior in native C, then project it cleanly into multiple runtimes without rewriting business logic per app.

## Learn More

- Wiki hub: [Banana Wiki](https://github.com/LahkLeKey/Banana/wiki)
- Onboarding and architecture: `docs/developer-onboarding.md`
- Build/test command index: `.wiki/Build-Run-Test-Commands.md`

> Banana is a prototype for Poly: a polymorphic, multi-platform runtime approach where core behavior starts in C and can be projected into API, desktop, frontend, and mobile delivery channels.

## Coverage 80 One-Pass Triage Checklist

Use this checklist when a merge-gated coverage run fails:

1. Open the coverage aggregate artifact and identify failing tuples first.
2. Inspect each lane summary for status, failure class, measured percent, threshold percent, and deficit percent.
3. Confirm denominator policy snapshot version matches all evaluated lanes.
4. Confirm evidence bundle paths are workspace-relative and present in artifacts.
5. Verify whether active exceptions are listed and whether any exception has expired.
6. For integration tuple failures, confirm `BANANA_PG_CONNECTION` and `BANANA_NATIVE_PATH` contracts were satisfied.
