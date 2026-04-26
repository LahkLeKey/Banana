# Quickstart — AI Orchestration (v2)

## Run the validator

```bash
python scripts/validate-ai-contracts.py
```

Expected output: `"issues": []` and a non-empty `prompt_count`.

## Resync wiki mirror

```bash
bash scripts/wiki-consume-into-specify.sh
```

Expected: `source` count == `copied` count == 12 (matches the allowlist).

## Sync wiki worktree (publication)

```bash
bash scripts/workflow-sync-wiki.sh
```

Defaults to `artifacts/wiki-sync/...`; refuses to use repo `.wiki` directly.

## Expand the allowlist (intentional)

1. Add the new file's `Title.md` to `.specify/wiki/human-reference-allowlist.txt`.
2. Add the corresponding markdown under `.wiki/` with the agreed content.
3. Run the consume script to mirror into `.specify/wiki/human-reference/`.
4. Run the validator. Expect green.

## Scope guardrails

- Don't expand `.wiki/` outside the allowlist process.
- Don't add an actor-guard condition that excludes `github.event_name == 'schedule'` if scheduled cron is intended.
- Don't introduce duplicate helper-agent descriptions.
