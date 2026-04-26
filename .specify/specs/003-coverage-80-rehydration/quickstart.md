# Quickstart: Coverage 80 Rehydration

This guide validates behavior required by spec `003-coverage-80-rehydration`.

## 1. Prerequisites

- Docker Engine with Docker Compose v2
- Bash shell at repository root
- .NET 8 SDK
- Bun 1.x and repository TypeScript dependencies
- Native build toolchain for merge-gated native coverage surfaces

## 2. Validate baseline scripts and preflight behavior

Run coverage entry points from repository root:

```bash
bash scripts/run-native-c-tests-with-coverage.sh
```

```bash
bash scripts/run-tests-with-coverage.sh
```

Expected:

- Coverage artifacts are emitted to lane-scoped artifact roots.
- No machine-specific absolute paths are required to interpret outputs.

## 3. Validate compose runtime channels for e2e coverage surfaces

```bash
bash scripts/compose-apps.sh
curl -fsS http://localhost:8080/health
bash scripts/compose-apps-down.sh
```

Expected:

- Runtime channels start and stop deterministically.
- Health checks classify runtime dependency failures as preflight/runtime contract failures.

## 4. Validate lane normalization and aggregate summary behavior

Execute merge-gated coverage workflow run:

```bash
gh workflow run "Compose CI"
```

After completion, validate:

- Every evaluated lane publishes normalized lane result records.
- Failure classes are one of: threshold, coverage contract, preflight contract, flake contract.
- Aggregate summary includes all applicable `(domain, layer)` tuples.

## 5. Validate denominator policy and not-applicable semantics

Inspect coverage evidence bundles and confirm:

- One denominator policy version used across all lanes in the run.
- Exclusion categories are consistent.
- Non-applicable tuples are explicit with rationale.

## 6. Validate exception lifecycle behavior

For a temporary exception record:

- verify complete metadata (owner, rationale, remediation plan, expiry)
- verify aggregate and lane summaries surface active exceptions
- verify expired exceptions do not suppress failures

## 7. Validate determinism window

Repeat selected baseline run window and compare terminal outcomes:

```bash
gh run list --workflow "Compose CI" --limit 10
```

Expected:

- Stable tuple outcomes for identical commit inputs, excluding declared flake-contract cases.

## 8. Evidence checklist

Capture these outputs in closure validation:

- Lane-level normalized coverage results
- Aggregate summary artifact
- Denominator policy snapshot references
- Exception snapshot references (if used)
- Failure taxonomy counts by class
- Verification window notes for repeat-run consistency
