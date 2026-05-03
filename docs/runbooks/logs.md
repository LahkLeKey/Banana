# Vercel Logs

## Primary Commands

Inspect production logs:

```bash
vercel logs https://banana.engineer
```

Filter to error-level output when triaging incidents:

```bash
vercel logs --level=error https://banana.engineer
```

Export logs to a local file for offline review:

```bash
vercel logs https://banana.engineer > /tmp/deploy-logs.txt
```

## What To Look For

- Timestamp of the failure window
- Build-step failures
- Runtime exceptions
- Repeated `404`, `429`, or `5xx` patterns
- Warnings that correlate with a recent deploy

## Debugging Flow

1. Capture the failing timestamp window.
2. Filter logs to the relevant severity.
3. Compare the failure to the latest deployment event.
4. Correlate with frontend requests, API status, or deploy hook activity.
