# Validation commands -- 014 ASP.NET Ensemble Pipeline

All commands run from repo root `c:/Github/Banana`.

## Native lane (must stay 7/7 -- proves no native drift)

```
cmake --preset default
cmake --build --preset default
ctest --preset default --output-on-failure
```

## Managed

```
dotnet test src/c-sharp/asp.net/asp.net.sln \
  --collect:"XPlat Code Coverage"
```

## Coverage threshold

```
bash scripts/check-dotnet-coverage-threshold.sh
```
