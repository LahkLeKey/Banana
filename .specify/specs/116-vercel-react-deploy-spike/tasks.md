# Tasks: Vercel React Deploy Spike (116)

**Input**: Spec at `.specify/specs/116-vercel-react-deploy-spike/spec.md`
**Prerequisites**: `banana.engineer` domain purchased; Vercel account active; `vercel.json` created at repo root

## Phase 1: Build Verification

- [x] T001 Run `python3 -c "import json; json.load(open('vercel.json')); print('OK')"` and confirm `vercel.json` is valid JSON.
- [x] T002 Run `cd src/typescript/react && bun run build` and confirm exit 0, capturing build time and bundle sizes.
- [x] T003 Install Vercel CLI (`bun add -g vercel`) and run `vercel --version` to confirm availability.

## Phase 2: Vercel Project Wiring

- [x] T004 Run `vercel link` in the repo root to associate the repository with a Vercel project (or create one named `banana`).
- [x] T005 Run `vercel env add VITE_BANANA_API_BASE_URL` for each environment tier (Development, Preview, Production) and document the values used.
- [x] T006 Run `vercel deploy --prebuilt` or `vercel deploy` from the repo root and capture the preview URL.
- [x] T007 Open the preview URL in a browser and confirm the React app loads, the title is correct, and deep-link routes (`/classify`) do not 404.

## Phase 3: Domain and SPA Rewrite

- [x] T008 Add `banana.engineer` as a production domain in the Vercel project dashboard and note the required DNS records (A or CNAME).
- [x] T009 Confirm that the `rewrites` in `vercel.json` (`/* → /index.html`) resolve SPA 404s by navigating directly to a deep path on the preview URL.
- [x] T010 Confirm `Cache-Control: immutable` is returned for a hashed asset (e.g., `dist/assets/index-*.js`) using `curl -I`.

## Phase 4: Research Documentation

- [x] T011 Write `research.md` in `.specify/specs/116-vercel-react-deploy-spike/` with: go/no-go verdict, build command confirmed, preview URL, domain wiring status, and open questions for specs 121–124.
- [x] T012 Update `.specify/specs/121-vercel-build-config/spec.md` status to "Ready for implementation" if go.
- [x] T013 Run `python scripts/validate-spec-tasks-parity.py .specify/specs/116-vercel-react-deploy-spike` and confirm `OK`.
