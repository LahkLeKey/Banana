# Plan: Vercel Deployment Rollback and Recovery (153)

## Design Phase

Rollback restores a previous deployment. Procedures:
1. Identify broken deployment ID.
2. List previous deployments: `vercel deployments --limit 10`.
3. Rollback: `vercel rollback --to <good-deployment-id>`.
4. Verify: health check, metrics.

## Success Criteria

- Rollback command executes successfully.
- Previous version restored and verified.
- Runbook accessible to on-call engineer.
