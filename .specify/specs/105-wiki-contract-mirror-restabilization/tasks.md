# Tasks: 105-wiki-contract-mirror-restabilization

## T001 - Reconcile wiki section index drift

**Status**: Planned
**File(s)**: `.wiki/*/README.md`, `scripts/wiki-scaffold.sh`
**Acceptance**: `bash scripts/wiki-scaffold.sh --dry-run --validate` returns success after scaffold generation.

---

## T002 - Reconcile allowlist and mirror parity

**Status**: Planned
**File(s)**: `.specify/wiki/human-reference/**`, `.specify/wiki/human-reference-allowlist.txt`
**Acceptance**: `python scripts/validate-ai-contracts.py` reports zero issues and no missing wiki contract entries.

---

## T003 - Validate wiki CI guardrails

**Status**: Planned
**File(s)**: `.github/workflows/wiki-lint.yml`
**Acceptance**: workflow checks continue to assert both validator and scaffold consistency.

---

## T004 - Add rate-limit-aware triage path

**Status**: Planned
**File(s)**: `.specify/specs/105-wiki-contract-mirror-restabilization/spec.md`, `.specify/specs/105-wiki-contract-mirror-restabilization/plan.md`
**Acceptance**: Docs distinguish wiki drift failures from transient 403 installation-token failures and define rerun steps.

---

## T005 - Capture closure evidence

**Status**: Planned
**File(s)**: `.specify/specs/105-wiki-contract-mirror-restabilization/tasks.md`
**Acceptance**: task closure references command outputs and run IDs for successful parity validation and any rate-limit recovery run.
