# Feature Specification: React Native Mobile (v2)

**Feature Branch**: `011-react-native-mobile`  
**Created**: 2026-04-25  
**Status**: Draft  
**Parent**: [`005-v2-regeneration`](../005-v2-regeneration/spec.md)  
**Baseline**: [`legacy-baseline/react-native-mobile.md`](../../legacy-baseline/react-native-mobile.md)

## Summary

Regenerate the Expo/RN mobile app with documented Android emulator
orchestration on Ubuntu WSL2/WSLg, an explicit iOS-on-Linux fallback policy,
and a clear list of `@banana/ui` exports it can reuse.

## User Scenarios & Testing

### User Story 1 — Android emulator on Ubuntu WSL2 (Priority: P1)

As a developer, `scripts/launch-container-channels-with-wsl2-mobile.sh`
brings up the Android emulator profile via WSLg when SDK tools are present.

### User Story 2 — iOS web-preview fallback on Linux (Priority: P1)

As a Linux developer, an iOS Simulator is unavailable; the spec preserves a
documented web-preview fallback rather than silently failing.

### User Story 3 — Cross-platform UI reuse (Priority: P2)

As an RN developer, I import only the `cross`-labeled exports from `@banana/ui`;
web-only Tailwind components are forbidden in RN source.

### User Story 4 — Profile-aware compose visibility (Priority: P2)

As an operator, `docker compose ps --profile apps --profile android-emulator`
correctly lists the emulator service.

### Edge Cases

- WSLg or Android SDK missing → fail with remediation; do not silently downgrade to web.
- Emulator container started without profile flags → not visible to `ps`; documented in quickstart.

## Requirements

### Functional Requirements

- **FR-001**: v2 MUST keep `scripts/launch-container-channels-with-wsl2-mobile.sh` (Windows) and `scripts/compose-mobile-emulators-wsl2.sh` (Ubuntu) entry points.
- **FR-002**: v2 MUST preserve iOS Simulator policy: macOS-only; Linux uses web-preview fallback.
- **FR-003**: v2 MUST keep Android emulator service profile-gated; documentation MUST require explicit `--profile apps --profile android-emulator` for `docker compose ps`.
- **FR-004**: v2 MUST consume only `cross`-labeled exports from `@banana/ui`.
- **FR-005**: v2 SHOULD reduce noise around `dist/` (artifact) vs source by gitignoring build output where safe.

### Hard contracts to preserve

- Windows + Ubuntu mobile entry-point script names.
- Profile-gated emulator service name.
- iOS web-preview fallback policy.
- Distro selection order shared with `010`.

### Pain points addressed

- Shared UI vs RN compatibility → cross-platform labeling (FR-004).
- Emulator preflight fragility → explicit remediation in scripts.
- `dist/` checked-in artifacts → reviewed (FR-005).

### Pain points deferred

- Mixed PMs (npm here) — keep until a Bun-RN story matures.

## Success Criteria

- **SC-001**: Android emulator launches end-to-end on Ubuntu WSL2 with WSLg and SDK tools installed.
- **SC-002**: iOS path on Linux returns a documented web-preview, not a silent failure.
- **SC-003**: RN source imports only `cross`-labeled `@banana/ui` exports (lint-enforced).
- **SC-004**: Compose visibility documented with required profile flags.
