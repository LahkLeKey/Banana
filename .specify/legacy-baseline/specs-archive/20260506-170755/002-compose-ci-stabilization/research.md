# Research: Compose CI Stabilization

**Date**: 2026-04-25
**Spec**: `.specify/specs/002-compose-ci-stabilization/spec.md`

## Decision 1: Enforce deterministic terminal states per compose lane

- **Decision**: Represent each merge-gated compose lane (`compose-tests`, `compose-runtime`, `compose-electron`, `e2e-smoke`) as an explicit pass/fail/skip contract with lane-scoped stage attribution.
- **Rationale**: Current failures include ambiguous nonzero exits (for example shell exit 126) and mixed cleanup behavior that obscures where the lane failed.
- **Alternatives considered**:
  - Keep current inline `docker compose` invocations without lane-level stage contract: rejected because triage still depends on log spelunking.
  - Collapse lanes into one composite job: rejected because failure isolation and merge-gate signal quality degrade.

## Decision 2: Always publish diagnostics bundles with stable paths

- **Decision**: Capture diagnostics with `if: always()` and publish lane bundles even when failures happen before test execution.
- **Rationale**: Missing or partial evidence currently forces reruns. Stable bundles support one-pass triage.
- **Alternatives considered**:
  - Publish diagnostics only on failure: rejected because skip and early abort states still need evidence.
  - Depend only on raw workflow logs: rejected because logs are not lane-scoped artifacts and are harder to diff.

## Decision 3: Introduce artifact publication manifests and fallback placeholders

- **Decision**: Define expected artifact outputs per lane and create deterministic placeholders or skip markers when optional surfaces are absent.
- **Rationale**: Upload steps fail with missing-path errors when optional projects or generated files are absent.
- **Alternatives considered**:
  - Allow upload steps to warn/fail on missing files: rejected because this creates false negatives and non-deterministic outcomes.
  - Disable artifact upload for optional surfaces entirely: rejected because release triage loses evidence.

## Decision 4: Encode optional surfaces as explicit skip semantics

- **Decision**: For optional projects (for example integration and e2e project presence checks), emit explicit skip artifacts (`*.skipped.txt` or JSON gate records) instead of silent no-op behavior.
- **Rationale**: Skip intent must be machine-readable and human-readable to prevent ambiguity in merge-gated reviews.
- **Alternatives considered**:
  - Keep boolean output checks only in step conditions: rejected because reviewers cannot inspect skip reasons post-run.
  - Treat missing optional projects as failures: rejected because repository contracts explicitly allow optional surfaces.

## Decision 5: Add preflight checks for shell/permission contract violations

- **Decision**: Add lane preflight checks that detect execute-bit issues, script readability, and key compose/script availability before lane execution.
- **Rationale**: Exit code 126 and similar failures are often setup/permission issues; surfacing these as explicit diagnostics reduces triage time.
- **Alternatives considered**:
  - Rely on compose command failures alone: rejected because root cause remains opaque.
  - Move all script validation into container images: rejected because CI lane failures also occur before image startup.

## Decision 6: Remove Node runtime deprecation drift from merge-gated workflows

- **Decision**: Upgrade GitHub Action references in merge-gated workflows to Node runtime-compatible versions where available; if an upgrade is blocked, track a bounded exception with owner and target date.
- **Rationale**: Runtime deprecation warnings are a forward-compatibility risk that can flip merge-safe workflows into hard failures when runner defaults change.
- **Alternatives considered**:
  - Ignore warnings until forced cutoff: rejected because this introduces avoidable release risk.
  - Pin older action versions permanently: rejected because it preserves known deprecation debt.

## Decision 7: Keep implementation scoped to delivery/runtime domain

- **Decision**: Modify workflow and orchestration surfaces only (`.github/workflows`, `scripts`, `docker-compose.yml`, and runtime docs/wiki sync).
- **Rationale**: The feature addresses CI determinism and evidence quality, not API/native business logic.
- **Alternatives considered**:
  - Refactor API/native behavior to reduce compose test pressure: rejected as out of scope for CI stabilization.
