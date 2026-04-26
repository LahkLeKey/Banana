# Quickstart — ASP.NET Pipeline (v2)

## Prerequisites

- .NET 8 SDK.
- Built native library (`006-native-core` quickstart).
- `BANANA_NATIVE_PATH` set if the native artifact is not in default search path.

## Build

```bash
dotnet build src/c-sharp/asp.net/Banana.Api.csproj -c Debug
```

VS Code task equivalent: **Build Banana API** (depends on **Build Native Library**).

## Run locally

```bash
dotnet run --project src/c-sharp/asp.net/Banana.Api.csproj
```

## Test

```bash
bash scripts/run-tests-with-coverage.sh
```

## Verify HTTP surface stability

After cutover:

```bash
# Spot-check stable routes versus baseline snapshot in research.md
curl -sS http://localhost:5000/health
curl -sS http://localhost:5000/api/banana/...
```

## Scope guardrails

- Do not modify native code here — that belongs to `006`.
- Do not change frontend HTTP clients here — that belongs to `009/010/011`.
- Keep `coverage-denominator.json` path + format.
