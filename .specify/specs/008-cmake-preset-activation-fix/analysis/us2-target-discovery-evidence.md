# US2 Target Discovery Evidence: CMake Preset Activation Fix

**Date**: 2026-04-26  
**Command**: `cmake --build --preset default --target help`

## Output

```
test: phony
edit_cache: phony
rebuild_cache: phony
tests/native/test: phony
tests/native/edit_cache: phony
tests/native/rebuild_cache: phony
banana_native: phony
banana_test_calc: phony
banana_test_calc.exe: phony
banana_test_dal_contracts: phony
banana_test_dal_contracts.exe: phony
banana_test_ml_models: phony
banana_test_ml_models.exe: phony
banana_test_operational_domains: phony
banana_test_operational_domains.exe: phony
all: phony
build.ninja: RERUN_CMAKE
clean: CLEAN
help: HELP
```

## Verification

| Expected Target | Discovered? |
|---|---|
| `banana_native` | ✅ Yes |
| `banana_test_calc` | ✅ Yes |
| `banana_test_ml_models` | ✅ Yes |
| `banana_test_operational_domains` | ✅ Yes |
| `banana_test_dal_contracts` | ✅ Yes |

**Result**: ✅ PASS — all expected native targets are discoverable.
