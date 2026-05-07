# Tasks: Frontend Slice Infra & Env Wiring

**Input**: Design documents from `.specify/specs/042-frontend-slice-infra-env/`
**Prerequisites**: `spec.md`, `plan.md`

## Phase 1: Setup

- [x] T001 Verify current frontend launch + env behavior and capture baseline (`API base: <unset>`)

## Phase 2: Tests First (optional but recommended)

- [x] T002 Add/adjust React API env resolution test expectations only if behavior contract changes in code (expected: no app code change)
  - No React app code contract change was needed; fix is infra/runtime env wiring only.

## Phase 3: Core Implementation

- [x] T003 Remove hardcoded React API base env from `docker/react.Dockerfile`
- [x] T004 Update `docker-compose.yml` `react-app` env to `VITE_BANANA_API_BASE_URL: ${BANANA_REACT_API_URL:-http://localhost:8080}`
- [x] T005 Update `docker-compose.yml` `electron-desktop` env with `BANANA_API_BASE_URL: ${BANANA_ELECTRON_API_URL:-http://localhost:8080}`
- [x] T006 Update `docker-compose.yml` `react-native-web` env with `EXPO_PUBLIC_BANANA_API_BASE_URL: ${BANANA_MOBILE_API_URL:-http://localhost:8080}`
- [x] T007 Update `.vscode/tasks.json` to ensure frontend slice launch tasks include runtime readiness before slice readiness when needed
- [x] T008 Update `.vscode/launch.json` compounds/configs so each frontend slice launch path guarantees API infra readiness

## Phase 4: Documentation

- [x] T009 Update `.specify/wiki/human-reference/Build-Run-Test-Commands.md` with per-slice env var contract and override examples

## Phase 5: Validation

- [x] T010 Run targeted validation:
  - `bash -n scripts/compose-run-profile.sh scripts/compose-profile-ready.sh`
  - `docker compose config` (sanity check interpolated env blocks)
  - `bash scripts/compose-run-profile.sh --profile runtime --action up`
  - `bash scripts/compose-profile-ready.sh --profile runtime`
  - `bash scripts/compose-run-profile.sh --profile apps --action up`
  - `docker compose exec react-app printenv VITE_BANANA_API_BASE_URL`
  - `docker compose exec electron-desktop printenv BANANA_API_BASE_URL` (if electron profile launched)
  - `docker compose exec react-native-web printenv EXPO_PUBLIC_BANANA_API_BASE_URL` (if apps profile launched)
  - `bash scripts/compose-run-profile.sh --profile apps --action down`
  - `bash scripts/compose-run-profile.sh --profile runtime --action down`
  - Verified output:
    - `docker compose --profile apps config | grep -E "VITE_BANANA_API_BASE_URL|BANANA_API_BASE_URL|EXPO_PUBLIC_BANANA_API_BASE_URL"` shows all set to `http://localhost:8080`
    - `docker compose exec react-app printenv VITE_BANANA_API_BASE_URL` -> `http://localhost:8080`
    - `docker compose exec react-native-web printenv EXPO_PUBLIC_BANANA_API_BASE_URL` -> `http://localhost:8080`

## Phase 6: Close-out

- [x] T011 Mark all completed tasks and summarize outcomes in this file
