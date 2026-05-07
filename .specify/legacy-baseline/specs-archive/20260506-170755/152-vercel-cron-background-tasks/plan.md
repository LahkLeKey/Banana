# Plan: Vercel Cron Jobs for Background Tasks (152)

## Design Phase

Vercel cron invokes HTTP endpoints on a schedule. Use cases:
- **Daily model refresh**: retrain classifier nightly.
- **Cleanup**: delete expired logs, temporary files.
- **Health checks**: ping internal services daily.

## Success Criteria

- Cron endpoint defined in vercel.json.
- Cron executes at scheduled time in production.
- Cron errors logged and alerted.
