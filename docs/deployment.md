# Deployment Guide

**Applies to**: `banana.engineer` (React SPA on Vercel) + `api.banana.engineer` (Fastify on Railway)

---

## React SPA — Vercel

### Build configuration

| Field | Value |
|---|---|
| Framework | Vite |
| Build command | `cd src/typescript/react && bun install && bun run build` |
| Output directory | `src/typescript/react/dist` |
| Node version | 20+ |

Configuration is authoritative in `vercel.json` at the repository root.

### Cache invalidation strategy (spec 135)

Vercel CDN serves all `dist/` assets. The strategy relies on two layers:

| Asset pattern | `Cache-Control` value | Rationale |
|---|---|---|
| `/index.html` | `no-cache` | Re-validates on every request; ensures users pick up new deploys immediately |
| `/assets/*.js`, `/assets/*.css` | `public, max-age=31536000, immutable` | Content-hashed by Vite; safe to cache for 1 year |

**How invalidation works**: Vite appends a content hash to every asset filename on each build (e.g., `vendor-C9RSz6Zg.js`). When code changes, the hash changes, the filename changes, and the browser fetches the new file — even though the old hash is still cached. Because `index.html` is `no-cache`, the browser always retrieves the latest HTML with the new asset filenames.

**Never purge the Vercel CDN manually** for routine deploys; the hash strategy handles cache busting automatically.

### Bundle chunks

| Chunk | Contents | Typical gzip size |
|---|---|---|
| `vendor` | `react`, `react-dom` | ~45 KB |
| `router` | `react-router-dom` | ~30 KB |
| `radix` | `@radix-ui/react-slot` | <1 KB |
| `query` | `@tanstack/react-query` | ~15 KB |
| `index` | App code | ~50 KB |

Chunks are configured in `src/typescript/react/vite.config.ts` via `build.rollupOptions.output.manualChunks`.

### Environment variables

| Variable | Tier | Required |
|---|---|---|
| `VITE_BANANA_API_BASE_URL` | All | Yes — production build throws if absent |
| `VITE_SENTRY_DSN` | Production | No — error reporting disabled when absent |
| `SENTRY_AUTH_TOKEN` | CI only | No — source maps skipped when absent |

Set per-tier in the Vercel dashboard (not in `.env` files committed to the repo).

---

## Fastify API — Railway

### Deploy flow

1. Push to `main` triggers `.github/workflows/api-deploy.yml`.
2. `prisma migrate deploy` runs against `DATABASE_URL` (= `BANANA_PG_CONNECTION`). On failure the deploy is skipped.
3. Railway CLI deploys the service.

### Required secrets (GitHub Actions environment `production`)

| Secret | Purpose |
|---|---|
| `BANANA_PG_CONNECTION` | PostgreSQL connection string |
| `RAILWAY_TOKEN` | Railway CLI authentication |
| `RAILWAY_SERVICE_ID` | Target Railway service |

### Migration rollback

See [docs/runbooks/db-migrations.md](runbooks/db-migrations.md).

---

## Promotion order

```
local (bun run dev) → PR preview (Vercel auto) → production (merge to main)
```

- **Local**: `VITE_BANANA_API_BASE_URL=http://localhost:8081`  
- **Preview**: `VITE_BANANA_API_BASE_URL=https://staging-api.banana.engineer` (Vercel Preview tier)  
- **Production**: `VITE_BANANA_API_BASE_URL=https://api.banana.engineer` (Vercel Production tier)
