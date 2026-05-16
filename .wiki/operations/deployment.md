# Deployment Runbook

> [Home](../Home.md) › [Operations](README.md) › Deployment Runbook

Banana has two production surfaces: the React SPA on Vercel and the API runtime backing it.

---

## React SPA → Vercel (banana.engineer)

### Prerequisites

`VITE_BANANA_API_BASE_URL` must be set in the Vercel project settings to `https://api.banana.engineer` before building. The build bakes the value in at compile time.

### Deploy

```bash
# From repo root
vercel deploy --prod --force
```

The Vercel project is configured to build from `src/typescript/react` using:

```
cd src/typescript/react && bun install && bun run build
```

Output directory: `dist`

### Verify

Open [https://banana.engineer](https://banana.engineer) and confirm:
- The workspace shell loads (sidebar with Workspace, Knowledge, Functions, BananaAI, Review Spikes, Classify, Operator)
- The BananaAI page shows the ensemble model result when a query is submitted
- The Functions page "Validate Runtime Health" run button succeeds

---

## API Runtime Surface

### Notes

- Keep `BANANA_NATIVE_PATH` explicit for runtime environments that load native assets.
- Keep `BANANA_PG_CONNECTION` configured for PostgreSQL-backed flows.
- Keep `VITE_BANANA_API_BASE_URL` aligned with the production API host.

### Verify

```bash
# Health check
curl https://api.banana.engineer/health

# Chat bootstrap (should return 201 with session)
curl -X POST https://api.banana.engineer/chat/sessions \
  -H "Content-Type: application/json" \
  -d '{"platform":"web"}'

# Ensemble classification
curl -X POST https://api.banana.engineer/ml/ensemble \
  -H "Content-Type: application/json" \
  -d '{"inputJson":"{\"text\":\"yellow banana\"}"}'
```

### Environment Variables

| Secret | Purpose |
|--------|---------|
| `BANANA_PG_CONNECTION` | PostgreSQL connection string |
| `BANANA_JWT_SECRET` | JWT signing secret (>=32 bytes) |
| `BANANA_NATIVE_PATH` | Override native library path when runtime channel requires it |

---

## Local Development (without Compose)

```bash
# React standalone
cd src/typescript/react
VITE_BANANA_API_BASE_URL=http://localhost:8080 bun run dev

# TypeScript API standalone (requires native lib path for FFI-backed flows)
cd src/typescript/api
BANANA_NATIVE_PATH=/path/to/libbanana_native.so bun run dev
```

For full local runtime use the Compose profiles — see [WSL2 Runtime Channels](wsl2-runtime-channels.md).
