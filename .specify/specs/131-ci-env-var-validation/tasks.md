# Tasks: CI Environment Variable Validation Gate (131)

**Input**: Spec at `.specify/specs/131-ci-env-var-validation/spec.md`
**Prerequisites**: Spec 124 (.env.example exists); spec 122 (CI workflow exists)

- [ ] T001 Add a `loadEnv` check in `vite.config.ts` that throws `Error: VITE_BANANA_API_BASE_URL is required for production builds` when the variable is empty and `mode === 'production'`.
- [ ] T002 Run `VITE_BANANA_API_BASE_URL="" bun run build` in `src/typescript/react` and confirm exit non-zero with a descriptive error message.
- [ ] T003 Add a pre-build env check step to `.github/workflows/vercel-deploy.yml`: iterate `required_vars` list and fail with `echo "Missing: $var"` if any are unset.
- [ ] T004 Run `bun run build` (with the var set) and confirm exit 0.
- [ ] T005 Run `python scripts/validate-spec-tasks-parity.py .specify/specs/131-ci-env-var-validation` and confirm `OK`.
