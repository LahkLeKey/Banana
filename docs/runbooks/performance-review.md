# Performance Review Runbook

## Overview

This runbook defines the weekly process for reviewing Core Web Vitals (CWV)
on `banana.engineer` using Vercel Speed Insights and Lighthouse CI.

## Weekly Review Steps

1. Open the Vercel Dashboard → Select the `banana` project → Click **Speed Insights**.
2. Check the **P75 values** for:
   - **LCP** (Largest Contentful Paint) — target: ≤ 2.5 s
   - **CLS** (Cumulative Layout Shift) — target: ≤ 0.1
   - **INP** (Interaction to Next Paint) — target: ≤ 200 ms
3. Compare to the baseline in [lighthouse-baseline.md](../lighthouse-baseline.md).
4. If any P75 value is in the **red** bucket (LCP > 4 s, CLS > 0.25, INP > 500 ms):
   - Open a GitHub issue titled `Perf regression: <metric> P75 = <value>` tagged `performance`.
   - Link to the Speed Insights screenshot.
5. Record the week's values in the baseline document.

## Regression Threshold

| Metric | Warning | Error (open issue) |
|---|---|---|
| LCP P75 | > 2.5 s | > 3.0 s |
| CLS P75 | > 0.1 | > 0.25 |
| INP P75 | > 200 ms | > 500 ms |

## Lighthouse CI Results

Per-PR Lighthouse CI results are uploaded as GitHub Actions artifacts
(`.lighthouseci/` directory). To review:

1. Go to the PR's **Checks** tab.
2. Find the `lighthouse-ci` job.
3. Download the `lighthouse-report` artifact and open `report.html`.

## Opening a Regression Issue

Use the `open-cwv-regression-issue.yml` workflow dispatch if you need to
manually track a regression:

```bash
gh workflow run open-cwv-regression-issue.yml
```

## Useful Links

- Vercel Speed Insights: <https://vercel.com/dashboard> → Speed Insights
- Vercel Analytics: <https://vercel.com/dashboard> → Analytics
- web.dev CWV reference: <https://web.dev/vitals/>
