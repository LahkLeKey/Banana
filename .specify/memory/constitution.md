<!--
Sync Impact Report
- Version change: 1.13.3 -> 1.13.4
- Modified principles:
	- Added XXIII. Banana-Derived Procedural Primitive Exclusivity
	- Added XXIV. Domain-Contract Test Decomposition
- Added workflow guidance: explicit Spec Kit orchestration preflight steps and command sequence
- Added workflow guidance: WASM rebuild + hot-reload contract for ARPG/runtime viewport fixes
- Templates requiring updates:
	- ✅ updated .specify/templates/plan-template.md (domain-contract test decomposition)
	- ✅ updated .specify/templates/spec-template.md (test organization guidance)
	- ✅ updated .specify/templates/tasks-template.md (bounded-suite native test guidance)

Previous sync:
- Version change: 1.10.1 -> 1.10.2
- Modified principles:
	- Added X. Observable Infrastructure as Code
	- Added XI. Spec-Driven Infrastructure Discovery
	- Added XII. Infrastructure Blockers Explicit
	- Added XIII. Performance Budgets as Code
	- Added XIV. Health Checks and Diagnostics
-	- Added XVIII. Confidence-Gated Clarification
	- Added XVIII. Confidence-Gated Clarification
	- Added XIX. Unified Monorepo Workflow Harness (DDD + SOLID)
- Added XV. Deployment Readiness as Code
- Added XVI. Memory-First Bootstrapping Efficiency
- Added XVII. Feedback-Loop Promotion
- Added platform constraints: health checks, performance budgets, CLI scriptability, metrics/logs
- Expanded platform constraints: production deploy proof, host inventory proof, capacity guardrails
- Expanded platform constraints: persistent spec-drain state and reusable inventory snapshots
- Added workflow guidance: infrastructure-dependent specs, runbook documentation, performance budget justification
- Added workflow guidance: deployment evidence artifact requirements for Vercel/Fly state
- Added workflow guidance: checkpoint-first scan discipline to avoid repeated full rescans
- Added workflow guidance: deferred-ledger closure when remaining specs are blocked/research-only
- Added workflow guidance: mandatory targeted Q/A when confidence in the next code-improving step is below 70%
- Added workflow guidance: single `Banana-Monorepo` harness requirement for all managed lanes
- Added workflow guidance: canonical harness file path `.github/workflows/banana.yml`
- Templates requiring updates:
	- ✅ updated .specify/templates/plan-template.md
	- ✅ compatible .specify/templates/spec-template.md
	- ✅ compatible .specify/templates/tasks-template.md
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

For **web portal work** (React, Electron, any page/component that renders in a browser), the VS Code Simple Browser must be opened at the relevant local or Vercel preview URL **at the start of work**, not only at task closure. The workflow is: open Simple Browser → make changes → hot-reload confirms the change → validate → close task. For Vercel-deployed portals (`banana.engineer`, preview URLs) the browser tab should also be opened to confirm the production or preview deployment renders correctly before marking any deploy-related task done.

### VII. Cross-API Feature Parity
When API surfaces expose overlapping capability areas (for example route modules, runtime channels, or compatibility layers), behavior parity is mandatory. Route availability, status semantics, and response-shape contracts must stay aligned or be explicitly documented as intentional divergence in the active spec before merge.

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

### XV. Deployment Readiness as Code
Deployment setup is not considered complete until it is both configured and actively serving a production deployment. For Banana this means:
- Vercel domain configuration must be paired with at least one successful production deployment (a configured domain with "No Deployment" is incomplete).
- Host inventory and capacity state (Vercel projects/deployments/domains and Fly apps/machine limits) must be captured via CLI and written to repository artifacts.
- Deployment requirements must be represented in scripts/runbooks, not only dashboard screenshots.
- Any blocked deployment state must include explicit remediation steps and an owner in the active spec artifacts.

### XVI. Memory-First Bootstrapping Efficiency
Spec-drain and infrastructure scanning workflows must prioritize persistent state reuse over repeated full computation. For Banana this means:
- Full scans must write reusable checkpoint/state artifacts before reruns are attempted.
- Reruns should resume from checkpoint and delta inputs whenever possible, not restart full discovery by default.
- Inventory snapshots used for gating and planning must be machine-readable and retained under repository artifacts.
- Constitution and runbook updates should codify newly observed scan bottlenecks as reusable bootstrap guidance.

### XVII. Feedback-Loop Promotion
Operational and delivery signals must be promoted into explicit, scored follow-up slices so improvement work is continuous and auditable. For Banana this means:
- Each completed drain cycle should emit promotion candidates derived from failures, deferred blockers, and observed UX/runtime gaps.
- Promotion candidates must include value, risk, and dependency-unlock scoring plus a clear owner.
- Promoted items should become explicit Spec Kit slices (spec/tasks) rather than remaining implicit notes.
- Promotion artifacts must be machine-readable and linked to the originating checkpoint evidence.

### XVIII. Confidence-Gated Clarification
Spec Kit work must not guess through ambiguity. When the operator is below 80% confidence that the next edit, command, or workflow change will improve the codebase, the operator MUST stop and ask targeted Q/A before proceeding. The questions must be decision-driving, narrow the uncertainty that blocks progress, and be recorded in the active spec or plan when they change scope, constraints, or acceptance expectations.

### XXII. Confidence Heartbeat Orchestration
Spec Kit orchestration must maintain an explicit confidence heartbeat throughout execution:
- each orchestration step records current confidence as a percentage,
- confidence must remain at or above 80% to continue autonomously,
- any below-threshold step requires a human-input checkpoint before execution continues,
- heartbeat evidence must be captured in feature artifacts (plan/tasks/evidence notes) for auditability.

### XXIII. Banana-Derived Procedural Primitive Exclusivity
Procedural-generation geometry in Banana runtime surfaces must derive from a banana primitive baseline. Classical primitive constructors (cube, sphere, plane) are not allowed as first-class generation sources in engine/runtime pathways. When block-like or orb-like structures are required, they must be generated through banana-derived vector profiles and documented as such in active specs.

### XXIV. Domain-Contract Test Decomposition
Native and engine test surfaces must be organized by bounded domain contracts, not timeline phases. Test targets, files, and suites should use domain names that communicate responsibility (for example physics, AI FSM, navigation, controller, render, world/signals) and should avoid ambiguous phase labels. Each suite must own one primary responsibility and compose through a thin orchestrator target for aggregate execution.

### XIX. Unified Monorepo Workflow Harness (DDD + SOLID)
Banana-managed CI/CD lanes must be orchestrated through a single workflow harness named `Banana-Monorepo`. Workflow design must follow domain-driven decomposition and SOLID-style responsibilities:
- each lane/job maps to a bounded domain context,
- each job has one clear responsibility,
- new lanes extend the harness without creating parallel top-level workflow sprawl,
- lane contracts remain stable and composable for the terminal pass/fail summary.
The canonical harness file is `.github/workflows/banana.yml`.

### XX. Zero-Warning CI Discipline
CI pipeline warnings and informational notices are treated as errors by QA agents and must be resolved before a branch can be considered clean. This means:
- **Deprecation warnings** (e.g., action runtime version notices) must be eliminated by pinning the affected action or dependency to a supported runtime within the same delivery slice that surfaces them — they are never left as acceptable noise.
- **Informational notices** emitted by CI tooling or validation scripts (e.g., missing avatar mappings, unresolved agent identities, incomplete configurations) must be resolved or explicitly deferred in the active spec with a tracked follow-up task. Silent deferral is not permitted.
- A CI run that emits warnings or notices is not considered pristine; it is treated as having a partial failure by QA agents even when all job conclusions are `success`.
- When a warning is structurally unavoidable (e.g., a third-party dependency that cannot yet be updated), it must be documented in an `exceptions/ci-warning-suppressions.md` file with an expiry date, owner, and remediation plan. Undocumented suppressions are violations.
- QA agents reviewing a PR must explicitly enumerate all warnings and notices present in the CI run output and confirm each is either resolved or has a tracked exception before approving.

### XXI. Domain-Driven SOLID Implementation Architecture
Implementation inside each Banana bounded context must follow domain-driven decomposition and SOLID responsibilities, not monolithic utility blobs. For native procedural generation and similar engine/tooling work this means:
- domain model/rules are isolated from orchestration and file I/O,
- application services coordinate algorithms but do not own persistence/serialization concerns,
- infrastructure adapters handle serialization, filesystem, and process boundaries,
- algorithm modules are replaceable via stable contracts (for example WFC, cellular automata, or hybrid pipelines) without rewriting unrelated layers,
- each source unit has one clear responsibility and depends on abstractions/contracts where practical.

## Platform Constraints

- Use `BANANA_PG_CONNECTION` whenever PostgreSQL-backed native and integration paths are exercised.
- Keep `BANANA_NATIVE_PATH` explicit for TypeScript API runtime wiring.
- Preserve `VITE_BANANA_API_BASE_URL` as the frontend API base URL contract.
- Preserve feature parity expectations across TypeScript API runtime channels for overlapping route contracts used by frontend/runtime channels.
- Preserve Bun as package manager for TypeScript React/Electron/mobile domains.
- Keep runtime-channel behavior aligned with the Windows + Docker Desktop + Ubuntu WSL2 contract.
- Treat `banana.engineer` as the production hostname; keep `VITE_BANANA_API_BASE_URL` pointing to the production API and never hardcode localhost in production builds.
- Preserve Vercel as the frontend hosting platform; `vercel.json` at the repository root is the authoritative deployment manifest.
- Health check endpoints (`/health`, `/diagnostics`) must respond within 100ms and include database connectivity status.
- Performance budgets must be enforced in CI; default thresholds: JS bundle < 150 KB gzip, LCP < 2500ms, CLS < 0.1.
- All infrastructure configuration (redirects, deploy hooks, SSL certs) must be scriptable via CLI tools and documented in runbooks.
- Metrics and logs must be queryable via CLI (`vercel logs`, `vercel metrics`, PostgreSQL `pg_stat_statements`) for operational debugging.
- Vercel production domains are valid only when a production deployment exists and is reachable.
- Deployment inventory must include CLI evidence for `vercel projects ls`, `vercel ls <project>`, and `vercel domains ls`.
- Fly.io capacity guardrails must be tracked from CLI outputs (`fly apps list`, machine limits/current usage) before provisioning new runtime surfaces.
- Host-level blocked states (for example, domain configured but no production deploy) must be treated as delivery blockers until remediated or explicitly deferred.
- Spec-drain automation must persist and reuse checkpoint state files (for example under `artifacts/sdlc-orchestration/`) instead of requiring full rescans per run.
- Bootstrap inventory snapshots must remain machine-readable under `artifacts/orchestration/` so follow-up scans can operate incrementally.
- CI warnings and informational notices are treated as errors; a run with any unresolved warning or notice is not considered a clean baseline.
- Structurally unavoidable warning suppressions must be documented in `exceptions/ci-warning-suppressions.md` with an expiry date and owner.
- New native/tooling features must be organized into domain/application/infrastructure slices (or equivalent bounded modules) before complexity exceeds a single-file prototype.
- Runtime procedural geometry must use banana-derived primitive adapters only; classical primitive constructors are disallowed for new engine/proc-gen work.
- Native test targets and files must use domain-contract naming and bounded suite ownership; phase-oriented labels are not permitted for new test surfaces.

## Workflow and Review

- Scope changes to the owning domain unless a cross-layer contract actually changes.
- Prefer existing entry points in `scripts/`, workspace tasks, CMake targets, Bun scripts, and compose profiles.
- Keep automation pull requests on provenance labels including `speckit-driven` and required-check workflows.
- Run `scripts/validate-ai-contracts.py` whenever AI workflow contracts (prompts, agents, skills, instructions, workflows, wiki mapping) are touched.
- Run `scripts/workflow-sync-wiki.sh` in the same change when contract surfaces move.
- For frontend and Electron behavior checks, open the VS Code Simple Browser at the relevant local or Vercel preview URL **before starting work** on any web portal change (React page, component, layout, navigation). Keep it open for hot-reload feedback during implementation and verify rendered state against the active runtime profile before marking tasks done.
- For native engine changes that affect React-delivered WASM runtime behavior (camera, terrain, renderer, input bridge), rebuild and publish updated WASM artifacts in the same slice. Canonical rebuild command on Windows/Git Bash is:
	`MSYS_NO_PATHCONV=1 docker run --rm -v "/c/Github/Banana:/workspace" -w /workspace emscripten/emsdk:3.1.57 bash -lc "bash scripts/build-engine-wasm.sh --release"`
- When WASM artifacts are rebuilt, commit updated `src/typescript/react/public/wasm/engine.js` and `src/typescript/react/public/wasm/engine.wasm` together so preview/prod consume the same native behavior.
- React runtime iteration must support hot reload for WASM bundle updates without restarting compose/docker channels; dev workflows should rely on browser refresh or in-page auto-reload watchers after artifact rebuild.
- When a change touches overlapping API capability across runtime channels, include parity validation evidence (or intentional-drift rationale) in the active Spec Kit artifacts before task closure.
- For checks that call GitHub REST/GraphQL or action metadata APIs, include explicit rate-limit handling and a documented rerun path in the active spec artifacts before task closure.
- For changes that touch the Vercel deployment manifest (`vercel.json`), the Vite build config, or `VITE_BANANA_API_BASE_URL` wiring, validate a clean `bun run build` and confirm the production environment contract before task closure.
- When implementing a spec marked `Ready for research` (infrastructure-dependent), confirm live infrastructure prerequisites are available before beginning work. If prerequisites are unavailable, flag the spec as blocked and continue with other ready specs.
- For specs that expose new CLI commands or infrastructure patterns, document the corresponding runbook in `docs/runbooks/` in the same delivery slice.
- Health check and diagnostic endpoints must be tested in both local (`npm run dev`) and CI contexts before task closure.
- Performance budget changes must include explicit justification (e.g., "LCP threshold increased to 3s: mobile network analysis shows 2.5s target breaks on 4G/3G"). Budget increases require team approval via code review; decreases are always welcome.
- Deployment-related specs must emit a machine-readable host-inventory artifact in the same slice (for example under `artifacts/orchestration/deployment-inventory/`).
- Vercel onboarding/repair work must include proof of both configuration and active production deployment, plus remediation steps when deployment status is error or missing.
- Multi-host deployment decisions (Vercel + Fly/Railway) must include explicit capacity and suspension-state checks before adding new machines/apps.
- Before running repository-wide scan loops, operators must attempt checkpoint resume first and document why a full rescan is required if resume is not used.
- Scan loops that complete should publish a compact summary artifact that can be consumed by later planning and constitution updates.
- When no runnable specs remain, operators should emit a deferred-ledger summary that accounts for blocked/research specs without forcing a full recomputation loop.
- After checkpoint accounting, operators should emit a promotion ledger capturing high-value follow-up slices and route those items into new specs.
- When confidence that the next action will improve the codebase falls below 80%, pause implementation and ask focused Q/A before editing or dispatching broad workflow changes. Reversible probes are acceptable only after that clarification step or when the human explicitly requests exploratory work.
- Keep all managed CI stages under `Banana-Monorepo`; do not introduce additional top-level workflow files for stage-specific orchestration unless explicitly approved as an exception in the active spec.
- After every CI run, enumerate all warnings and notices in the run output. Any unresolved warning or notice must be fixed in the same branch or filed as a tracked follow-up task with an expiry before the branch is considered clean.
- When a CI action emits a runtime deprecation warning, pin the action to a Node 24-native version in the same slice that surfaces the warning — do not defer to a separate cleanup PR.
- For procedural-generation and engine-adjacent tooling, require explicit contracts between algorithm engines, configuration parsing, and output serializers so alternate algorithms can be introduced without broad rewrites.
- For procedural-generation mesh work, review evidence must show banana-derived primitive usage (orbanana vector adapters) rather than direct cube/sphere/plane builders.
- For native test refactors, reviewers must verify that test structure mirrors domain boundaries and SOLID ownership (single-responsibility suite files + explicit orchestrator).
- Before Spec Kit orchestration runs, extension health must be verified (`specify extension list`) and corrupted/disabled required extensions must be remediated or explicitly deferred with owner and expiry.

### Spec Kit Orchestration Preflight

Run this preflight before each Spec Kit orchestration run:

1. Extension health preflight (update-first):
	`.specify/scripts/bash/spec-extension-preflight.sh --update-first --json`
2. Confidence startup gate with heartbeat evidence:
	`.specify/scripts/bash/spec-confidence-gate.sh --confidence <n> --step "go-copilot-start" --notes "startup gate"`
3. Continue only when confidence is `>= 80%`; when confidence is `< 80%`, pause and request human input before any further orchestration step.
4. Resolve active feature from `.specify/feature.json` before selecting tasks.
5. Append heartbeat evidence after each major orchestration step and before/after validator runs.

## Governance

This constitution governs Spec Kit driven development and automation workflows in Banana.
Amendments require a pull request that documents rationale, migration impact, and validation updates.
Reviewers should block merges that violate these principles.

**Version**: 1.13.4 | **Ratified**: 2026-04-24 | **Last Amended**: 2026-05-12
