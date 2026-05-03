# Uptime Monitoring Runbook

## Overview

`banana.engineer` and `api.banana.engineer/health` are monitored via
UptimeRobot (free tier). Alerts fire by email when either endpoint is down
for more than 5 minutes.

## Monitors

| Monitor | URL | Check Interval | Alert Threshold |
|---|---|---|---|
| banana.engineer (web) | `https://banana.engineer` | 5 min | 1 failed check |
| API health | `https://api.banana.engineer/health` | 5 min | 1 failed check |

## Configuring UptimeRobot

1. Go to [UptimeRobot](https://uptimerobot.com) and sign in (or create a free account).
2. Click **Add New Monitor** → **HTTP(s)**.
3. Set **Friendly Name**: `banana.engineer` (or `Banana API`).
4. Set **URL** to the target URL above.
5. Set **Monitoring Interval**: 5 minutes.
6. Under **Alert Contacts**, add the team engineering email.
7. Click **Create Monitor**.
8. Repeat for the second monitor.

## Verifying Alerts Work

1. Temporarily set the monitor URL to `https://banana.engineer/intentional-404-test`.
2. Wait up to 5 minutes and confirm an alert email is received.
3. Restore the URL to the correct value.

## Adding New Monitors

When a new public endpoint is added (e.g., `status.banana.engineer`):

1. Follow the steps above for the new URL.
2. Add a row to the monitor table in this document.
3. Commit the updated runbook.

## Escalation Path

- **Level 1**: Alert email to engineering list (auto, within 5 min of outage).
- **Level 2**: If outage persists > 30 min, check Vercel Status page and API host dashboard.
- **Level 3**: Open a GitHub issue tagged `incident` and post in team Slack channel.
