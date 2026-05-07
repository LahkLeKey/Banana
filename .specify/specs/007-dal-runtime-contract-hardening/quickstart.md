# Quickstart: DAL Runtime Contract Hardening

## Purpose

Execute the smallest useful workflow to implement and validate deterministic DAL runtime outcome behavior.

## Prerequisites

- Repository root: `C:/Github/Banana`
- Active feature pointer set to `.specify/specs/007-dal-runtime-contract-hardening`
- Native library and ASP.NET runtime wiring available for local validation

## 1. Confirm active feature

```bash
cd /c/Github/Banana
cat .specify/feature.json
```

Expected `feature_directory`:

- `.specify/specs/007-dal-runtime-contract-hardening`

## 2. Review in-scope source surfaces

```bash
cd /c/Github/Banana
find src/native/core/dal src/native/core -maxdepth 2 -type f | sort
find src/native/wrapper src/c-sharp/asp.net/Pipeline src/c-sharp/asp.net/NativeInterop -type f | sort
```

## 3. Implement bounded DAL hardening updates

Focus on:

- deterministic non-success outcomes for unconfigured/dependency-unavailable/query-failure states
- preserved success-path compatibility
- native-to-managed mapping consistency

## 4. Run required validation lanes

```bash
cd /c/Github/Banana
# Native
scripts/run-native-c-tests-with-coverage.sh

# Targeted unit
dotnet test tests/unit --filter "FullyQualifiedName~StatusMapping|FullyQualifiedName~NativeBananaClient"

# Contract lane
dotnet test tests/e2e --filter "FullyQualifiedName~Contracts"
```

## 5. Review completion evidence

Confirm outcomes are deterministic and that no synthetic DAL success is emitted in in-scope non-success cases.

## 6. Exact commands used in current execution

```bash
cd /c/Github/Banana

# Targeted managed lanes
dotnet test tests/unit --filter "FullyQualifiedName~StatusMappingTests|FullyQualifiedName~NativeBananaClientTests"
dotnet test tests/e2e --filter "FullyQualifiedName~Contracts.E2eRunnerContractTests"

# CMake Tools status checks attempted for native lane
# Build_CMakeTools -> "Build failed: Unable to configure the project"
# RunCtest_CMakeTools -> "Build failed: Unable to configure the project"
```

Native-lane blocker details are documented in `analysis/native-validation-blocker.md`.

## Expected outcomes

- DAL failure semantics are explicit and deterministic.
- ASP.NET mapping remains aligned with hardened native outcomes.
- Required validation lanes pass for the bounded slice.
