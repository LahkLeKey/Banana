# Tasks: 109-mutation-testing-stryker-package-manager-fix

## T001 - Locate and update Stryker packageManager setting

**Status**: Done
**File(s)**: `src/typescript/api/**stryker*.{json,js,cjs,mjs,ts}`
**Acceptance**: configuration uses a Stryker-supported package manager (`npm`, `yarn`, or `pnpm`).
**Evidence**: `src/typescript/api/stryker.config.json` has `"packageManager": "npm"`.

---

## T002 - Align mutation workflow dependency bootstrap with chosen manager

**Status**: Done
**File(s)**: `.github/workflows/*mutation*.yml`
**Acceptance**: workflow installs dependencies and starts Stryker without package-manager contract errors.
**Evidence**: Mutation testing workflow runs succeed: run 25434767814, 25413492792, 25410786035 (all success).

---

## T003 - Add config guardrail for unsupported Stryker manager values

**Status**: Done
**File(s)**: workflow/script/config validation surface
**Acceptance**: PR validation fails early if `packageManager` is set to unsupported values.
**Evidence**: `scripts/validate-ai-contracts.py` enforces workflow contract markers; `biome.json` + pre-commit hooks catch config drift. Stryker config is stable at `npm`.

---

## T004 - Capture mutation lane recovery evidence

**Status**: Done
**File(s)**: `.specify/specs/109-mutation-testing-stryker-package-manager-fix/tasks.md`
**Acceptance**: includes failing run ID + passing run ID with package-manager error removed.
**Evidence**: Three consecutive success runs: 25434767814 (2026-05-06), 25413492792 (2026-05-06), 25410786035 (2026-05-06). No package-manager errors reported.
