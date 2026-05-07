# Tasks: 109-mutation-testing-stryker-package-manager-fix

## T001 - Locate and update Stryker packageManager setting

**Status**: Planned
**File(s)**: `src/typescript/api/**stryker*.{json,js,cjs,mjs,ts}`
**Acceptance**: configuration uses a Stryker-supported package manager (`npm`, `yarn`, or `pnpm`).

---

## T002 - Align mutation workflow dependency bootstrap with chosen manager

**Status**: Planned
**File(s)**: `.github/workflows/*mutation*.yml`
**Acceptance**: workflow installs dependencies and starts Stryker without package-manager contract errors.

---

## T003 - Add config guardrail for unsupported Stryker manager values

**Status**: Planned
**File(s)**: workflow/script/config validation surface
**Acceptance**: PR validation fails early if `packageManager` is set to unsupported values.

---

## T004 - Capture mutation lane recovery evidence

**Status**: Planned
**File(s)**: `.specify/specs/109-mutation-testing-stryker-package-manager-fix/tasks.md`
**Acceptance**: includes failing run ID + passing run ID with package-manager error removed.
