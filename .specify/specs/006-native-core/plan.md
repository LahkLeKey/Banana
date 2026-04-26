# Implementation Plan: Native Core (v2)

**Branch**: `006-native-core` | **Date**: 2026-04-25 | **Spec**: [spec.md](./spec.md)

## Summary

Lean v2 native layer with a stable wrapper ABI, consolidated core domain
modules, unified error reporting, and explicit DAL gating.

## Technical Context

**Language/Version**: C11.  
**Primary Dependencies**: CMake 3.20+, libpq (DAL), platform C runtime.  
**Storage**: PostgreSQL via `BANANA_PG_CONNECTION`.  
**Testing**: C unit tests in `tests/native/`, executed by `scripts/run-native-c-tests-with-coverage.sh`.  
**Target Platform**: Windows x64 (Visual Studio 17 2022 Release), Linux containers (gcc).  
**Project Type**: Native shared library + tests.  
**Performance Goals**: No regression on hot paths (lifecycle, ripeness, classify).  
**Constraints**: Preserve `NativeStatusCode` numeric values and public symbol names.  
**Scale/Scope**: Target ≤15 core modules (down from ~25).

## Constitution Check

- Hard contracts preserved: PASS (FR-001, FR-002, FR-006, FR-007).
- Helper-agent decomposition: PASS (`native-core-agent` / `native-wrapper-agent` / `native-dal-agent`).

## Project Structure

```text
src/native/
├── core/
│   ├── domain/        # consolidated modules (≤15)
│   └── dal/           # banana_db.{c,h} + minimal internals
├── wrapper/
│   ├── banana_wrapper.{c,h}
│   └── banana_wrapper_internal.h
└── testing/           # native test helpers

tests/native/          # one file per wrapper export + DAL gating tests
CMakeLists.txt         # target: banana_wrapper
```

**Structure Decision**: Keep `core/`, `wrapper/`, `testing/` split; consolidate
within `core/domain/`. Co-locate DAL under `core/dal/` (matches v1 path used
by `BANANA_PG_CONNECTION` consumers).

## Phasing

- **Phase 0**: Inventory consumers of every public symbol; mark unused as drop candidates.
- **Phase 1**: Land v2 wrapper alongside v1 with identical ABI.
- **Phase 2**: Consolidate core domain modules; managed consumer untouched.
- **Phase 3**: Unify error reporting; introduce `BANANA_PG_CONNECTION` typed failure.
- **Phase 4**: Drop legacy modules; bump CMake to v2 only.

## Complexity Tracking

| Violation | Why Needed | Simpler Alternative Rejected Because |
|-----------|------------|--------------------------------------|
| Side-by-side v1/v2 | Cutover safety for `007` | Big-bang swap rejected: regression risk on numeric ABI |
