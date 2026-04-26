# Research: CMake Preset Activation Fix

**Branch**: `008-cmake-preset-activation-fix` | **Phase**: 0 | **Date**: 2026-04-26

## R-01: Source of truth for configure/build/test activation

**Question**: Which contract should be authoritative for CMake Tools preset activation?

**Decision**: Use root `CMakePresets.json` as the canonical source and align workspace settings to activate `default` configure/build/test presets.

**Rationale**: Presets provide deterministic configure/build/test contract and are compatible with both CMake Tools and CLI validation.

**Alternatives considered**:
- Kit-only activation without presets: rejected due drift risk and weaker reproducibility.
- Folder-local ad hoc generators: rejected due multi-root inconsistency.

## R-02: Multi-root workspace context strategy

**Question**: How should root and tests workspace contexts map to the same CMake project?

**Decision**: Keep root-level settings authoritative and ensure tests-folder settings bind to monorepo root project path while using identical preset names.

**Rationale**: Prevents context split where one folder resolves presets and another fails activation.

**Alternatives considered**:
- Separate independent CMake project under `tests/native`: rejected because tests link to root-native targets.
- Global workspace override only: rejected because folder-level state can still diverge.

## R-03: Validation gate for bug closure

**Question**: What is minimum sufficient evidence for closure?

**Decision**: Require tool-integrated success evidence (CMake Tools configure/build/test discovery and execution). Keep preset-CLI pass as supporting evidence when extension state is unstable.

**Rationale**: User impact is in CMake Tools behavior; closure must prove that path works.

**Alternatives considered**:
- CLI-only acceptance: rejected as insufficient for the actual reported bug.

## R-04: Failure diagnostics contract

**Question**: What failure output should be considered actionable?

**Decision**: Diagnostic output must clearly indicate one of: missing preset, invalid preset name, or workspace-context mismatch.

**Rationale**: Silent/ambiguous errors increase triage time and block validation.

**Alternatives considered**:
- Generic configure failure only: rejected due low triage value.
