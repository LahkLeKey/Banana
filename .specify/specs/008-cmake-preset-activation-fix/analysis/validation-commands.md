# Validation Commands: CMake Preset Activation Fix

**Date**: 2026-04-26  
**Purpose**: Reference commands for validating CMake preset activation and native lane execution.

## CLI Preset Validation (Reliable Path)

```bash
cd /c/Github/Banana

# Configure
cmake --preset default

# Build
cmake --build --preset default

# Run tests
ctest --preset default --output-on-failure
```

## Individual Test Targets

```bash
# Run a specific test by name
cd /c/Github/Banana
ctest --preset default --output-on-failure -R banana_test_dal_contracts
ctest --preset default --output-on-failure -R banana_test_calc
```

## List Available Targets

```bash
cd /c/Github/Banana
cmake --build --preset default --target help
```

## List Available CTest Tests

```bash
cd /c/Github/Banana
cmake --preset default
cd build/cmake-tools
ctest -N
```

## CMake Tools Extension Path (Tool-Integrated)

From VS Code with `(Monorepo) Banana` folder focused:
- **Configure**: `Ctrl+Shift+P` → `CMake: Configure`
- **Build**: `Ctrl+Shift+P` → `CMake: Build`
- **Run Tests**: `Ctrl+Shift+P` → `CMake: Run Tests`

Or via status bar CMake Tools controls.

## Verification Checks

```bash
# Check CMakePresets.json is valid JSON with required presets
cd /c/Github/Banana
python -c "import json; d=json.load(open('CMakePresets.json')); print('configurePresets:', [p['name'] for p in d['configurePresets']]); print('buildPresets:', [p['name'] for p in d['buildPresets']]); print('testPresets:', [p['name'] for p in d['testPresets']])"

# Check settings.json alignment
cat .vscode/settings.json
cat tests/.vscode/settings.json
```

## Build Output Location

With preset `default` and `binaryDir: "${sourceDir}/build/cmake-tools"`:
- Build artifacts: `/c/Github/Banana/build/cmake-tools/`
- Test binary: `/c/Github/Banana/build/cmake-tools/tests/native/`
