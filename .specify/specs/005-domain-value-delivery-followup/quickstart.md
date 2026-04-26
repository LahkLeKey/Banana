# Quickstart: Domain Value Delivery Follow-Up (Phase 2)

## Prerequisites

- Repository root: `C:/Github/Banana`
- .NET SDK available
- Bun available for TypeScript API tests
- `BANANA_NATIVE_PATH` set to valid library for success-path checks

## 1. Build and run ASP.NET API locally

```bash
cd /c/Github/Banana
BANANA_NATIVE_PATH=/workspace/out/native/bin/libbanana_native.so dotnet run --project src/c-sharp/asp.net/Banana.Api.csproj
```

## 2. Validate native unavailable contract (503)

Start API with invalid native path and call ripeness:

```bash
curl -s -X POST http://localhost:5000/ripeness/predict \
  -H "Content-Type: application/json" \
  -d '{"inputJson":"yellow banana"}'
```

Expect status `503` and payload with `error=native_unavailable`.

## 3. Validate TS API ripeness route

```bash
cd /c/Github/Banana/src/typescript/api
bun test src/routes/ripeness.test.ts
```

## 4. Run ASP.NET unit tests for contract mapping

```bash
cd /c/Github/Banana
dotnet test tests/unit
```

## 5. Run integration-equivalent lane (e2e contracts)

```bash
cd /c/Github/Banana
dotnet test tests/e2e --filter "FullyQualifiedName~Contracts"
```

## 6. Run full feature validation set

```bash
cd /c/Github/Banana
dotnet test tests/unit && dotnet test tests/e2e && bun run --cwd src/typescript/api test
```

## Expected outcomes

- Native runtime failures produce deterministic 503 payloads with `BANANA_NATIVE_PATH` remediation.
- ML/not-banana controllers return typed payload contracts via mapping abstractions.
- Fastify exposes and validates `/ripeness/predict` contract parity.
- Harvest/truck HTTP surfaces are reachable and status-mapped consistently.
- Integration-equivalent contract lane is reproducible via `tests/e2e` command.