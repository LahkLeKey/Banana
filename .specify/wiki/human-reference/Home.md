# Banana Wiki

> This wiki is the fastest way to get productive in Banana without breaking runtime contracts.

## About This Space

Banana is a multi-language monorepo with a native C core, ASP.NET API, Fastify API, and React/Electron/Mobile frontends.
The canonical execution path is `controller → service → pipeline → native interop`.

---

## Sections

### [Getting Started](getting-started/README.md)
Everything you need on day one: setup checklist, request walkthrough, commands, and repo map.
- [First Day Checklist](getting-started/first-day-checklist.md)
- [How A Request Works](getting-started/how-a-request-works.md)
- [Build, Run, and Test Commands](getting-started/build-run-test-commands.md)
- [Repository Layout](getting-started/repository-layout.md)

### [Architecture](architecture/README.md)
System diagrams, native wrapper design, database pipeline, and domain model.
- [Overview](architecture/overview.md)
- [Native Wrapper ABI](architecture/native-wrapper-abi.md)
- [Database Pipeline Stage](architecture/database-pipeline-stage.md)
- [Domain Model](architecture/domain-model.md)

### [Operations](operations/README.md)
CI runbooks, compose profiles, WSL2 runtime channels, and triage guides.
- [CI and Compose Notes](operations/ci-compose-notes.md)
- [WSL2 Runtime Channels](operations/wsl2-runtime-channels.md)
- [CI Runtime Compatibility](operations/ci-runtime-compatibility.md)
- [Compose CI Stabilization](operations/ci-compose-stabilization.md)

### [Security](security/README.md)
Threat model, STRIDE analysis, secret inventory, and security headers.
- [Threat Model](security/threat-model.md)
- [CSP and Security Headers](security/csp-and-headers.md)

### [Data](data/README.md)
Migration discipline, coverage exceptions, and data contracts.
- [Migration Discipline](data/migration-discipline.md)
- [Coverage Exceptions](data/coverage-exceptions.md)

### [Governance](governance/README.md)
API parity rules, autonomous self-training contracts, and policy documents.
- [API Parity Governance](governance/api-parity-governance.md)
- [Autonomous Self-Training](governance/autonomous-self-training.md)

---

## Quick Reference

| I want to... | Go to... |
|-------------|----------|
| Get set up on day one | [First Day Checklist](getting-started/first-day-checklist.md) |
| Understand request execution | [How A Request Works](getting-started/how-a-request-works.md) |
| Find a command | [Build, Run, and Test Commands](getting-started/build-run-test-commands.md) |
| Debug a CI failure | [CI and Compose Notes](operations/ci-compose-notes.md) |
| Understand the native boundary | [Native Wrapper ABI](architecture/native-wrapper-abi.md) |
| Check security posture | [Threat Model](security/threat-model.md) |
| Launch mobile/desktop channel | [WSL2 Runtime Channels](operations/wsl2-runtime-channels.md) |

---

## Runtime Contracts You Should Not Drift

- `BANANA_NATIVE_PATH` — native library path for API and integration tests
- `BANANA_PG_CONNECTION` — PostgreSQL-backed native/integration flows
- `VITE_BANANA_API_BASE_URL` — React frontend API base URL
- `BANANA_JWT_SECRET` — JWT signing secret (>=32 bytes random)

---

## Prompt-Friendly Project Map

Use these boundaries when asking an agent to make changes:

- native core domain: `src/native/core/domain`
- native DAL: `src/native/core/dal`
- wrapper ABI + adapters: `src/native/wrapper`
- ASP.NET API pipeline: `src/c-sharp/asp.net`
- frontend/electron/mobile: `src/typescript`
- runtime and delivery automation: `docker`, `scripts`, `.github/workflows`
