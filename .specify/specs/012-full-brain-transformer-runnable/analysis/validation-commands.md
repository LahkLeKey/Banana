# Validation commands -- 012 Full Brain Transformer

All commands run from repo root `c:/Github/Banana`.

## Configure + build

```
cmake --preset default
cmake --build --preset default
```

## Native lane (ctest)

```
ctest --preset default --output-on-failure
```

Expected: 7/7 tests pass after this slice lands (was 6/6 after 011).

## Targeted ad-hoc probe

```
PATH="$PWD/build/cmake-tools:$PATH" /tmp/probe_transformer.exe
```
(Probe binary linked against `build/cmake-tools/libbanana_native` -- not committed.)
