# Implementation Plan: React Native Mobile (v2)

**Branch**: `011-react-native-mobile` | **Date**: 2026-04-25 | **Spec**: [spec.md](./spec.md)

## Technical Context

**Language/Version**: TypeScript / RN / Expo.  
**Primary Dependencies**: Expo, Metro, `@banana/ui` (cross subset).  
**Target Platform**: Android via WSLg; iOS via web-preview fallback on Linux, native sim on macOS.  
**Constraints**: Profile-gated compose service; WSLg distro order shared with `010`.

## Project Structure

```text
src/typescript/react-native/
├── App.tsx
├── app.json + babel.config.js + metro.config.js
├── package.json + package-lock.json
└── src/                 # platform-aware screens/components
```

## Phasing

- **Phase 0**: Inventory imports from `@banana/ui`; flag any web-only.
- **Phase 1**: Add cross-platform import lint.
- **Phase 2**: Tighten launch scripts + emulator preflight messages.
- **Phase 3**: Decide `dist/` retention policy; gitignore where safe.
