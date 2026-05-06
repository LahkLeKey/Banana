# Research: Performance and CDN Spike (119)

**Date**: 2026-05-02
**Verdict**: ✅ GO — all optimizations implemented or confirmed no-op

## Bundle Analysis (spec 119 T003)

### Top-5 chunks by gzip size (measured 2026-05-02 from `bun run build`):

| Chunk | Raw | Gzip | Notes |
|---|---|---|---|
| `index-DLIJkkKK.js` | 214.03 KB | **67.98 KB** | App code — changes every deploy |
| `router-DfW6Fj96.js` | 101.61 KB | **34.02 KB** | react-router-dom — stable, cached |
| `query-BNWLMHzH.js` | 28.30 KB | **8.64 KB** | @tanstack/react-query |
| `ClassifyPage-xvs1-zhr.js` | 23.88 KB | **7.90 KB** | Lazy-loaded route chunk |
| `web-vitals-By3A_XPg.js` | 5.89 KB | **2.25 KB** | Vercel Speed Insights |

Total first-load transfer (gzip): ~122 KB JS + 3.62 KB CSS = ~126 KB.  
Repeat visits (vendor chunks cached): ~68 KB (index only).  
**Cache-hit reduction: ~46% on repeat visits.**

### Before (baseline — estimated from pre-split build):

- Single JS chunk: ~360 KB raw / ~118 KB gzip (all deps + app code bundled together)

### After (manualChunks vendor split — spec 133):

Vite `rollupOptions.output.manualChunks` extracts:
- `vendor`: react + react-dom — 0.03 KB raw (tree-shaken into index; chunk is tiny stub)
- `router`: react-router-dom — 101.61 KB raw / 34.02 KB gzip (stable, cached)
- `radix`: @radix-ui/react-slot — 2.73 KB raw / 1.30 KB gzip
- `query`: @tanstack/react-query — 28.30 KB raw / 8.64 KB gzip
- `index`: app code — 214.03 KB raw / 67.98 KB gzip (only chunk that changes per deploy)

**Result**: On repeat visits only the `index` chunk (~68 KB gzip) is re-fetched.
Router, query, and radix chunks are served from browser cache.  
Estimated repeat-visit savings: ~43 KB gzip vs a monolithic bundle.

## React Lazy-Loading Evaluation (spec 119 T004)

`src/typescript/react/src/lib/router.tsx` already uses `React.lazy` + `Suspense` for all three routes:
- `ClassifyPage` → lazy chunk `ClassifyPage-xvs1-zhr.js` (7.90 KB gzip)
- `OperatorPage` → lazy chunk `OperatorPage-Bwmp5NsP.js` (0.31 KB gzip)
- `NotFoundPage` → lazy chunk `NotFoundPage-DYACsxyw.js` (0.35 KB gzip)

All route chunks excluded from the initial JS payload. ✅ No additional work needed.

## manualChunks Evaluation (spec 119 T005)

`vite.config.ts` `build.rollupOptions.output.manualChunks` already extracts `vendor`, `router`, `radix`, `query`.
See Bundle Analysis section above for measured sizes. ✅ Implemented.

## Font Loading (spec 134)

Audit of `src/typescript/react/src/index.css` and `index.html`:
- No Google Fonts CDN calls (`fonts.googleapis.com`) found.
- `@banana/ui/tokens/web.css` is self-hosted via the shared UI package.
- No render-blocking font resources. ✅ No action needed.

## Vercel Image Optimization (spec 119 T007)

No user-supplied image URLs in the current classify flow. The classify route submits an image as a multipart file upload to the ASP.NET API, not a URL. Vercel Image Optimization (`/_vercel/image`) operates on `<img src>` URLs and is not applicable to the file-upload flow.  
Revisit if a banana image gallery or URL-based classify UX is added in a future spec.

## Cache Headers (spec 135)

`vercel.json` confirmed:
- `/index.html`: `no-cache, no-store, must-revalidate` — SPA shell is always fresh.
- `/assets/*`: `public, max-age=31536000, immutable` — hashed assets cached 1 year.
- `/*`: security headers (X-Content-Type-Options, X-Frame-Options, Referrer-Policy).

## Vercel Image Optimization (spec 136 scope)

No user-supplied image URLs in the current classify flow. Vercel Image Optimization not
required for v1. Revisit if banana image upload UX is added.

## Lighthouse CI Gate

`lighthouserc.json` configured with LCP ≤ 2.5 s and CLS ≤ 0.1 error thresholds.
`@lhci/cli` installed. Lighthouse CI job wired in `vercel-deploy.yml`.

## Open Questions for Specs 133–136

- 133: manualChunks implemented in `vite.config.ts`. ✅
- 134: No external fonts found; no action needed. ✅
- 135: Cache headers in `vercel.json` confirmed correct. ✅
- 136: Lighthouse CI gate wired; baseline to be captured post-deploy.
