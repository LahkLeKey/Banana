# Deploy Hooks

## Purpose

Deploy hooks allow automation or external systems to trigger a Vercel deployment without pushing a new Git commit.

## Create Hook

Create a deploy hook for the main branch:

```bash
vercel deploy-hooks create main
```

Store the returned URL in your secret manager. Do not commit it to the repository.

## Trigger Hook

Trigger a deployment manually:

```bash
curl -X POST "$VERCEL_DEPLOY_HOOK_URL"
```

## Verification

After triggering the hook:

1. Inspect the deployment in the Vercel dashboard.
2. Confirm the target branch and deployment timestamp are correct.
3. Confirm the deployment completes without build or runtime errors.

## Rotation

Rotate the hook when it is exposed, shared too broadly, or included in screenshots/logs:

1. Create a new hook.
2. Update the secret store and dependent automation.
3. Delete the old hook from Vercel.
4. Trigger one test deployment with the new hook.
