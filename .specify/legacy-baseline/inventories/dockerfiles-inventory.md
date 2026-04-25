# Dockerfiles + Compose Services (v1 snapshot)

## Dockerfiles (`docker/`)

| File | Purpose |
| --- | --- |
| `native-builder.Dockerfile` | Builds `libbanana_native.so`, copies to `/workspace/out/native/bin/`. Output mounted by other services as a named volume `native-artifacts`. |
| `api.Dockerfile` | ASP.NET 8 runtime. Reads `BANANA_NATIVE_PATH=/native` and `BANANA_PG_CONNECTION` injected via compose env. Exposes 8080. |
| `api-fastify.Dockerfile` | Fastify TypeScript API runtime (Bun). Uses `prisma.config.ts` + `DATABASE_URL`. |
| `react.Dockerfile` | Vite/React build with `VITE_BANANA_API_BASE_URL`. **Critical**: copies `shared/ui` package files BEFORE install and runs `bun install --cwd /workspace/src/typescript/shared/ui` first; `.dockerignore` MUST exclude `**/node_modules` to prevent host Windows symlinks from clobbering Linux installs. |
| `electron.Dockerfile` | Electron runtime. Uses `npm ci --omit=dev` (NOT `bun`) for lockfile reproducibility. Sets `CXXFLAGS=-fpermissive` for native module compatibility. Normalize CRLF in copied scripts (`sed -i 's/\r$//'`). |
| `react-native.Dockerfile` | RN/Expo runtime for web preview + Android shell. |
| `tests.Dockerfile` | Test runner image (dotnet + bun + native artifacts). |
| `workflow-local.Dockerfile` | Local-workflow harness (mirrors GH Actions environment). |

## Compose services (`docker-compose.yml`)

Compose project name: `banana-container`.

| Service | Profiles | Notes |
| --- | --- | --- |
| `postgres` | runtime, tests, electron, apps, all, api-fastify | postgres:16, user/db `cinterop`. Healthcheck via `pg_isready`. |
| `native-build` | runtime, tests, all, electron, apps, api-fastify | One-shot; builds + copies `libbanana_native.so` to `native-artifacts` volume. |
| `api` | runtime, electron, apps, all, api-fastify | ASP.NET. `depends_on` native-build (completed) + postgres (healthy). Mounts `native-artifacts:/native:ro`. Health: `/health`. Ports `8080:8080`. |
| `api-fastify` | api-fastify | Fastify peer. |
| `test-unit` | tests | xUnit unit tests. |
| `test-integration` | tests | Integration tests with postgres. |
| `test-all` | tests | Combined test runner. |
| `electron-example` | electron | **Smoke container (`npm run smoke`), NOT desktop UI**. |
| `electron-desktop` | electron | Desktop UI container; uses WSLg. |
| `react-app` | apps | React + Vite. |
| `react-native-web` | apps | RN web preview. |
| `android-emulator` | android-emulator (gated profile) | KVM-required Android emulator with WSLg. **Profile-gated** — must pass `--profile apps --profile android-emulator` to `docker compose ps`. |
| `workflow-train-not-banana-local` | workflows | Local trainer mirror. |
| `workflow-orchestrate-triaged-local` | workflows | Local triage mirror. |
| `workflow-orchestrate-sdlc-local` | workflows | Local SDLC mirror. |

Volumes: `native-artifacts` (shared `.so`).

Network: `banana-container_default` (default compose-managed). For ad-hoc Ubuntu test runs requiring `host=postgres`, attach the container to this network so compose DNS resolves.

## v2 Constraints (spec 012)

- Service names + profiles enumerated above are the v1 baseline. Channel scripts in `012/contracts/channel-scripts.md` MUST keep referencing services by name; renaming requires updating both contract + scripts atomically.
- Dockerfile lessons (CRLF normalization, `**/node_modules` ignore, `bun install --cwd` order, `npm ci --omit=dev` + `CXXFLAGS=-fpermissive`) are first-class invariants — preserve in v2.
