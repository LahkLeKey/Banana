# Domain Management

## Redirect Strategy

The production apex domain is `banana.engineer`. The `www.banana.engineer` host should permanently redirect to the apex domain so bookmarks, certificates, and analytics stay consolidated.

## Vercel CLI Workflow

Create or inspect the redirect with:

```bash
vercel redirects add www.banana.engineer banana.engineer
vercel redirects list
```

## Verification

Verify the redirect from a shell:

```bash
curl -I https://www.banana.engineer
curl -IL https://www.banana.engineer
```

Expected behavior:

- Initial response is `301` or `308`
- `Location` header points to `https://banana.engineer`
- Final resolved URL is the apex domain with no redirect loop

## Rollback

If the redirect is misconfigured:

1. Remove the redirect entry from Vercel.
2. Re-run `vercel redirects list` to confirm it is gone.
3. Re-test `curl -I https://www.banana.engineer` after the config change propagates.
