<!--
Sync Impact Report
- Version change: 1.4.0 -> 1.5.0
- Modified principles:
	- Added X. Observable Infrastructure as Code
	- Added XI. Spec-Driven Infrastructure Discovery
	- Added XII. Infrastructure Blockers Explicit
	- Added XIII. Performance Budgets as Code
	- Added XIV. Health Checks and Diagnostics
- Added platform constraints: health checks, performance budgets, CLI scriptability, metrics/logs
- Added workflow guidance: infrastructure-dependent specs, runbook documentation, performance budget justification
- Templates requiring updates: none (all existing templates remain compatible)
- Follow-up TODOs:
	- Add performance budget template to .specify/templates/
	- Create infrastructure spec template for CLI-driven configuration
-->
# Banana Spec Kit Constitution

## Core Principles

### I. Domain Core First
All business behavior begins in native C under `src/native`; upper layers orchestrate, validate, and present results, but do not duplicate core domain rules.

### II. Layered Interop Contract
Managed flows must preserve the `controller -> service -> pipeline -> native interop` architecture. Cross-layer behavior changes require explicit contract updates in the touched layers.

### III. Spec First Delivery
AI-assisted work follows Spec Kit stages before implementation via the Specify CLI: `specify specify`, `specify plan`, and `specify tasks`. Emergency fixes may bypass this only with explicit human approval and a follow-up spec update.

### IV. Verifiable Quality Gates
Every change must run domain-appropriate validation before merge, including workflow-required checks for automation pull requests.

### V. Documentation and Wiki Parity
When workflow behavior, automation contracts, prompts, skills, or runbooks change, repository docs and wiki snapshots must be updated in the same SDLC slice.

### VI. One-Window Interactive Validation
Frontend and desktop UX changes must be validated in an interactive in-workspace surface before task closure. For Banana this means using the VS Code integrated browser/web view flow (with compose-backed runtime services) so implementation, diagnostics, and rendered behavior are reviewed in one window.

### VII. Cross-API Feature Parity
When ASP.NET and Fastify expose overlapping capability areas, behavior parity is mandatory. Route availability, status semantics, and response-shape contracts must stay aligned or be explicitly documented as intentional divergence in the active spec before merge.

### VIII. Rate-Limit Resilient Automation
Workflow gates that depend on GitHub APIs or marketplace action metadata must include a rate-limit resilience path. Required checks must avoid permanent false-negative failure caused solely by transient installation-token exhaustion and must provide deterministic recovery evidence through retries, backoff windows, rerun guidance, or non-blocking fallback classification.

### IX. Public Delivery Surface Contract
The canonical public delivery surface for Banana is **banana.engineer** served via Vercel. The React app (`src/typescript/react`) is the primary deployable artifact; its build output (`bun run build` → `dist/`) must remain Vercel-compatible at all times. Environment contracts must support three tiers — local (`.env.local`), preview (per-branch Vercel preview), and production (`banana.engineer`) — each bound by explicit `VITE_BANANA_API_BASE_URL` and backing API configuration. API backends exposed to `banana.engineer` must enforce CORS for that origin. No change may break the Vercel build or degrade the production site without an explicit rollback plan in the active spec.

### X. Observable Infrastructure as Code
Infrastructure configuration (Vercel redirects, deploy hooks, health checks, metrics queries, SSL certificates, database monitoring) must be:
- Defined and versioned in specs before manual configuration.
- Discoverable and scriptable via CLI tools (`vercel`, `railway`, etc.) and documented in runbooks.
- Verifiable through automated health checks, diagnostic endpoints, or integration tests in CI.
- Updated in the same delivery slice when infrastructure state changes.

### XI. Spec-Driven Infrastructure Discovery
When new platform capabilities are identified (e.g., via CLI analysis or documentation review), they must be scaffolded into specs before implementation. New specs must be marked `Ready for implementation` (if actionable without live infrastructure) or `Ready for research` (if requiring live accounts, production traffic, or external services). This pattern allows the spec-drainage automation to pick up discovered capabilities systematically without human intervention per feature.

### XII. Infrastructure Blockers Explicit
Specs that depend on live infrastructure (Vercel project, Railway deployment, DNS setup, production traffic, external SaaS accounts) must document their blockers in the spec's prerequisites and plan sections. Blocked specs shall not prevent the spec-drainage workflow from cascading downstream — i.e., dependent specs can proceed if they have alternate validation paths. Blocking specs must include a `[Blocker]` tag in plan.md and be tracked in the feature's research.md.

### XIII. Performance Budgets as Code
Frontend and API performance targets must be codified (e.g., in `bundlebudget.json`) and enforced via CI gates before merge. Performance regressions cannot be accepted without an explicit spec amendment and team review. Budgets must include thresholds for bundle size (JS/CSS/HTML gzip), Core Web Vitals (LCP, CLS), and API latency (p99 percentile). Budget enforcement is non-negotiable; failures must include actionable debugging guidance (e.g., "Build size increased 50 KB: compare bundle chunks at dist/assets/*.js").

### XIV. Health Checks and Diagnostics
Every deployable component (frontend, API, infrastructure service) must expose health-check and diagnostic endpoints (or equivalent monitoring surface) for automated health probing in CI/CD workflows and production. Minimum surface: `/health` (lightweight, fast) and `/diagnostics` (detailed system info, resource usage, latency metrics). Health check failures must be actionable — i.e., include specific reason (database offline, memory threshold exceeded, etc.) to guide rapid incident response.

## Platform Constraints

- Use `BANANA_PG_CONNECTION` whenever PostgreSQL-backed native and integration paths are exercised.
- Keep `BANANA_NATIVE_PATH` explicit for ASP.NET runtime wiring.
- Preserve `VITE_BANANA_API_BASE_URL` as the frontend API base URL contract.
- Preserve feature parity expectations across ASP.NET (`:8080`) and Fastify (`:8081`) for overlapping route contracts used by frontend/runtime channels.
- Preserve Bun as package manager for TypeScript React/Electron/mobile domains.
- Keep runtime-channel behavior aligned with the Windows + Docker Desktop + Ubuntu WSL2 contract.
- Treat `banana.engineer` as the production hostname; keep `VITE_BANANA_API_BASE_URL` pointing to the production API and never hardcode localhost in production builds.
- Preserve Vercel as the frontend hosting platform; `vercel.json` at the repository root is the authoritative deployment manifest.
- Health check endpoints (`/health`, `/diagnostics`) must respond within 100ms and include database connectivity status.
- Performance budgets must be enforced in CI; default thresholds: JS bundle < 150 KB gzip, LCP < 2500ms, CLS < 0.1.
- All infrastructure configuration (redirects, deploy hooks, SSL certs) must be scriptable via CLI tools and documented in runbooks.
- Metrics and logs must be queryable via CLI (`vercel logs`, `vercel metrics`, PostgreSQL `pg_stat_statements`) for operational debugging.

## Workflow and Review

- Scope changes to the owning domain unless a cross-layer contract actually changes.
- Prefer existing entry points in `scripts/`, workspace tasks, CMake targets, dotnet projects, Bun scripts, and compose profiles.
- Keep automation pull requests on provenance labels including `speckit-driven` and required-check workflows.
- Run `scripts/validate-ai-contracts.py` whenever AI workflow contracts (prompts, agents, skills, instructions, workflows, wiki mapping) are touched.
- Run `scripts/workflow-sync-wiki.sh` in the same change when contract surfaces move.
- For frontend and Electron behavior checks, verify rendered state in the VS Code integrated browser against the active runtime profile before marking tasks done.
- When a change touches overlapping ASP.NET and Fastify API capability, include parity validation evidence (or intentional-drift rationale) in the active Spec Kit artifacts before task closure.
- For checks that call GitHub REST/GraphQL or action metadata APIs, include explicit rate-limit handling and a documented rerun path in the active spec artifacts before task closure.
- For changes that touch the Vercel deployment manifest (`vercel.json`), the Vite build config, or `VITE_BANANA_API_BASE_URL` wiring, validate a clean `bun run build` and confirm the production environment contract before task closure.
- When implementing a spec marked `Ready for research` (infrastructure-dependent), confirm live infrastructure prerequisites are available before beginning work. If prerequisites are unavailable, flag the spec as blocked and continue with other ready specs.
- For specs that expose new CLI commands or infrastructure patterns, document the corresponding runbook in `docs/runbooks/` in the same delivery slice.
- Health check and diagnostic endpoints must be tested in both local (`npm run dev`) and CI contexts before task closure.
- Performance budget changes must include explicit justification (e.g., "LCP threshold increased to 3s: mobile network analysis shows 2.5s target breaks on 4G/3G"). Budget increases require team approval via code review; decreases are always welcome.

## Governance

This constitution governs Spec Kit driven development and automation workflows in Banana.
Amendments require a pull request that documents rationale, migration impact, and validation updates.
Reviewers should block merges that violate these principles.

**Version**: 1.5.0 | **Ratified**: 2026-04-24 | **Last Amended**: 2026-05-02
