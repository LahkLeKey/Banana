---
name: Testing And Coverage Standards
description: Guidance for Banana unit, integration, native, and coverage workflows.
applyTo: "{tests/**/*.cs,tests/**/*.c,scripts/run-tests-with-coverage.sh,scripts/run-native-c-tests-with-coverage.sh,.github/workflows/**/*.yml}"
---

# Testing And Coverage Standards

- Use `test-triage-agent` to isolate failures, repair harness issues, and route fixes to the right helper agent.
- Use `integration-agent` when validation must cross helpers, domains, runtime scripts, or coverage flows.
- Prefer the existing test projects and scripts over one-off commands: native tests in `tests/native`, unit tests in `tests/unit`, integration tests in `tests/integration`.
- Keep integration assumptions explicit: `BANANA_NATIVE_PATH` and `BANANA_PG_CONNECTION` must be set when native-backed integration tests run.
- When a change touches more than one layer, favor the repo’s aggregated test path in `scripts/run-tests-with-coverage.sh`.
- Coverage artifacts should continue to land in the existing `build`, `.artifacts`, or `artifacts` locations rather than new output folders.
- Do not lower or bypass the native coverage gate without an explicit user request.
