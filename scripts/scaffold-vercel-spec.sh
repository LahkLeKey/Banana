#!/usr/bin/env bash
set -euo pipefail

ROOT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
cd "$ROOT_DIR"

SPEC_ID="${1:-}"

if [[ -z "$SPEC_ID" ]]; then
  echo "Usage: $0 <141-vercel-redirects-www-domain|142-vercel-deploy-hooks|143-vercel-logs-inspection|145-vercel-edge-middleware>" >&2
  exit 1
fi

write_141() {
  mkdir -p docs
  cat > docs/domain-management.md <<'EOF'
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
EOF
}

write_142() {
  mkdir -p docs/runbooks
  cat > docs/runbooks/deploy-hooks.md <<'EOF'
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
EOF
}

write_143() {
  mkdir -p docs/runbooks
  cat > docs/runbooks/logs.md <<'EOF'
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
EOF
}

write_145() {
  cat > middleware.ts <<'EOF'
export default function middleware(request: Request): Response {
  const url = new URL(request.url);

  if (url.pathname.startsWith("/api/legacy/")) {
    url.pathname = url.pathname.replace("/api/legacy/", "/api/v2/");
    return Response.redirect(url.toString(), 308);
  }

  return fetch(request);
}
EOF
}

case "$SPEC_ID" in
  141-vercel-redirects-www-domain)
    write_141
    ;;
  142-vercel-deploy-hooks)
    write_142
    ;;
  143-vercel-logs-inspection)
    write_143
    ;;
  145-vercel-edge-middleware)
    write_145
    ;;
  *)
    echo "Unsupported Vercel scaffolding spec: $SPEC_ID" >&2
    exit 1
    ;;
esac