# Implementation Plan: Compose Runtime (v2)

**Branch**: `012-compose-runtime` | **Date**: 2026-04-25 | **Spec**: [spec.md](./spec.md)

## Technical Context

**Language/Version**: Bash, YAML (compose), Dockerfile.  
**Primary Dependencies**: Docker Desktop, WSL2, GH CLI.  
**Target Platform**: Windows host orchestrating Linux containers via WSL2.  
**Constraints**: All env contracts and exit codes from `005-v2-regeneration` parent.

## Project Structure

```text
docker-compose.yml
docker/
├── api.Dockerfile               # canonical .NET API
├── api-fastify.Dockerfile       # canonical Fastify API (if both retained)
├── electron.Dockerfile
├── react.Dockerfile
├── react-native.Dockerfile
├── native-builder.Dockerfile
├── tests.Dockerfile
└── workflow-local.Dockerfile

scripts/
├── launch-container-channels-with-wsl2-electron.sh
├── launch-container-channels-with-wsl2-mobile.sh
├── compose-electron-desktop-wsl2.sh
├── compose-mobile-emulators-wsl2.sh
├── compose-apps.sh
├── compose-apps-down.sh
├── workflow-orchestrate-triaged-item-pr.sh
├── workflow-orchestrate-sdlc.sh
└── workflow-sync-wiki.sh
```

## Phasing

- **Phase 0**: Inventory all `scripts/*.sh`; mark canonical vs helper vs deprecated.
- **Phase 1**: Document channel script catalog in `contracts/channel-scripts.md`.
- **Phase 2**: Verify CRLF + dockerignore invariants in all Dockerfiles.
- **Phase 3**: Pin base image digests; record residual highs as platform risk.
- **Phase 4**: After `008` route-ownership decision, consolidate API Dockerfiles if applicable.
