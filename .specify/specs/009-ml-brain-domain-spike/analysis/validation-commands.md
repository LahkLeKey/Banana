# Validation Commands: ML Brain Domain SPIKE

**Date**: 2026-04-26
**Purpose**: Reference lane commands for post-SPIKE follow-up implementation validation.

## SPIKE Validation (Artifact Review)

SPIKE artifacts are review-gated, not runtime-tested. Validation in this SPIKE means:
1. All required analysis files exist.
2. Domain definitions are non-overlapping.
3. Matrix covers all families across all shared dimensions.
4. Readiness packets are present for all three domains.

## Post-SPIKE Follow-Up Lane Commands

These commands become valid once follow-up implementation slices execute.

### Native Lane

```bash
# Prerequisite: BANANA_PG_CONNECTION env var (if DAL-adjacent)
cd /c/Github/Banana

# Configure and build
cmake --preset default
cmake --build --preset default-build

# Run native tests
ctest --preset default-test --output-on-failure
```

### .NET Unit Lane

```bash
cd /c/Github/Banana
dotnet test tests/unit --configuration Debug
```

### .NET Integration Lane (requires compose stack)

```bash
cd /c/Github/Banana
dotnet test tests/integration
```

### Coverage Denominator Guard

```bash
cd /c/Github/Banana
bash scripts/check-banana-api-coverage-denominator.sh
```

### Full Test Suite (all lanes)

```bash
cd /c/Github/Banana
bash scripts/run-tests-with-coverage.sh
```

## SPIKE Artifact Existence Gate

```bash
# Check Phase 1 setup files
ls .specify/specs/009-ml-brain-domain-spike/analysis/

# Check Phase 2 foundational files
ls .specify/specs/009-ml-brain-domain-spike/analysis/terminology-rules.md
ls .specify/specs/009-ml-brain-domain-spike/analysis/comparison-dimensions.md
ls .specify/specs/009-ml-brain-domain-spike/analysis/confidence-policy.md

# Check US1 domain definitions
ls .specify/specs/009-ml-brain-domain-spike/analysis/brain-domain-definitions.md
ls .specify/specs/009-ml-brain-domain-spike/analysis/model-family-mappings.md

# Check US2 matrix
ls .specify/specs/009-ml-brain-domain-spike/analysis/model-family-tradeoff-matrix.md
ls .specify/specs/009-ml-brain-domain-spike/analysis/domain-fit-scorecards.md

# Check US3 readiness packets
ls .specify/specs/009-ml-brain-domain-spike/analysis/readiness-packet-left-brain.md
ls .specify/specs/009-ml-brain-domain-spike/analysis/readiness-packet-right-brain.md
ls .specify/specs/009-ml-brain-domain-spike/analysis/readiness-packet-full-brain.md
```

## Environment Contract Notes

- `BANANA_PG_CONNECTION` — Required only when follow-up slices exercise DAL paths.
- `BANANA_NATIVE_PATH` — Required by ASP.NET for native lib resolution.
- SPIKE itself does not require either variable (documentation-only).
