# Banana Wiki

> This wiki is the fastest way to get productive in Banana without breaking runtime contracts.

## Live Right Now

| Surface | URL |
|---------|-----|
| Web workspace | **[banana.engineer](https://banana.engineer)** |
| REST API | **[banana-api.fly.dev](https://banana-api.fly.dev)** |

The React SPA (banana.engineer) is a Confluence-style workspace with pages: Workspace, Knowledge, Functions, BananaAI, Review Spikes, Classify, and Operator.
**BananaAI** routes queries through the live ensemble classification models.

## About This Space

Banana is a multi-language monorepo with a native C core, ASP.NET API, Fastify API, and React/Electron/Mobile frontends.
Canonical execution path: controller -> service -> pipeline -> native interop.

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
Deployments, CI runbooks, compose profiles, WSL2 runtime channels, and triage guides.
- [Deployment Runbook](operations/deployment.md)
- [CI and Compose Notes](operations/ci-compose-notes.md)
- [WSL2 Runtime Channels](operations/wsl2-runtime-channels.md)
- [CI Runtime Compatibility](operations/ci-runtime-compatibility.md)
- [Compose CI Stabilization](operations/ci-compose-stabilization.md)

### [Security](security/README.md)
- [Threat Model](security/threat-model.md)
- [CSP and Security Headers](security/csp-and-headers.md)

### [Data](data/README.md)
- [Migration Discipline](data/migration-discipline.md)
- [Coverage Exceptions](data/coverage-exceptions.md)

### [Governance](governance/README.md)
- [API Parity Governance](governance/api-parity-governance.md)
- [Autonomous Self-Training](governance/autonomous-self-training.md)

---

## Quick Reference

| I want to... | Go to... |
|-------------|----------|
| Open the live workspace | [banana.engineer](https://banana.engineer) |
| Hit the live API | [banana-api.fly.dev](https://banana-api.fly.dev) |
| Get set up on day one | [First Day Checklist](getting-started/first-day-checklist.md) |
| Understand request execution | [How A Request Works](getting-started/how-a-request-works.md) |
| Find a command | [Build, Run, and Test Commands](getting-started/build-run-test-commands.md) |
| Deploy React or API | [Deployment Runbook](operations/deployment.md) |
| Debug a CI failure | [CI and Compose Notes](operations/ci-compose-notes.md) |
| Understand the native boundary | [Native Wrapper ABI](architecture/native-wrapper-abi.md) |
| Check security posture | [Threat Model](security/threat-model.md) |
| Launch mobile/desktop channel | [WSL2 Runtime Channels](operations/wsl2-runtime-channels.md) |

---

## Runtime Contracts

- VITE_BANANA_API_BASE_URL  -- React frontend API base URL (Vercel project settings)
- BANANA_NATIVE_PATH        -- native library path for API and integration tests
- BANANA_PG_CONNECTION      -- PostgreSQL connection string
- BANANA_JWT_SECRET         -- JWT signing secret (>=32 bytes)

---

## Prompt-Friendly Project Map

- native core: src/native/
- ASP.NET API: src/c-sharp/asp.net/
- TypeScript (React/Electron/Mobile/Fastify): src/typescript/
- runtime delivery: docker/, scripts/, .github/workflows/
- live frontend: Vercel -> banana.engineer
- live API: Fly.io -> banana-api.fly.dev
