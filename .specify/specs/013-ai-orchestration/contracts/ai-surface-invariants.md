# Contract — AI Surface Invariants

## Wiki freeze

- `.wiki/` is the human-reference publication surface, frozen at the file list in `.specify/wiki/human-reference-allowlist.txt`.
- `.specify/wiki/human-reference/` mirrors the allowlist exactly.
- Adding a page requires updating the allowlist, the source, and the mirror in the same change.
- The validator (`scripts/validate-ai-contracts.py`) enforces parity and blocks drift.

## Helper-agent decomposition

- Each domain has a parent agent and one or more helpers.
- Exactly one helper agent owns any given file path.
- Use the parent agent only when more than one helper in the domain must move together.
- See `.github/skills/banana-agent-decomposition/SKILL.md`.

## Workflow guards

- Scheduled workflows (`on: schedule`) MUST include `github.event_name == 'schedule'` in any actor-based guard, since cron runs as `github-actions[bot]`.
- YAML `${{ ... }}` env defaults containing `:` MUST be quoted.
- Input descriptions containing JSON-like `{...: ...}` snippets MUST be quoted.

## Constitution + prompts

- `.specify/memory/constitution.md` is the authoritative constitution.
- Every prompt that references wiki content MUST link to a file under `.specify/wiki/` (validated by the guard).

## Validator command of record

```bash
python scripts/validate-ai-contracts.py
```

Expected: exit 0, JSON output with `"issues": []`.
