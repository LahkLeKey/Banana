# Tasks: Cache Headers and CDN Strategy (135)

**Input**: Spec at `.specify/specs/135-cache-headers-cdn/spec.md`
**Prerequisites**: Spec 123 (banana.engineer domain active); spec 121 (vercel.json hardened)

- [x] T001 Add a `no-cache` header entry in `vercel.json` for `source: "/index.html"` alongside the existing asset `immutable` entry.
- [x] T002 Validate `vercel.json`: `python3 -c "import json; json.load(open('vercel.json')); print('OK')"`.
- [x] T003 Deploy to Vercel preview and confirm with `curl -I <preview-url>/index.html | grep cache-control` that `no-cache` is returned.
- [x] T004 Confirm hashed assets still return `Cache-Control: public, max-age=31536000, immutable` with `curl -I <preview-url>/assets/index-*.js`.
- [x] T005 Document cache invalidation behavior in `docs/storybook.md` → `docs/deployment.md` (or a new `docs/deployment.md`).
- [x] T006 Run `python scripts/validate-spec-tasks-parity.py .specify/specs/135-cache-headers-cdn` and confirm `OK`.
