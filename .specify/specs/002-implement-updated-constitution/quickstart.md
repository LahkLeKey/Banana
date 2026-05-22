# Quickstart: Constitution-Aligned Feature Delivery Blueprint

## Prerequisites
- Repository available at `c:/Github/Banana`
- Bash shell with Speckit scripts executable
- Active feature set to `.specify/specs/002-implement-updated-constitution`

## 1. Verify active feature context
```bash
.specify/scripts/bash/check-prerequisites.sh --json --paths-only
```
Expected: `FEATURE_DIR` and `FEATURE_SPEC` point to `002-implement-updated-constitution`.

## 2. Review specification and constitution
- Read `.specify/specs/002-implement-updated-constitution/spec.md`
- Read `.specify/memory/constitution.md`

## 3. Validate planning artifacts
Confirm these files exist and are populated:
- `plan.md`
- `research.md`
- `data-model.md`
- `contracts/feature-delivery-contract.md`
- `quickstart.md`

## 4. Run task generation
```bash
.specify/scripts/bash/setup-tasks.sh --json
```
Then run the `/speckit.tasks` workflow to produce `tasks.md`.

## 5. Constitution gate recheck
Before implementation:
- Verify disclosure-impact mapping exists.
- Verify cross-domain contract touchpoints are explicit.
- Verify measurable quality gates are listed.
- Verify delivery evidence artifacts are defined.

## 6. Proceed to implementation
Run `/speckit.implement` after `tasks.md` is approved.
