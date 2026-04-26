# Compose CI Stabilization

## Purpose

This page defines deterministic failure-evidence behavior for Compose CI merge-gated lanes.

## Lane Contract

- Each compose lane writes stage, reason-code, exit-code, and lane-result JSON.
- Each lane collects diagnostics (compose ps/logs + metadata files) regardless of pass/fail.
- Each lane produces an artifact manifest describing expected, optional, and produced paths.
- Optional paths use fallback placeholders to avoid missing-path upload failures.

## Permission Failure Contract

- Permission-related failures map to reason code permission or permission_contract_violation.
- Permission failures include remediation guidance in a remediation metadata file.
- Remediation metadata is included in diagnostics and lane artifact manifests as optional evidence.

## Non-Compose Merge-Gate Result Contract

- Merge-gated coverage/test jobs write deterministic job result records under .artifacts/compose-ci/jobs.
- Result records include status, stage, reason_code, and exit_code.
- Optional or missing test-project surfaces emit explicit skip semantics.

## Aggregate Summary Contract

- Workflow publishes a machine-readable failure aggregate summary artifact.
- Aggregate summary enumerates failed jobs with stage/reason attribution.
- Aggregate summary is also added to GitHub job summary output.

## Path-Safe Upload Contract

- Upload paths are normalized so artifact actions always resolve to real paths.
- Guard steps ensure expected directories/files exist before upload steps run.
- Missing-path warning scenarios are treated as contract violations.
