# Feature Specification: Frontend Slice Infra & Env Wiring

**Feature Branch**: `[042-frontend-slice-infra-env]`
**Created**: 2026-04-26
**Status**: Draft
**Input**: User description: "fix the launch profiles so they spin up infra needed for each slice we are interacting with (frontend platform as a slice) the web app launches but I just see API base: <unset>"

## In Scope *(mandatory)*

- Fix `VITE_BANANA_API_BASE_URL` so the browser-accessible URL (`http://localhost:8080`) reaches the API from the host, not the container-internal hostname `api`.
- Remove the hardcoded `ENV VITE_BANANA_API_BASE_URL=http://api:8080` from `docker/react.Dockerfile` and replace with a compose-supplied runtime value so different environments can override.
- Ensure the `docker-compose.yml` `react-app` service passes `VITE_BANANA_API_BASE_URL: ${BANANA_REACT_API_URL:-http://localhost:8080}` so the dev Vite server receives a browser-resolvable URL.
- Ensure Electron's `BANANA_API_BASE_URL` is wired to a browser-resolvable host URL in the compose `electron-desktop` service.
- Ensure React Native / Expo web preview receives `EXPO_PUBLIC_BANANA_API_BASE_URL` (or equivalent) in the `react-native-web` compose service.
- Update VS Code `launch.json` compounds for each frontend slice (`Frontends: API + React`, `Frontends: API + Electron`, `Frontends: API + React Native`) to use a compound that first starts the `runtime` profile (postgres + native-build + api) if not already up, then starts the frontend profile.
- Document the env var contract per slice in `.specify/wiki/human-reference/Build-Run-Test-Commands.md` and in the compose file inline.

## Out of Scope *(mandatory)*

- Changing how the API itself resolves its own env vars.
- Modifying production or CI build-time Vite bundling (the fix is scoped to dev/local compose mode).
- Adding new application features to the React, Electron, or React Native frontends.
- Rewriting the VS Code task infrastructure beyond what is needed for correct infra startup ordering.

## User Scenarios & Testing *(mandatory)*

### User Story 1 - React web app shows resolved API URL (Priority: P1)

As a developer launching `Frontends: API + React` via VS Code F5, I want the web app to connect to the live API so that the Banana dashboard works end-to-end without manual env var setup.

**Why this priority**: The `API base: <unset>` symptom means every API call silently fails; this is the most visible breakage in the dev loop.

**Independent Test**: Press F5 → `Frontends: API + React`. Open `http://localhost:5173`. The banner shows `API base: http://localhost:8080` (not `<unset>`). The `/health` endpoint is reachable and banana summary loads.

**Acceptance Scenarios**:

1. **Given** a clean Docker environment, **When** `Compose: apps up + ready` completes, **Then** `docker compose exec react-app printenv VITE_BANANA_API_BASE_URL` prints `http://localhost:8080`.
2. **Given** the apps profile is running, **When** the browser opens `http://localhost:5173`, **Then** the React app shows `API base: http://localhost:8080` and the banana summary loads successfully.
3. **Given** `BANANA_REACT_API_URL` is set in the host environment before compose up, **When** the react-app container starts, **Then** `VITE_BANANA_API_BASE_URL` reflects the override value.

---

### User Story 2 - Electron desktop shows resolved API URL (Priority: P2)

As a developer launching `Frontends: API + Electron`, I want the Electron window to connect to `http://localhost:8080` so the preload bridge resolves `apiBaseUrl` correctly.

**Why this priority**: Same root cause as the React slice; once the pattern is fixed for React the Electron fix is low-risk and delivers full desktop dev loop.

**Independent Test**: Press F5 → `Frontends: API + Electron`. The Electron app opens. The title bar or console logs show `apiBaseUrl: http://localhost:8080` (not empty / undefined).

**Acceptance Scenarios**:

1. **Given** the electron-desktop compose service is running, **When** `docker compose exec electron-desktop printenv BANANA_API_BASE_URL`, **Then** it prints `http://localhost:8080`.
2. **Given** the electron-desktop window is open, **When** a banana classify request is made, **Then** it reaches the API container and returns a result.

---

### User Story 3 - React Native web preview shows resolved API URL (Priority: P3)

As a developer launching `Frontends: API + React Native (web)`, I want the Expo web preview at `http://localhost:19006` to have a browser-resolvable API URL.

**Why this priority**: Lower-traffic dev path; mobile emulator launch is the primary mobile experience. Web preview is a fast fallback that should still function.

**Independent Test**: Press F5 → `Frontends: API + React Native (web)`. Open `http://localhost:19006`. No network errors related to unresolved API hostname appear in the browser console.

**Acceptance Scenarios**:

1. **Given** the react-native-web compose service is running, **When** `docker compose exec react-native-web printenv EXPO_PUBLIC_BANANA_API_BASE_URL`, **Then** it prints `http://localhost:8080`.

---

### User Story 4 - VS Code compound launch starts API infra before frontend (Priority: P2)

As a developer pressing F5 for any frontend slice, I want the API and its dependencies (postgres, native-build) to be confirmed healthy before the frontend container starts, so I never see a race where the React app loads but the API returns 503.

**Why this priority**: The `api` service is already in the `apps` profile, but its dependencies (postgres ready, native-build completed) have no guaranteed ordering when launched as a single `apps` profile. Making the runtime readiness explicit eliminates flaky first-launch behaviour.

**Independent Test**: On a cold Docker Desktop (all containers stopped), press F5 → `Frontends: API + React`. The apps profile starts. The `compose-profile-ready.sh` wait loop confirms the API health endpoint returns 200 before the terminal unblocks.

**Acceptance Scenarios**:

1. **Given** all containers are stopped, **When** `Compose: apps up + ready` runs, **Then** the task does not exit until `http://localhost:8080/health` returns HTTP 200.
2. **Given** the API is unhealthy (native-build not complete), **When** `compose-profile-ready.sh --profile apps` is called, **Then** it waits and retries until healthy or times out with a clear error.

## Technical Notes

### Root Cause

`docker/react.Dockerfile` bakes `ENV VITE_BANANA_API_BASE_URL=http://api:8080` at image build time. The value `api` is a Docker-internal service hostname not resolvable from the **host browser**. When Vite dev server injects this value into `import.meta.env`, the browser fetches `http://api:8080` and fails silently (or the variable resolves as `<unset>` if the build-time `ENV` is shadowed by an empty runtime override).

### Fix Approach

1. **Remove** `ENV VITE_BANANA_API_BASE_URL=http://api:8080` from `docker/react.Dockerfile`.
2. **Add** `VITE_BANANA_API_BASE_URL: ${BANANA_REACT_API_URL:-http://localhost:8080}` to the `react-app` service `environment` block in `docker-compose.yml` (replacing the current `http://api:8080` value).
3. **Add** `BANANA_API_BASE_URL: ${BANANA_ELECTRON_API_URL:-http://localhost:8080}` to the `electron-desktop` service `environment` block.
4. **Add** `EXPO_PUBLIC_BANANA_API_BASE_URL: ${BANANA_MOBILE_API_URL:-http://localhost:8080}` to the `react-native-web` service `environment` block.
5. The `BANANA_*_API_URL` overrides allow CI or other environments to supply a different base URL without touching the compose file.

### Env Var Contract Per Slice

| Slice | Container env var | Default dev value | Override env var |
|-------|-------------------|-------------------|-----------------|
| React (web) | `VITE_BANANA_API_BASE_URL` | `http://localhost:8080` | `BANANA_REACT_API_URL` |
| Electron desktop | `BANANA_API_BASE_URL` | `http://localhost:8080` | `BANANA_ELECTRON_API_URL` |
| React Native web | `EXPO_PUBLIC_BANANA_API_BASE_URL` | `http://localhost:8080` | `BANANA_MOBILE_API_URL` |

### Files Touched

- `docker/react.Dockerfile` — remove hardcoded `ENV VITE_BANANA_API_BASE_URL`
- `docker-compose.yml` — update `react-app`, `electron-desktop`, `react-native-web` environment blocks
- `.specify/wiki/human-reference/Build-Run-Test-Commands.md` — document env var contract
