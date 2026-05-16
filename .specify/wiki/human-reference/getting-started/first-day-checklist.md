<!-- breadcrumb: Getting Started > First Day Checklist -->

# First Day Checklist

> [Home](../Home.md) › [Getting Started](README.md) › First Day Checklist

Related pages: [Build, Run, and Test Commands](build-run-test-commands.md), [How A Request Works](how-a-request-works.md)

Use this page as a practical setup guide.

## Step 1: Learn The Map

Read these pages in order:

1. [How A Request Works](how-a-request-works.md)
2. [Database Pipeline Stage](../architecture/database-pipeline-stage.md)
3. [Architecture Overview](../architecture/overview.md)
4. [Repository Layout](repository-layout.md)
5. [Build, Run, and Test Commands](build-run-test-commands.md)
6. [WSL2 Runtime Channels](../operations/wsl2-runtime-channels.md)
7. [Native Wrapper ABI](../architecture/native-wrapper-abi.md)

## Step 2: Follow One Request In Code

Open these folders/files in this exact order:

1. `src/typescript/api/src/routes/`
2. `src/typescript/api/src/domains/`
3. `src/typescript/api/src/lib/anti-cheat-interop.ts`
4. `src/native/wrapper/`
5. `src/native/core/domain/`
6. `src/native/core/dal/domain/`

Goal: see how one /banana request moves through the app.

## Step 3: Run The Project

1. Build native with VS Code task `Build Native Library` or `scripts/build-native.bat`.
2. Build API with `bun run --cwd src/typescript/api build`.
3. Run tests:
                - `bun test --cwd src/typescript/api`
        - `ctest --test-dir build/native -C Release --output-on-failure`
4. Start API locally with `scripts/run-api.sh`.
5. Open Swagger and call:
        - `GET /health`
        - `GET /banana?purchases=10&multiplier=2`
        - `POST /batches/create`
        - `POST /ripeness/predict`
6. Optional runtime channels:
        - Desktop one-click launcher: `bash -lc 'set -eo pipefail; scripts/launch-container-channels-with-wsl2-electron.sh'`
        - Mobile one-click launcher: `bash -lc 'set -eo pipefail; scripts/launch-container-channels-with-wsl2-mobile.sh'`
        - Note: iOS Simulator is macOS-only; Ubuntu path uses iOS-style web preview fallback.

## Step 4: If Something Fails

- Native load issue: check BANANA_NATIVE_PATH.
- DB issue in native path: check BANANA_PG_CONNECTION.
- DB issue in API path: check `DATABASE_URL`/Prisma runtime config and `BANANA_PG_CONNECTION` for native flows.
- Wrong /banana output: inspect pipeline step order and metadata headers.
- 500 response: check `ErrorHandlingMiddleware` mapping and logs.
- Mobile launch issue: check WSLg mount, Android SDK + AVD in Ubuntu, and `http://localhost:19006` readiness.

## Step 5: First Safe Change For New Contributors

1. Add a small test-only improvement.
2. Run all tests.
3. Update the closest wiki page or `.github` guidance if behavior changed.

## Simple Mental Model

- Route handler receives transport input.
- Service/domain layer orchestrates route-specific behavior.
- Wrapper exports are the only native ABI used by API callers.
- Native core domain handles business behavior.
- Native DAL handles projection and aggregate persistence concerns.
