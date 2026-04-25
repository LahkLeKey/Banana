# Quickstart — Native Core (v2)

## Prerequisites

- Windows + Visual Studio 17 2022 (Build Tools) or Linux + gcc.
- CMake 3.20+.
- For DAL paths: `BANANA_PG_CONNECTION` set to a reachable Postgres.

## Build

```bash
# from repo root
rm -f build/native/CMakeCache.txt && rm -rf build/native/CMakeFiles
cmake -S . -B build/native -G "Visual Studio 17 2022" -A x64
cmake --build build/native --config Release
```

VS Code task equivalent: **Build Native Library**.

## Test

```bash
bash scripts/run-native-c-tests-with-coverage.sh
```

## Verify ABI parity (managed side)

```bash
dotnet build src/c-sharp/asp.net/Banana.Api.csproj -c Debug
dotnet test tests/integration  # or unit, depending on cutover phase
```

## Scope guardrails

- Do not touch `src/c-sharp/`, `src/typescript/`, `docker/`, or `scripts/` here. Those belong to other slices.
- Do not change numeric `NativeStatusCode` values.
- Do not silently skip DAL tests when `BANANA_PG_CONNECTION` is unset — return typed failure.
