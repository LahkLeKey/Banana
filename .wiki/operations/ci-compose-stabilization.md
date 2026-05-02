<!-- breadcrumb: Operations > Compose CI Stabilization -->

# Compose CI Stabilization

> [Home](../Home.md) › [Operations](README.md) › Compose CI Stabilization

Related pages: [CI and Compose Notes](ci-compose-notes.md), [CI Runtime Compatibility](ci-runtime-compatibility.md)

## What Changed

Compose CI now writes deterministic failure evidence for both compose lanes and non-compose merge-gated test/coverage jobs.

## Why This Matters

- Failing runs are easier to debug in one pass.
- Artifact uploads no longer depend on fragile path assumptions.
- Permission failures now include direct remediation notes.

## Where To Look In A Failed Run

- Compose lane artifacts:
  - `compose-tests-artifacts`
  - `compose-runtime-artifacts`
  - `compose-electron-artifacts`
- Non-compose job results:
  - artifacts under `.artifacts/compose-ci/jobs`
- Workflow aggregate:
  - `compose-ci-failure-aggregate`

## Fast Triage Steps

1. Open the lane/job result record and confirm status, stage, and reason_code.
2. If permission-related, read remediation metadata in diagnostics.
3. Review artifact manifest to confirm expected vs produced paths.
4. Open failure aggregate summary to see all failed jobs in one place.
