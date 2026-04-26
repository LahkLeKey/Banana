# First Day Checklist

> Read the [Wiki Home](Home.md) for more details.

Related pages: [Build, Run, and Test Commands](Build-Run-Test-Commands.md), [How A Request Works](How-A-Request-Works.md)

Use this page as a practical setup guide.

## Step 1: Learn The Map

Read these pages in order:

1. [How-A-Request-Works.md](How-A-Request-Works.md)
2. [How-The-Database-Step-Works.md](How-The-Database-Step-Works.md)
3. [Architecture-Diagrams.md](Architecture-Diagrams.md)
4. [Repository-Layout.md](Repository-Layout.md)
5. [Build-Run-Test-Commands.md](Build-Run-Test-Commands.md)
6. [WSL2-Mobile-Runtime-Channels.md](WSL2-Mobile-Runtime-Channels.md)
7. [Why-We-Use-A-Wrapper.md](Why-We-Use-A-Wrapper.md)
8. [AI-Customization-Map.md](AI-Customization-Map.md)

## Step 2: Follow One Request In Code

Open these folders/files in this exact order:

1. `src/c-sharp/asp.net/Controllers/BananaController.cs`
2. `src/c-sharp/asp.net/Services/BananaService.cs`
3. `src/c-sharp/asp.net/Pipeline/Steps/`
4. `src/c-sharp/asp.net/DataAccess/`
5. `src/c-sharp/asp.net/NativeInterop/`
6. `src/native/wrapper/`
7. `src/native/core/domain/`
8. `src/native/core/dal/domain/`

Goal: see how one /banana request moves through the app.

## Step 3: Run The Project

1. Build native with VS Code task `Build Native Library` or `scripts/build-native.bat`.
2. Build API with VS Code task `Build Banana API`.
3. Run tests:
	- `dotnet test tests/unit/Banana.UnitTests.csproj -c Release`
	- `dotnet test tests/integration/Banana.IntegrationTests.csproj -c Release`
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
- DB issue in managed path: check `ConnectionStrings:DefaultConnection` and `DbAccess` options.
- Wrong /banana output: inspect pipeline step order and metadata headers.
- 500 response: check `ErrorHandlingMiddleware` mapping and logs.
- Mobile launch issue: check WSLg mount, Android SDK + AVD in Ubuntu, and `http://localhost:19006` readiness.

## Step 5: First Safe Change For New Contributors

1. Add a small test-only improvement.
2. Run all tests.
3. Update the closest wiki page or `.github` guidance if behavior changed.

## Simple Mental Model

- Controller receives transport input.
- Service orchestrates route-specific behavior.
- Pipeline runs ordered steps for `GET /banana`.
- Wrapper exports are the only native ABI used by managed callers.
- Native core domain handles business behavior.
- Native DAL handles projection and aggregate persistence concerns.
