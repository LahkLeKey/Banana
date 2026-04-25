# Banana Wiki Home

This wiki is the fastest way to get productive in Banana without breaking runtime contracts.

## 10-Minute Spec Kit Path

1. Run [First-Day-Checklist.md](First-Day-Checklist.md).
2. Launch VS Code profile `Demo Channel` and verify `/health` and `/banana`.
3. Read [How-A-Request-Works.md](How-A-Request-Works.md) once, end to end.
4. Keep [Build-Run-Test-Commands.md](Build-Run-Test-Commands.md) open while coding.

If you only read one architecture line, read this one:

- `controller -> service -> pipeline -> native interop`

## What Banana Actually Ships

- API routes:
	- `GET /health`
	- `GET /banana`
	- `POST /batches/create`
	- `GET /batches/{id}/status`
	- `POST /ripeness/predict`
- Native-first domain model in C with a stable wrapper ABI.
- Two DB projection paths: `NativeDal` and `ManagedNpgsql`.
- One-click desktop and mobile runtime channels for WSL2/WSLg.

## Where To Read Next (By Intent)

- New to repo: [First-Day-Checklist.md](First-Day-Checklist.md)
- Understand request execution: [How-A-Request-Works.md](How-A-Request-Works.md)
- Understand DB pipeline stage: [How-The-Database-Step-Works.md](How-The-Database-Step-Works.md)
- Visual architecture context: [Architecture-Diagrams.md](Architecture-Diagrams.md)
- Find code quickly: [Repository-Layout.md](Repository-Layout.md)
- Run the right command first: [Build-Run-Test-Commands.md](Build-Run-Test-Commands.md)
- Mobile runtime contracts: [WSL2-Mobile-Runtime-Channels.md](WSL2-Mobile-Runtime-Channels.md)
- CI and compose troubleshooting: [CI-Compose-Notes.md](CI-Compose-Notes.md)
- Why wrapper exists: [Why-We-Use-A-Wrapper.md](Why-We-Use-A-Wrapper.md)
- AI guidance map: [AI-Customization-Map.md](AI-Customization-Map.md)

## Prompt-Friendly Project Map

Use these boundaries when asking an agent to make changes:

- native core domain: `src/native/core/domain`
- native DAL: `src/native/core/dal`
- wrapper ABI + adapters: `src/native/wrapper`
- ASP.NET API pipeline: `src/c-sharp/asp.net`
- frontend/electron/mobile: `src/typescript`
- runtime and delivery automation: `docker`, `scripts`, `.github/workflows`

## Contracts You Should Not Drift

- Native path for managed integration: `BANANA_NATIVE_PATH`
- PostgreSQL-backed native/integration flows: `BANANA_PG_CONNECTION`
- Frontend API base URL: `VITE_BANANA_API_BASE_URL`
- WSL2 mobile/desktop launch contracts documented in [WSL2-Mobile-Runtime-Channels.md](WSL2-Mobile-Runtime-Channels.md)

## Quick Word Guide

- core domain: native behavior grouped by cultivation, processing, supply chain, logistics, retail, and related contexts
- core DAL: native persistence and projection code in `src/native/core/dal`
- wrapper: ABI boundary in `src/native/wrapper` used by managed callers
- pipeline: ordered ASP.NET request steps used by the banana calculation route

<!-- AUTO-SYNC-WIKI-NAV START -->
## Wiki Navigation

### Human Reading
- [Human-Reading-Guide.md](Human-Reading-Guide.md)

### AI Audit Trails
- [AI-Audit-Trails.md](AI-Audit-Trails.md)
- [Auto-SDLC-Snapshots.md](Auto-SDLC-Snapshots.md)

- Source commit: 3fb1207
- Synced at (UTC): 2026-04-25T19:15:28Z
<!-- AUTO-SYNC-WIKI-NAV END -->
