# Specify Wiki Canonical Store

This directory is the canonical wiki-content store for Spec Kit and automation workflows.

## Policy

- Canonical machine-readable wiki content lives under `.specify/wiki/`.
- `.wiki/` remains available for human reference and browsing.
- Automation should consume from `.specify/wiki/` and publish human-facing snapshots to `.wiki/`.

## Human Reference Mirror

- Path: `.specify/wiki/human-reference/`
- Source: local `.wiki/` checkout markdown pages
- Approved page allowlist: `.specify/wiki/human-reference-allowlist.txt`
- Sync command:

```bash
bash scripts/wiki-consume-into-specify.sh
```

## Notes

- Use `--dry-run` to preview changes.
- Use `--no-prune` to avoid deleting unmatched target files.

## Temporary Wiki Freeze Contract

- `.wiki/` is currently frozen to the pages listed in `.specify/wiki/human-reference-allowlist.txt`.
- `scripts/wiki-consume-into-specify.sh` enforces the allowlist by default and fails when unexpected `.wiki` pages appear.
- `scripts/validate-ai-contracts.py` enforces both `.wiki` allowlist compliance and mirror parity with `.specify/wiki/human-reference/`.
- To add new wiki pages intentionally, update the allowlist in the same spec-driven change and then re-run wiki consume + contract validation.
