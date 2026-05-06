# Validation Commands

## Native Lane

```bash
cd /c/Github/Banana
scripts/run-native-c-tests-with-coverage.sh
```

## Unit Lane (Targeted)

```bash
cd /c/Github/Banana
dotnet test tests/unit --filter "FullyQualifiedName~StatusMapping|FullyQualifiedName~NativeBananaClient"
```

## Contract Lane

```bash
cd /c/Github/Banana
dotnet test tests/e2e --filter "FullyQualifiedName~Contracts"
```
