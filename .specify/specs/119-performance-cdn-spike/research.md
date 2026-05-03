# Research: Performance and CDN Spike (119)

**Date**: 2026-05-02
**Verdict**: ✅ GO — all optimizations implemented or confirmed no-op

## Bundle Analysis

### Before (baseline — estimated from pre-spec build):

- Single JS chunk: ~360 KB (all deps + app code)

### After (manualChunks vendor split — spec 133):

Vite `rollupOptions.output.manualChunks` extracts:
- `vendor`: react + react-dom — ~140 KB (cached long-term)
- `router`: react-router-dom — ~25 KB
- `radix`: @radix-ui/react-slot — ~4 KB
- `query`: @tanstack/react-query — ~35 KB
- `index`: app code — ~160 KB (changes per deploy, not cached)

**Result**: Initial transfer ~160 KB (app chunk only on revisit); full first load ~364 KB total
(comparable to before, but repeat visits ~58% lighter due to vendor cache hits).

## Font Loading (spec 134)

Audit of `src/typescript/react/src/index.css` and `index.html`:
- No Google Fonts CDN calls (`fonts.googleapis.com`) found.
- `@banana/ui/tokens/web.css` is self-hosted via the shared UI package.
- No render-blocking font resources. ✅ No action needed.

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
