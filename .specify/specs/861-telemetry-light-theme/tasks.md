# Tasks: 861 Telemetry Light Theme

**Feature**: 861-telemetry-light-theme
**Branch**: `feat/861-telemetry-light-theme`
**Spec**: `.specify/specs/861-telemetry-light-theme/spec.md`
**Plan**: `.specify/specs/861-telemetry-light-theme/plan.md`

---

## T001 — Fix dark-hardcoded tooltip in TelemetryDashboardPage

**Status**: [x]
**Agent**: react-ui-agent
**Files**:
- `src/typescript/react/src/pages/TelemetryDashboardPage.tsx`

Replace `bg-slate-900 text-slate-50` bar-chart hover tooltip with semantic
light-first tokens (`border-border bg-background text-foreground shadow-sm`).

**Acceptance**: Page renders tooltip using semantic tokens; `bg-slate-900` absent
from rendered HTML.

---

## T002 — Add light-first regression test to TelemetryDashboardPage.test.tsx

**Status**: [x]
**Agent**: react-ui-agent
**Files**:
- `src/typescript/react/src/pages/TelemetryDashboardPage.test.tsx`

Add a bun test asserting the rendered DOM does not contain `bg-slate-900` and
does contain `bg-background`, confirming the tooltip uses semantic light tokens.

**Acceptance**: `bun test src/pages/TelemetryDashboardPage.test.tsx` — 3 pass.

---

## T003 — Create dashboard copy-policy checker script

**Status**: [x]
**Agent**: react-ui-agent
**Files**:
- `scripts/check-dashboard-copy-policy.ts`
- `scripts/dashboard-copy-policy-terms.json`

CLI Bun script that scans target dashboard source files for restricted vendor
brand terms. Skips import lines and comment lines. Exits 1 on violations in
strict mode (default), 0 in `--no-strict` audit mode. Exits 2 on fatal errors.
Terms file: JSON array of restricted strings (`ChatGPT`, `OpenAI`, `Anthropic`,
`Claude`, `Gemini`, `Copilot`, `LLaMA`, `Llama`, `Mistral`, `Grok`, `xAI`).

**Acceptance**:
- `bun scripts/check-dashboard-copy-policy.ts` exits 0 on clean repo.
- Script exits 1 when a seeded violation is injected.

---

## T004 — Add pytest tests for copy-policy checker

**Status**: [x]
**Agent**: test-triage-agent
**Files**:
- `tests/scripts/test_check_dashboard_copy_policy.py`

12 tests covering: compliant pass, repo default pass, JSX violation, multiple
violations, import/comment skip, non-strict mode, unknown arg, missing terms
file, bad JSON, missing source file.

**Acceptance**: `pytest tests/scripts/test_check_dashboard_copy_policy.py` — 12 passed.

---

## T005 — Wire copy-policy check into CI (analytics-scan job)

**Status**: [x]
**Agent**: workflow-agent
**Files**:
- `.github/workflows/pre-commit.yml`

Add `bun scripts/check-dashboard-copy-policy.ts` step to the existing
`analytics-scan` job in `pre-commit.yml`, and include
`test_check_dashboard_copy_policy.py` in the pytest invocation.

**Acceptance**: `analytics-scan` job in CI runs copy-policy check and tests;
failures surface as a CI gate before merge.
