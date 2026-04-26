# Implementation Plan: Tests & Coverage (v2)

**Branch**: `014-test-coverage` | **Date**: 2026-04-25 | **Spec**: [spec.md](./spec.md)

## Technical Context

**Language/Version**: C# (xUnit), C (custom test runners), Bun/Node test runners.  
**Primary Dependencies**: Coverlet (Cobertura), native test scripts.  
**Constraints**: Stable coverage anchor; loud DAL gating.

## Project Structure

```text
tests/
├── unit/                  # in-process, no I/O, mocked seams
├── integration/           # cross-component within a process boundary; uses fakes for native
├── e2e/                   # full stack, real native + real APIs
└── native/                # native C tests including DAL gating

src/c-sharp/asp.net/coverage-denominator.json   # stable anchor

scripts/
├── run-tests-with-coverage.sh
└── run-native-c-tests-with-coverage.sh

artifacts/
└── coverage-*.cobertura.xml   # pruned per policy
```

## Phasing

- **Phase 0**: Document taxonomy in `contracts/test-taxonomy.md`.
- **Phase 1**: Make DAL gating fail loud (coordinated with `006`).
- **Phase 2**: Centralize interop fakes (coordinated with `007`).
- **Phase 3**: Add `artifacts/` retention policy + CI cleanup.
