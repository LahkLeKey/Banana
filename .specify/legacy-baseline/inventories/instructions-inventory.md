# Instructions Inventory (v1 snapshot)

Source: `.github/instructions/*.instructions.md`. 6 files. Each declares an
`applyTo` glob; VS Code Copilot auto-loads matching instructions.

| File | `applyTo` |
| --- | --- |
| `api.instructions.md` | `src/c-sharp/asp.net/**/*.cs` |
| `frontend.instructions.md` | `src/typescript/**/*.{ts,tsx,js,json,css}` |
| `infra.instructions.md` | `{docker-compose.yml,docker/**/*,.github/workflows/**/*.yml,scripts/**/*.sh,scripts/**/*.bat,.vscode/**/*.json}` |
| `mobile-runtime.instructions.md` | `{scripts/compose-mobile-emulators-wsl2.sh,scripts/launch-container-channels-with-wsl2-mobile.sh,.vscode/launch.json,.github/ubuntu-wsl2-runtime-contract.md}` |
| `native.instructions.md` | `{CMakeLists.txt,src/native/**/*.c,src/native/**/*.h,tests/native/**/*.c}` |
| `testing.instructions.md` | `{tests/**/*.cs,tests/**/*.c,scripts/run-tests-with-coverage.sh,scripts/run-native-c-tests-with-coverage.sh,.github/workflows/**/*.yml}` |

Verbatim content preserved at `../github-ai-surface/instructions/`.

## v2 Constraints

- Path globs above target v1 layout. v2 may reorganize source — when it
  does, the `applyTo` globs MUST be updated atomically, otherwise the
  domain-specific guidance silently stops loading.
- `.github/copilot-instructions.md` (the workspace-wide root) lives at
  `../github-ai-surface/copilot-instructions.md` and includes the
  Speckit-START/END block referencing the active feature plan.
