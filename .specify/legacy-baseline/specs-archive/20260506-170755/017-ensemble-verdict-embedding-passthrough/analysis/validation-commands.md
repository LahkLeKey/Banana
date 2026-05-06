# Validation commands -- 017 Ensemble Verdict Embedding Passthrough

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
bash scripts/check-dotnet-coverage-threshold.sh \
  .artifacts/coverage/unit-report/Summary.txt \
  80 \
  .artifacts/coverage/unit-report/unit-line-rate-gate.json \
  unit-line-rate
```

## Trigger flip (close-out only)

After 017 merges, edit
[../../013-ml-brain-composition-spike/analysis/deferred-registry.md](../../013-ml-brain-composition-spike/analysis/deferred-registry.md)
to mark U-001's trigger as FIRED -- this unlocks slice 016.
