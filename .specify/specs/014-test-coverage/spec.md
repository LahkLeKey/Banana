# Feature Specification: Tests & Coverage (v2)

**Feature Branch**: `014-test-coverage`  
**Created**: 2026-04-25  
**Status**: Draft  
**Parent**: [`005-v2-regeneration`](../005-v2-regeneration/spec.md)  
**Baseline**: [`legacy-baseline/test-coverage.md`](../../legacy-baseline/test-coverage.md)

## Summary

Regenerate the test layout with clear unit/integration/e2e/native boundaries,
fail-loud DAL gating, retention-bounded coverage artifacts, and a stable
coverage anchor (`coverage-denominator.json`).

## User Scenarios & Testing

### User Story 1 — Loud DAL gating (Priority: P1)

As a CI run, native DAL tests fail with a typed config-missing message when
`BANANA_PG_CONNECTION` is unset, instead of silently skipping.

### User Story 2 — Stable coverage anchor (Priority: P1)

As a maintainer, `coverage-denominator.json` location and format remain
stable across v1 → v2; coverage thresholds don't drift.

### User Story 3 — Bounded artifact retention (Priority: P2)

As CI, `artifacts/coverage-*.cobertura.xml` retention is bounded; old runs
are pruned by an explicit policy.

### User Story 4 — Clear unit/integration/e2e boundary (Priority: P2)

As a test author, I know which folder a new test belongs in by reading a one-page taxonomy.

### Edge Cases

- An interop interface gains a method → all fakes update in one helper script (avoid CS0535 cascade).
- A test relies on a deleted module → fail loudly during build, not at runtime.

## Requirements

### Functional Requirements

- **FR-001**: Native DAL tests MUST fail (not skip) when `BANANA_PG_CONNECTION` is unset, unless explicitly marked gated.
- **FR-002**: `coverage-denominator.json` location and format MUST remain stable.
- **FR-003**: `scripts/run-tests-with-coverage.sh` and `scripts/run-native-c-tests-with-coverage.sh` script names MUST remain stable.
- **FR-004**: Cobertura XML MUST remain the coverage interchange format.
- **FR-005**: A documented test taxonomy MUST exist (this spec or `contracts/`).
- **FR-006**: Adding a method to the interop seam MUST update fakes via a single helper or codegen, not 10+ manual edits (coordinated with `007`).
- **FR-007**: `artifacts/` retention MUST be bounded by an explicit policy.

### Hard contracts to preserve

- `coverage-denominator.json` path + format.
- Coverage script names.
- Cobertura format.
- Native test runner integration with `006`.

### Pain points addressed

- Silent DAL skip → typed failure (FR-001, coordinated with `006`).
- CS0535 cascade → centralized fakes (FR-006, coordinated with `007`).
- Unbounded artifacts → retention policy (FR-007).
- Unclear unit vs integration vs e2e → taxonomy (FR-005).

## Success Criteria

- **SC-001**: A test taxonomy doc exists and is linked from this slice's quickstart.
- **SC-002**: Native DAL tests return typed failure on unset PG conn.
- **SC-003**: `coverage-denominator.json` unchanged in path/format.
- **SC-004**: Adding an interop method touches ≤2 test files (`007` SC-003).
- **SC-005**: `artifacts/` size has a documented upper bound or pruning rule.
