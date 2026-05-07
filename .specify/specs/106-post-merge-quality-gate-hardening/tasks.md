# Tasks: 106-post-merge-quality-gate-hardening

## T001 - Inventory and normalize required stabilization gates

**Status**: Planned
**File(s)**: `.github/workflows/*.yml`
**Acceptance**: ASP.NET build, TS API tests, React build, and AI contract validation are mapped to deterministic PR checks.

---

## T002 - Standardize failure output and remediation hints

**Status**: Planned
**File(s)**: `.github/workflows/*.yml`, `scripts/compose-ci-*.sh`
**Acceptance**: failing lanes emit clear root-cause and operator action guidance in job summaries.

---

## T003 - Add explicit rate-limit failure classification

**Status**: Planned
**File(s)**: affected workflow steps and helper scripts
**Acceptance**: 403 installation-token/API exhaustion is surfaced as rate-limit class, not generic regression.

---

## T004 - Define deterministic rerun/recovery contract

**Status**: Planned
**File(s)**: `.specify/specs/106-post-merge-quality-gate-hardening/spec.md`, `.specify/specs/106-post-merge-quality-gate-hardening/plan.md`, workflow notes
**Acceptance**: artifacts define when to rerun, what success looks like after reset, and what evidence is required.

---

## T005 - Validate consolidated gates on clean branch state

**Status**: Planned
**File(s)**: PR check evidence
**Acceptance**: target lanes pass together for a clean commit with no functional regressions.

---

## T006 - Validate rate-limit recovery path with run evidence

**Status**: Planned
**File(s)**: `.specify/specs/106-post-merge-quality-gate-hardening/tasks.md`
**Acceptance**: closure includes at least one failed rate-limit run ID and successful post-reset rerun ID.
