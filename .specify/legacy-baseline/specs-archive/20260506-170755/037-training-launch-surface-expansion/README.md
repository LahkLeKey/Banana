# 037 Training Launch Surface Expansion

Status: COMPLETE

## Summary

Adds full F5 and command-palette training launch coverage for not-banana, banana, and ripeness across `ci` and `local` profiles, with explicit artifacts output paths under `artifacts/training/<model>/local`.

## Execution Tracker

- [x] Updated existing not-banana launch entries with explicit `--output` and `--corpus` args.
- [x] Added banana and ripeness launch entries for `ci` and `local` profiles.
- [x] Added compound launch entry `Train: all (ci profile)`.
- [x] Added mirrored `.vscode/tasks.json` entries for all launch commands.
- [x] Added local training runbook (`analysis/local-training-runbook.md`).
- [x] Ran AI-contract and spec/tasks parity validators.

## Notes

- Outputs are intentionally directed to `artifacts/training/*` so seed corpora and promoted image registries remain untouched during local training runs.
