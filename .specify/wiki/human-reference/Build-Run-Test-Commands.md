# Build, Run, and Test Commands

> Read the [Wiki Home](Home.md) for more details.

Related pages: [First Day Checklist](First-Day-Checklist.md), [CI and Compose Notes](CI-Compose-Notes.md), [WSL2 Mobile Runtime Channels](WSL2-Mobile-Runtime-Channels.md)

This page contains the command snippets used for local development and CI parity.

## VS Code Tasks

- `Build Native Library`
- `Build Banana API`
- `Start Compose Apps`
- `Stop Compose Apps`

## Build Native (Windows)

```bat
scripts\build-native.bat
```

Expected native output:

- `build/native/bin/Release/banana_native.dll`

## Build Native And API (bash)

```bash
./scripts/build-all.sh
```

## Run API Locally

Option A (recommended wrapper script):

```bash
./scripts/run-api.sh
```

Option B (manual):

```bash
export BANANA_NATIVE_PATH="$(pwd)/build/native/bin/Release"
dotnet run --project src/c-sharp/asp.net/Banana.Api.csproj
```

PowerShell equivalent:

```powershell
$env:BANANA_NATIVE_PATH = "$PWD/build/native/bin/Release"
dotnet run --project src/c-sharp/asp.net/Banana.Api.csproj
```

Swagger URLs:

- `http://localhost:5000/swagger`
- `https://localhost:5001/swagger`

## Docker Compose Orchestration

Canonical profile orchestration:

```bash
bash scripts/compose-run-profile.sh --profile runtime --action up
bash scripts/compose-profile-ready.sh --profile runtime
bash scripts/compose-run-profile.sh --profile runtime --action down
```

Profile reproducibility validation:

```bash
bash scripts/validate-compose-run-profiles.sh --profile runtime --attempts 3
```

Run full apps stack (API + React + React Native web + DB/native-build dependencies):

```bash
bash scripts/compose-apps.sh
```

Detached mode:

```bash
DETACH=1 bash scripts/compose-apps.sh
```

Stop apps:

```bash
bash scripts/compose-apps-down.sh
```

Run tests profile:

```bash
bash scripts/compose-tests.sh
```

Run runtime API-only profile:

```bash
bash scripts/compose-runtime.sh
```

## One-Click WSL2 Channel Launchers

Desktop channel launcher (Windows shell):

```bash
bash -lc 'set -eo pipefail; scripts/launch-container-channels-with-wsl2-electron.sh'
```

Mobile channel launcher (Windows shell):

```bash
bash -lc 'set -eo pipefail; scripts/launch-container-channels-with-wsl2-mobile.sh'
```

Direct Ubuntu launchers (inside WSL):

```bash
scripts/compose-electron-desktop-wsl2.sh
scripts/compose-mobile-emulators-wsl2.sh
```

Important platform notes:

- Android emulator on Ubuntu WSL2 is supported when Android SDK tools and an AVD are installed in the distro.
- Apple iOS Simulator does not run on Ubuntu; the mobile launcher uses an iOS-style web preview fallback.

## Frontend And Electron

Compose-provided frontend API base contract (host browser resolvable):

- React web (`react-app`): `VITE_BANANA_API_BASE_URL=${BANANA_REACT_API_URL:-http://localhost:8080}`
- Electron desktop (`electron-desktop` preload): `BANANA_API_BASE_URL=${BANANA_ELECTRON_API_URL:-http://localhost:8080}`
- React Native web (`react-native-web`): `EXPO_PUBLIC_BANANA_API_BASE_URL=${BANANA_MOBILE_API_URL:-http://localhost:8080}`

Example override before launching compose:

```bash
export BANANA_REACT_API_URL="http://localhost:8080"
export BANANA_ELECTRON_API_URL="http://localhost:8080"
export BANANA_MOBILE_API_URL="http://localhost:8080"
```

React app (`src/typescript/react`, Bun package manager):

```bash
cd src/typescript/react
bun install
bun run dev
bun run check
bun run build
```

Electron smoke (`src/typescript/electron`):

```bash
cd src/typescript/electron
npm install
npm run smoke
```

Optional local native path for Electron smoke:

```bash
export BANANA_ENV_NATIVE_PATH="$(pwd)/build/native/bin/Release"
```

## Test Commands

```bash
dotnet test tests/unit/Banana.UnitTests.csproj -c Release
dotnet test tests/integration/Banana.IntegrationTests.csproj -c Release
ctest --test-dir build/native -C Release --output-on-failure
```

## Coverage Commands

Aggregated managed and native-friendly coverage flow:

```bash
export BANANA_PG_CONNECTION="host=127.0.0.1 port=5432 user=cinterop password=cinterop dbname=cinterop"
./scripts/run-tests-with-coverage.sh
```

Native-only coverage gate (80 percent line minimum):

```bash
export BANANA_PG_CONNECTION="host=127.0.0.1 port=5432 user=cinterop password=cinterop dbname=cinterop"
./scripts/run-native-c-tests-with-coverage.sh
```

## Runtime Contracts To Keep Explicit

- `BANANA_NATIVE_PATH`: API and integration native loading.
- `BANANA_PG_CONNECTION`: PostgreSQL-backed native and integration paths.
- `VITE_BANANA_API_BASE_URL`: React runtime API base URL.

