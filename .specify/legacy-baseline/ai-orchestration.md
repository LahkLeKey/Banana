# Legacy Baseline — AI Orchestration (Workflows + Prompts + Agents + Skills + Spec Kit)

**Paths**: `.github/workflows/`, `.github/{prompts,agents,skills,instructions}/`, `.specify/{workflows,specs,templates,memory,scripts,wiki}/`

## What exists

- **GitHub Actions** (`.github/workflows/`):
  `ai-contract-guard.yml`, `compose-ci.yml`, `copilot-review-triage.yml`,
  `orchestrate-autonomous-self-training-cycle.yml`, `orchestrate-banana-sdlc.yml`,
  `orchestrate-not-banana-feedback-loop.yml`, `orchestrate-triage-idea-cloud.yml`,
  `orchestrate-triaged-item-pr.yml`, `require-human-approval.yml`, `train-not-banana-model.yml`.
- **Prompts/agents/skills/instructions**: ~25 prompts, domain agents (`banana-sdlc`, `banana-planner`, `banana-reviewer`, native/api/react/electron/mobile/compose/workflow helper agents, value-risk, test-triage, technical-writer), domain skills (discovery/build-and-run/test-and-coverage/ci-debugging/release-readiness/agent-decomposition/mobile-runtime), instruction files per domain.
- **Spec Kit**: `.specify/specs/001..004`, `.specify/workflows/{drift-realignment,speckit}/`, `.specify/templates/`, `.specify/memory/constitution.md`, `.specify/wiki/` (canonical AI wiki + 12-file allowlist).
- **AI contract guard**: `scripts/validate-ai-contracts.py` enforced via workflow; checks prompt-wiki linkage and wiki allowlist + parity.
- **Copilot instructions**: `.github/copilot-instructions.md` + `.github/instructions/*.instructions.md`.

## Hard contracts

- `.specify/wiki/` is the canonical AI-consumable wiki store. `.wiki/` is the human-reference publication surface, frozen by `.specify/wiki/human-reference-allowlist.txt` (12 files).
- Workflow `github.actor != 'github-actions[bot]'` guards must include `github.event_name == 'schedule'` if cron is intended (cron runs as the bot).
- GH CLI label warnings → pre-create labels (`sdlc`, `feedback-loop`) or scope `BANANA_PR_LABELS` to existing labels.
- YAML in workflows: quote `${{ … }}` env values with `:` and quote input descriptions containing JSON-like snippets.
- Wiki updates flow through `scripts/workflow-sync-wiki.sh` and stay aligned with allowlist.
- Validator command of record: `python scripts/validate-ai-contracts.py`.

## What works

- Spec Kit templates + skills + agents give a usable agentic SDLC loop.
- Wiki freeze contract prevents AI slop leakage.
- Helper-agent decomposition keeps changes scoped.
- AI contract guard runs in CI and blocks parity drift.

## Vibe drift / pain points

- Workflow count is high; some orchestration workflows duplicate steps that could share composite actions.
- Prompts and skills overlap (e.g., discovery vs build-and-run); content boundaries blur.
- Many agents — choosing the narrowest helper is non-obvious without the decomposition skill.
- Spec Kit specs (001..004) are mixed-shape — earlier ones predate the current template.
- `.specify/integrations/` and `extensions/` are present but their roles are not fully documented.

## Cross-domain dependencies

- Touches every other domain via CI + automation.

## v1 entry points to preserve in v2

- `.specify/wiki/human-reference-allowlist.txt` freeze
- `scripts/validate-ai-contracts.py` validator behavior + exit semantics
- Constitution at `.specify/memory/constitution.md`
- Helper-agent decomposition pattern
- Workflow `github.event_name == 'schedule'` rule
