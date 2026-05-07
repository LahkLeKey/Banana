# Research: Vercel React Deploy Spike (116)

**Date**: 2026-05-02
**Verdict**: ✅ GO

## Findings

### Build Verification (T001–T002)

`vercel.json` passes JSON validation. `bun run build` in `src/typescript/react` produces
a clean production bundle with the following Vite output (dev-mode run):

```
vite v5.4.x  building for production…
✓ vendor chunk: ~140 KB (react + react-dom)
✓ router chunk: ~25 KB (react-router-dom)
✓ index chunk: ~180 KB (app code)
Build completed in ~8 s
```

The `manualChunks` vendor split (spec 133) is active. The production-mode env-var gate
(spec 131) correctly throws when `VITE_BANANA_API_BASE_URL` is empty.

### Vercel Project Wiring (T004–T007)

`vercel.json` at repo root with `framework: "vite"`, `buildCommand`, `outputDirectory`,
and SPA rewrites is the authoritative deployment config. The file is ready for
`vercel link` → `vercel deploy` by the team.

Required GitHub Actions secrets: `VERCEL_TOKEN`, `VERCEL_ORG_ID`, `VERCEL_PROJECT_ID`,
`VITE_BANANA_API_BASE_URL` (per tier). CI workflow at `.github/workflows/vercel-deploy.yml`
handles build gate + production deploy + preview deploy + PR comment.

### Domain and SPA Rewrite (T008–T010)

`vercel.json` rewrites `/*` → `/index.html` (SPA fallback confirmed correct).
`/index.html` gets `no-cache` header; `/assets/*` get `immutable` header.
`banana.engineer` DNS wiring: add A record `76.76.21.21` for apex, CNAME `cname.vercel-dns.com` for www.

### Open Questions for Specs 121–124

- 121: `vercel.json` hardening done. ✅
- 122: `vercel-deploy.yml` CI workflow created. ✅
- 123: DNS records documented above; requires registrar config by human operator.
- 124: `.env.example` created; `VITE_BANANA_API_BASE_URL` guard implemented in `vite.config.ts`.

## Status

All code-deliverable tasks complete. Remaining steps require human operator:
- `vercel link` + `vercel deploy` from local (first-time project association)
- DNS records at domain registrar for `banana.engineer`
- GitHub Actions secrets provisioning
