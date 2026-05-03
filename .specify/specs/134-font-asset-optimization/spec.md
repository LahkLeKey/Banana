# Feature Specification: Font and Asset Loading Optimization (134)

**Feature Branch**: `feature/134-font-asset-optimization`
**Created**: 2026-05-02
**Status**: Draft (pending spike 119 go verdict)
**Input**: Scaffolded from spike `119-performance-cdn-spike`

## In Scope

- Audit `index.css` and `tailwind.config.ts` for external font loads (Google Fonts CDN calls).
- Replace any remote Google Fonts with locally hosted, subsetted WOFF2 files using `fontsource` or a self-hosted copy.
- Add `font-display: swap` to all `@font-face` declarations.
- Confirm render-blocking font requests are eliminated (Lighthouse audit: no render-blocking resources).

## Out of Scope

- Bundle splitting (spec 133). Image optimization (spec 135).

## Success Criteria

```bash
cd src/typescript/react && bun run build
# No external font CDN calls in dist/
grep -r "fonts.googleapis.com" dist/ && echo "FAIL: still using Google CDN" || echo "PASS"
python scripts/validate-spec-tasks-parity.py .specify/specs/134-font-asset-optimization
```
