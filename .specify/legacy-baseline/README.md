# Banana Legacy Baseline

Snapshots of the current ("v1", AI-vibe-coded) Banana implementation captured per
domain so the v2 regeneration effort (`.specify/specs/005-v2-regeneration/`) has
a stable factual record of:

- What exists today (paths, entry points, runtime contracts)
- What works and should be preserved
- What is over-engineered, duplicated, or fragile ("vibe drift" notes)
- Hard external contracts that v2 must not silently break

These files are deliberately descriptive — not aspirational. Target behavior
lives in the per-domain `spec.md` files under `.specify/specs/006-*` through
`.specify/specs/014-*`.

## Layout

| File | Domain | v2 spec |
|------|--------|---------|
| `native-core.md` | `src/native/{core,wrapper,testing}` + CMake | `006-native-core` |
| `aspnet-pipeline.md` | `src/c-sharp/asp.net` controllers/services/pipeline/interop | `007-aspnet-pipeline` |
| `typescript-api.md` | `src/typescript/api` Fastify + Prisma | `008-typescript-api` |
| `react-shared-ui.md` | `src/typescript/{react,shared/ui}` | `009-react-shared-ui` |
| `electron-desktop.md` | `src/typescript/electron` | `010-electron-desktop` |
| `react-native-mobile.md` | `src/typescript/react-native` | `011-react-native-mobile` |
| `compose-runtime.md` | `docker/`, `docker-compose.yml`, `scripts/` | `012-compose-runtime` |
| `ai-orchestration.md` | `.github/workflows`, `.github/{prompts,agents,skills,instructions}`, `.specify/workflows` | `013-ai-orchestration` |
| `test-coverage.md` | `tests/`, coverage scripts, CI gates | `014-test-coverage` |

## How to update

When the legacy implementation changes (rare during v2 work), edit the relevant
file here in the same PR. When v2 supersedes a baseline, mark that domain
section "Superseded by 0XX-…" rather than deleting — the history is the point.
