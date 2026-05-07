# In-Scope Files: CMake Preset Activation Fix

**Date**: 2026-04-26
**Purpose**: Enumerate configuration surfaces in scope for the 008 bug fix.

## CMake Project Files

| File | Purpose | Fix Domain |
|---|---|---|
| `CMakePresets.json` | Canonical configure/build/test preset definitions | T011: validate preset catalog |
| `CMakeLists.txt` | Root project definition | Read-only reference |
| `tests/native/CMakeLists.txt` | Native test project definition | Read-only reference |

## VS Code Settings

| File | Purpose | Fix Domain |
|---|---|---|
| `.vscode/settings.json` | Root workspace folder CMake Tools settings | T008: align activation settings |
| `tests/.vscode/settings.json` | Tests folder CMake Tools settings and source binding | T009: align binding settings |
| `Banana.code-workspace` | Workspace-level defaults shared across all folders | T010: align workspace defaults |

## Workspace Structure (for context)

| Folder Name | Path | Has CMakeLists.txt |
|---|---|---|
| `(Monorepo) Banana` | `.` | ✅ Yes (root CMakeLists.txt) |
| `(Monorepo) Tests` | `tests` | ✅ Yes (tests/native/CMakeLists.txt) |
| `(C) Native` | `src/native` | ❌ No |
| `(C#) Api` | `src/c-sharp/asp.net` | ❌ No |
| `(TS) Api` | `src/typescript/api` | ❌ No |
| All other TS folders | `src/typescript/...` | ❌ No |
| `(Monorepo) Docker` | `docker` | ❌ No |
| `(Monorepo) Scripts` | `scripts` | ❌ No |
| `(Specify) Specs` | `.specify/specs` | ❌ No |

## Out of Scope

- Native C source files (`src/native/**/*.c`)
- ASP.NET pipeline, React, Electron code
- CI workflow files (not changed in this slice)
- CMakePresets.json preset logic or build targets (content only; existence/name validated)
