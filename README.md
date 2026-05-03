# Banana

A multi-language monorepo with a native C core projected into API, web, desktop, and mobile channels — and a live AI workspace at **[banana.engineer](https://banana.engineer)**.

> Core idea: keep domain behavior in native C, project it cleanly into multiple runtimes without rewriting business logic per app. BananaAI composes the custom classification models into a unified assistant experience.

## Live Deployments

| Surface | URL | Stack |
|---------|-----|-------|
| Web workspace | **[banana.engineer](https://banana.engineer)** | React + Vite → Vercel |
| REST API | **[api.banana.engineer](https://api.banana.engineer)** | ASP.NET 8 + native C → Fly.io |

<img width="1007" height="1154" alt="image" src="https://github.com/user-attachments/assets/d2b7c6fa-9540-4c66-9b81-fc5b09294f29" />


## Web Suite (banana.engineer)

The React SPA is a Confluence-style workspace with the following pages:

| Page | Path | What it does |
|------|------|-------------|
| Workspace | `/workspace` | Project home — overview and quick links |
| Knowledge | `/knowledge` | Architecture notes and domain reference |
| Functions | `/functions` | Runnable platform functions (health, banana summary) |
| BananaAI | `/banana-ai` | Unified assistant — live ensemble classification |
| Review Spikes | `/review-spikes` | Scoped research slices (161-165) |
| Classify | `/classify` | Banana image classifier |
| Operator | `/operator` | Operational runbook assistant |

**BananaAI** routes queries through `POST /ml/ensemble/embedding` and returns a live label, score, escalation flag, and embedding fingerprint from the custom models.

## Architecture

```
native C (src/native/)
    └── ASP.NET API (src/c-sharp/asp.net/)  →  api.banana.engineer
    └── Fastify API (src/typescript/api/)   →  local / compose runtime
React SPA (src/typescript/react/)           →  banana.engineer (Vercel)
Electron  (src/typescript/electron/)        →  desktop channel
Mobile    (src/typescript/react-native/)    →  iOS / Android
```

Canonical execution path: `controller → service → pipeline → native interop`

## Required Environment Variables

| Variable | Where | Purpose |
|----------|-------|---------|
| `VITE_BANANA_API_BASE_URL` | Vercel / local React | API base URL for the SPA |
| `BANANA_NATIVE_PATH` | API host | Path to `libbanana_native.so` |
| `BANANA_PG_CONNECTION` | API host | PostgreSQL connection string |
| `BANANA_JWT_SECRET` | API host | JWT signing secret (≥ 32 bytes) |

## Local Development

### Run the full compose stack

```bash
# Runtime (native + ASP.NET)
bash scripts/compose-run-profile.sh --profile runtime --action up
bash scripts/compose-profile-ready.sh --profile runtime

# Apps (Fastify API + React)
bash scripts/compose-run-profile.sh --profile apps --action up

# Electron desktop
bash scripts/compose-run-profile.sh --profile electron-desktop --action up

# Mobile Android
bash scripts/compose-run-profile.sh --profile mobile --action up
```

### React (standalone)

```bash
cd src/typescript/react
VITE_BANANA_API_BASE_URL=http://localhost:8080 bun run dev
```

## Deployment

### React → Vercel

```bash
vercel deploy --prod --force
```

`VITE_BANANA_API_BASE_URL` must be set in Vercel project settings before building.

### ASP.NET API → Fly.io

```bash
flyctl deploy --app banana-api --dockerfile docker/fly.Dockerfile
```

## Component Library

[![Storybook Deploy](https://github.com/LahkLeKey/Banana/actions/workflows/storybook-deploy.yml/badge.svg)](https://github.com/LahkLeKey/Banana/actions/workflows/storybook-deploy.yml)

Interactive component stories: **[lahklekey.github.io/Banana](https://lahklekey.github.io/Banana/)**

## Further Reading

- Architecture and onboarding: `docs/developer-onboarding.md`
- Deployment runbook: `docs/deployment.md`
- Wiki: [Banana Wiki](https://github.com/LahkLeKey/Banana/wiki)
- Spec drain state: `artifacts/sdlc-orchestration/`
