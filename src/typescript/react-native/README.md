# Banana React Native App

Expo + TypeScript mobile app for Banana runtime checks, reusing shared primitives from @banana/ui.

## Quick Start

1. Install shared UI dependencies:
   - cd ../shared/ui && bun install
2. Install mobile app dependencies:
   - npm install
3. Start Expo:
   - npm run start

## Compose Runtime

- The main VS Code launch profile `Banana Channels (Container Driven)` starts this app through Docker Compose as `react-native-web`.
- Compose endpoint: `http://localhost:19006`
- The container sets `EXPO_PUBLIC_BANANA_API_BASE_URL=http://localhost:8080`.

## Shared UI Contract

- Import primitives from @banana/ui/native.
- Keep the root @banana/ui package as the shared source of truth for web, Electron, and React Native primitives.
- Keep component API parity in src/typescript/shared/ui when adding shared controls.

## Scripts

- npm run start
- npm run android
- npm run ios
- npm run web
- npm run check

## API URL Notes

- The app default uses `EXPO_PUBLIC_BANANA_API_BASE_URL` when provided.
- Android emulator can access host localhost through http://10.0.2.2:8080.
- iOS simulator can use http://127.0.0.1:8080.
- Physical devices should use your machine LAN IP.
