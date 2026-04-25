# Implementation Plan: AI Orchestration (v2)

**Branch**: `013-ai-orchestration` | **Date**: 2026-04-25 | **Spec**: [spec.md](./spec.md)

## Technical Context

**Language/Version**: YAML (workflows), Markdown (prompts/agents/skills/instructions/specs), Python 3 (validator), Bash (orchestration scripts).  
**Primary Dependencies**: GitHub Actions, GH CLI.  
**Constraints**: AI contract guard always green; wiki freeze preserved; helper-agent decomposition kept.

## Project Structure

```text
.github/
├── workflows/                  # composite-action-backed where shared
├── prompts/
├── agents/
├── skills/
├── instructions/
├── copilot-instructions.md
└── ubuntu-wsl2-runtime-contract.md

.specify/
├── memory/constitution.md
├── specs/                      # all v2 slices
├── workflows/                  # drift-realignment, speckit, etc.
├── templates/
├── wiki/
│   ├── human-reference/        # mirror of allowlisted files
│   ├── human-reference-allowlist.txt
│   └── README.md
└── scripts/

scripts/
├── validate-ai-contracts.py
├── workflow-sync-wiki.sh
├── wiki-consume-into-specify.sh
└── workflow-orchestrate-*.sh
```

## Phasing

- **Phase 0**: Audit overlap among helper agents and skills.
- **Phase 1**: Deduplicate agent/skill descriptions; assign one canonical per domain.
- **Phase 2**: Extract shared workflow steps into composite actions; update workflows.
- **Phase 3**: Add schedule-event clauses to actor guards.
- **Phase 4**: Document `.specify/integrations/` and `extensions/` roles (deferred from baseline).
