# Banana

Pass the Banana from native C into API, desktop, web, and mobile channels.

This repo is a practical playground for one core idea: keep domain behavior in native C, then project it cleanly into multiple runtimes without rewriting business logic per app.

## Learn More

- Wiki hub: [Banana Wiki](https://github.com/LahkLeKey/Banana/wiki)
- Onboarding and architecture: `docs/developer-onboarding.md`
- Build/test command index: `.wiki/Build-Run-Test-Commands.md`

## Merge-Gate Troubleshooting

When a merge-gated workflow fails, use this order so the first failure signal stays actionable.

1. Identify whether the failure is policy-state or runtime-state.
2. Check `copilot-review-triage` annotations for unresolved finding count and linked suggestion issues.
3. Confirm JavaScript action runtime compatibility is green (Node 24) before investigating application logic.
4. Open compose lane artifacts under `.artifacts/compose-ci/*` and review `lane-result.json`, `artifact-manifest.json`, and `diagnostics/*`.
5. For permission failures (`exit code 126` or `permission_contract_violation`), use remediation guidance emitted by compose lane metadata.
6. If AI contract guard fails, re-run `python .specify/scripts/validate-ai-contracts.py` and verify wiki allowlist plus canonical mirror parity.

Common local checks:

- `bash scripts/check-workflow-runtime-deprecations.sh --workflow .github/workflows/compose-ci.yml`
- `bash scripts/check-workflow-runtime-deprecations.sh --workflow .github/workflows/copilot-review-triage.yml`
- `bash scripts/validate-compose-ci-lane-contract.sh --lane compose-tests`
- `bash scripts/validate-compose-ci-lane-contract.sh --lane compose-runtime`
- `bash scripts/validate-compose-ci-lane-contract.sh --lane compose-electron`

> Banana is a prototype for Poly: a polymorphic, multi-platform runtime approach where core behavior starts in C and can be projected into API, desktop, frontend, and mobile delivery channels.
