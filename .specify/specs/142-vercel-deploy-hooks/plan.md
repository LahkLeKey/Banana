# Plan: Vercel Deploy Hooks for CI Integration (142)

## Design Phase

### Deploy Hook Purpose

A Vercel deploy hook is a webhook that triggers a new deployment without a Git push.
Use cases:
- **Cron-based rebuilds** (spec 120 potential follow-up): rebuild nightly to pick up model updates.
- **External event triggers**: rebuild when training completes or data updates.
- **Manual deployment** (override workflow): team can redeploy via webhook URL without GitHub.

### Security Implications

- **Webhook URL is sensitive** — treat as secret; do not commit to repo.
- **Rotating URL**: Generate new URL and retire old one if URL leaks.
- **Rate limiting**: Vercel applies default rate limits; monitor for abuse.

## Implementation Phase

### CLI Command

```bash
vercel deploy-hooks create main
# Outputs webhook URL
```

### Integration with GitHub Actions

GitHub Actions can call the webhook via `curl`:

```yaml
- name: Trigger Vercel Deploy Hook
  run: curl -X POST ${{ secrets.VERCEL_DEPLOY_HOOK }}
```

## Documentation Phase

- `docs/runbooks/deploy-hooks.md`: webhook purpose, secret management, rotation procedure.

## Monitoring Phase

- Log webhook calls in GitHub Actions.
- Monitor Vercel deployment logs for failures triggered by webhook.

## Success Criteria

- Webhook URL generated and accessible.
- POST to webhook triggers a build.
- No build failures introduced by webhook integration.
