# Feature Specification: Cross-Platform Native Build Matrix (linux/mac/win, x86_64/arm64)

**Feature Branch**: `074-native-cross-platform-build-matrix`
**Created**: 2026-05-01
**Status**: Stub (scaffolded by 067 cross-domain follow-up planning)
**Wave**: third
**Domain**: native / infra
**Depends on**: #072

## Problem Statement

Native binaries only ship for linux/x86_64. Apple Silicon developers and any future arm64 server target are blind.

## In Scope *(mandatory)*

- Extend CMake presets and CI matrix to cover linux/x86_64, linux/arm64, macos/arm64, windows/x86_64.
- Cross-compile via clang or use github-hosted runners per arch as available.
- Publish artifacts under `out/native/<triple>/` and update consumers (compose, dotnet) to pick the right triple.
- Document download/install surface in the wiki.

## Out of Scope *(mandatory)*

- FreeBSD / illumos builds.
- Static linking decisions (preserve current dynamic-link contract).

## Notes for the planner

- Run `speckit.specify` against this stub to expand User Scenarios, Functional Requirements, and Success Criteria before `speckit.plan`.
- Cross-layer dependencies (API or native changes) MUST be enumerated in the plan artifact and parity-governed (feature 047) when applicable.
- This stub was generated as part of the cross-domain planning batch documented in [`.specify/specs/067-cross-domain-followup-planning/spec.md`](../067-cross-domain-followup-planning/spec.md).

## Status: Implemented

Implemented on branch `sprint3-074-084-086` (May 2026).

- `CMakePresets.json`: added `linux-x86_64-release`, `linux-arm64-release`, `macos-arm64-release`, `windows-x86_64-release` configure/build presets with `BANANA_TRIPLE` and `installDir` per-triple.
- `.github/workflows/native-cross-platform.yml`: CI matrix across all four triples; ARM64 cross-compile via `gcc-aarch64-linux-gnu`.
- Artifacts published under `out/native/<triple>/`.
