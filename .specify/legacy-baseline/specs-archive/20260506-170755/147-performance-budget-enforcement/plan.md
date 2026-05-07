# Plan: Performance Budget Enforcement (147)

## Design Phase

### Budget Categories

1. **Bundle Size** (JavaScript):
   - vendor chunk < 80 KB
   - main chunk < 70 KB
   - total < 150 KB gzip

2. **CSS**:
   - < 50 KB gzip

3. **Core Web Vitals**:
   - LCP < 2.5s (from Lighthouse)
   - CLS < 0.1 (from Lighthouse)
   - FID < 100ms (deprecated; use INP)

4. **API Response Times**:
   - `/classify` < 500ms (p99)
   - `/health` < 50ms
   - Other routes < 1s

## Success Criteria

- Script correctly identifies over-budget bundles.
- CI job fails if budget exceeded.
- Team consensus on budget values documented.
