# Tasks: 105-wiki-contract-mirror-restabilization

## T001 - Reconcile wiki section index drift

**Status**: Done
**File(s)**: `.wiki/*/README.md`, `scripts/wiki-scaffold.sh`
**Acceptance**: `bash scripts/wiki-scaffold.sh --dry-run --validate` returns success after scaffold generation.
**Evidence**: `.wiki/operations/README.md` had drift; regenerated via `bash scripts/wiki-scaffold.sh`. `--validate` now returns `wiki-scaffold --validate: OK`.

---

## T002 - Reconcile allowlist and mirror parity

**Status**: Done
**File(s)**: `.specify/wiki/human-reference/**`, `.specify/wiki/human-reference-allowlist.txt`
**Acceptance**: `python scripts/validate-ai-contracts.py` reports zero issues and no missing wiki contract entries.
**Evidence**: `validate-ai-contracts.py` exits 0 with `"issues": []`.

---

## T003 - Validate wiki CI guardrails

**Status**: Done
**File(s)**: `.github/workflows/wiki-lint.yml`
**Acceptance**: workflow checks continue to assert both validator and scaffold consistency.
**Evidence**: wiki-lint.yml: 3 consecutive success runs (25352023251, 25350507467, 25350459644).

---

## T004 - Add rate-limit-aware triage path

**Status**: Done
**File(s)**: `.specify/specs/105-wiki-contract-mirror-restabilization/spec.md`, `.specify/specs/105-wiki-contract-mirror-restabilization/plan.md`
**Acceptance**: Docs distinguish wiki drift failures from transient 403 installation-token failures and define rerun steps.
**Evidence**: wiki-sync.sh handles transient 403 by retry; triage guidance is embedded in operations wiki section.

---

## T005 - Capture closure evidence

**Status**: Done
**File(s)**: `.specify/specs/105-wiki-contract-mirror-restabilization/tasks.md`
**Acceptance**: task closure references command outputs and run IDs for successful parity validation and any rate-limit recovery run.
**Evidence**: wiki scaffold: OK; AI contracts: exit 0, 0 issues; wiki-lint CI: 3 success runs (25352023251, 25350507467, 25350459644).
