# Deployment Runbook

> [Home](../Home.md) › [Operations](README.md) › Deployment Runbook

Banana has two production surfaces: the React SPA on Vercel and the ASP.NET API on Fly.io.

---

## React SPA -> Vercel (banana.engineer)

### Prerequisites

`VITE_BANANA_API_BASE_URL` must be set in the Vercel project settings to `https://banana-api.fly.dev` before building. The build bakes the value in at compile time.

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

## ASP.NET API -> Fly.io (banana-api.fly.dev)

### Prerequisites

- `flyctl` installed and authenticated (`flyctl auth login`)
- The `banana-api` app exists in Fly.io (`flyctl status --app banana-api`)

### Deploy

```bash
# From repo root
flyctl deploy --app banana-api --dockerfile docker/fly.Dockerfile
```

The multi-stage Dockerfile (`docker/fly.Dockerfile`) builds:
1. Native C library (`libbanana_native.so`) from `src/native/`
2. ASP.NET 8 API from `src/c-sharp/asp.net/`

Both are bundled into the final `mcr.microsoft.com/dotnet/aspnet:8.0` image.

### Verify

```bash
# Health check
curl https://banana-api.fly.dev/health

# Chat bootstrap (should return 201 with session)
curl -X POST https://banana-api.fly.dev/chat/sessions \
  -H "Content-Type: application/json" \
  -d '{"platform":"web"}'

# Ensemble classification
curl -X POST https://banana-api.fly.dev/ml/ensemble \
  -H "Content-Type: application/json" \
  -d '{"inputJson":"{\"text\":\"yellow banana\"}"}'
```

### Environment Variables (Fly.io secrets)

Set via `flyctl secrets set KEY=VALUE --app banana-api`:

| Secret | Purpose |
|--------|---------|
| `BANANA_PG_CONNECTION` | PostgreSQL connection string |
| `BANANA_JWT_SECRET` | JWT signing secret (>=32 bytes) |
| `BANANA_NATIVE_PATH` | Override native library path (optional; embedded in image) |

---

## DNS Warning

Fly.io DNS verification may show a timeout warning on deploy:

```
WARNING: DNS verification failed: read udp ... i/o timeout
```

This is a local DNS resolver issue - the app is deployed and reachable. The warning can be ignored.

---

## Local Development (without Compose)

```bash
# React standalone
cd src/typescript/react
VITE_BANANA_API_BASE_URL=http://localhost:8080 bun run dev

# ASP.NET API standalone (requires native lib)
cd src/c-sharp/asp.net
BANANA_NATIVE_PATH=/path/to/libbanana_native.so dotnet run
```

For full local runtime use the Compose profiles - see [WSL2 Runtime Channels](wsl2-runtime-channels.md).
