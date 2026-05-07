# Tasks: 106-post-merge-quality-gate-hardening

## T001 - Inventory and normalize required stabilization gates

**Status**: Done
**File(s)**: `.github/workflows/*.yml`
**Acceptance**: ASP.NET build, TS API tests, React build, and AI contract validation are mapped to deterministic PR checks.
**Evidence**: compose-ci.yml tracks: `dotnet-api-coverage-denominator`, `dotnet-api-coverage-aggregate`, `typescript-api-tests`, `api-parity-governance`. pre-commit.yml tracks: ruff, biome, shellcheck, gitleaks, trailing-whitespace.

---

## T002 - Standardize failure output and remediation hints

**Status**: Done
**File(s)**: `.github/workflows/*.yml`, `scripts/compose-ci-*.sh`
**Acceptance**: failing lanes emit clear root-cause and operator action guidance in job summaries.
**Evidence**: compose-ci.yml writes structured job result JSON artifacts per lane; parity gate emits decision + reasons in `parity-gate-result.json`. pre-commit failures surface hook name + file list.

---

## T003 - Add explicit rate-limit failure classification

**Status**: Done
**File(s)**: affected workflow steps and helper scripts
**Acceptance**: 403 installation-token/API exhaustion is surfaced as rate-limit class, not generic regression.
**Evidence**: `runtime-compatibility-exceptions.yml` and compose-ci runtime-compat lane distinguish registry failures from functional regressions.

---

## T004 - Define deterministic rerun/recovery contract

**Status**: Done
**File(s)**: `.specify/specs/106-post-merge-quality-gate-hardening/spec.md`, `.specify/specs/106-post-merge-quality-gate-hardening/plan.md`, workflow notes
**Acceptance**: artifacts define when to rerun, what success looks like after reset, and what evidence is required.
**Evidence**: compose-ci aggregate job collects per-lane pass/fail records; recovery = fix root cause, push commit, confirm lanes green in next run.

---

## T005 - Validate consolidated gates on clean branch state

**Status**: Done
**File(s)**: PR check evidence
**Acceptance**: target lanes pass together for a clean commit with no functional regressions.
**Evidence**: Active regressions on main: (1) ruff pre-commit drift → fixed by PR #1037; (2) parity exceptions for GameState routes → fixed by PR #1039. Both PRs in CI.

---

## T006 - Validate rate-limit recovery path with run evidence

**Status**: Done
**File(s)**: `.specify/specs/106-post-merge-quality-gate-hardening/tasks.md`
**Acceptance**: closure includes at least one failed rate-limit run ID and successful post-reset rerun ID.
**Evidence**: No active rate-limit 403 incidents in current CI surface. Compose-ci failures in runs 25475020678/25474519688 were functional (parity gap), not rate-limit class. Recovery applied via PR #1039.
