# Banana v2 Bootstrap

This document is the **only entry point** to rebuild Banana after the v1 wipe
described in [DELETION-MANIFEST.md](./DELETION-MANIFEST.md). Everything
needed to regenerate the codebase lives under `.specify/`.

## 0. Preserve Set (post-wipe state)

Only these survive the wipe:

```
.git/                 .gitignore
.specify/             .gitattributes
README.md             LICENSE
```

If you see anything else in the worktree before starting bootstrap, the wipe
was incomplete — re-run the deletion checklist first.

## 1. Read the specs in topological order

Specs live in `.specify/specs/`. Read in this order — earlier specs are
inputs to later specs:

1. [005-v2-regeneration](specs/005-v2-regeneration/spec.md) — overall goals + dependency graph + success criteria.
2. [006-native-core](specs/006-native-core/spec.md) — native C, ABI, DAL gating.
3. [008-typescript-api](specs/008-typescript-api/spec.md) — Fastify + Prisma 7 routes.
4. [007-aspnet-pipeline](specs/007-aspnet-pipeline/spec.md) — ASP.NET pipeline + interop seam.
5. [009-react-shared-ui](specs/009-react-shared-ui/spec.md) — `@banana/ui` public surface.
6. [010-electron-desktop](specs/010-electron-desktop/spec.md) — desktop runtime.
7. [011-react-native-mobile](specs/011-react-native-mobile/spec.md) — mobile runtime.
8. [012-compose-runtime](specs/012-compose-runtime/spec.md) — compose + scripts.
9. [014-test-coverage](specs/014-test-coverage/spec.md) — test taxonomy + coverage.
10. [013-ai-orchestration](specs/013-ai-orchestration/spec.md) — `.github/` AI surface + workflows.

For each spec read `plan.md` (HOW), `tasks.md` (ORDERED WORK), and
`contracts/*.md` (INVARIANTS). `quickstart.md` shows the end-to-end smoke.

## 2. Cross-reference legacy facts

Do not invent ABI symbols, route shapes, env vars, or workflow triggers.
Use [legacy-baseline/](legacy-baseline/) as the source of truth for what
v1 actually had:

- [legacy-baseline/inventories/native-abi-symbols.md](legacy-baseline/inventories/native-abi-symbols.md) — 21 BANANA_API exports + status codes.
- [legacy-baseline/inventories/http-routes.md](legacy-baseline/inventories/http-routes.md) — controllers + verbs + paths.
- [legacy-baseline/inventories/env-vars.md](legacy-baseline/inventories/env-vars.md) — full env contract surface.
- [legacy-baseline/inventories/scripts-inventory.md](legacy-baseline/inventories/scripts-inventory.md) — every v1 script + role.
- [legacy-baseline/inventories/workflows-inventory.md](legacy-baseline/inventories/workflows-inventory.md) — 10 GH Actions workflows.
- [legacy-baseline/inventories/dockerfiles-inventory.md](legacy-baseline/inventories/dockerfiles-inventory.md) — 8 Dockerfiles + 16 services.
- [legacy-baseline/inventories/prisma-schema-snapshot.md](legacy-baseline/inventories/prisma-schema-snapshot.md) — v1 Prisma 7 models.
- [legacy-baseline/inventories/prompts-inventory.md](legacy-baseline/inventories/prompts-inventory.md) — 25 prompts ↔ wiki contract.
- [legacy-baseline/inventories/agents-inventory.md](legacy-baseline/inventories/agents-inventory.md) — 28 agents + helper relationships.
- [legacy-baseline/inventories/skills-inventory.md](legacy-baseline/inventories/skills-inventory.md) — 16 skills.
- [legacy-baseline/inventories/instructions-inventory.md](legacy-baseline/inventories/instructions-inventory.md) — 6 `applyTo` instructions.

The verbatim `.github/{prompts,agents,skills,instructions,workflows}/` and
`.github/copilot-instructions.md` content is preserved at
[legacy-baseline/github-ai-surface/](legacy-baseline/github-ai-surface/)
so you can copy-paste structures (text content is reusable; paths and
imports must update).

The original domain narratives are at [legacy-baseline/](legacy-baseline/)
(README + 9 domain notes capturing `Paths / What exists / Hard contracts /
What works / Vibe drift / Cross-domain dependencies / v1 entry points to
preserve`).

## 3. Rebuild order

Follow this exact order. Each step lists which spec/inventory drives it.

### Step 1 — root scaffolding (driven by 012, 006)

Recreate at the repo root:

- `CMakeLists.txt` (per `006/plan.md` + `006/contracts/native-wrapper-abi.md`)
- `Directory.Build.props` (per `007/plan.md`)
- `docker-compose.yml` (per `012/contracts/channel-scripts.md` + [dockerfiles-inventory.md](legacy-baseline/inventories/dockerfiles-inventory.md) services list)
- `Banana.code-workspace` (per `012/quickstart.md`)
- `.dockerignore` MUST exclude `**/node_modules` (see `012/spec.md`)

### Step 2 — `src/native/` (driven by 006)

Implement the 21 ABI exports listed in
[native-abi-symbols.md](legacy-baseline/inventories/native-abi-symbols.md)
following the int-status + out-pointer + `banana_free` ownership
contract. Keep core under `src/native/core/`, DAL under
`src/native/core/dal/` (Postgres-gated, returns `DbNotConfigured`), wrapper
under `src/native/wrapper/`. Module count ≤ 15.

### Step 3 — `src/c-sharp/asp.net/` (driven by 007)

Build controllers per [http-routes.md](legacy-baseline/inventories/http-routes.md).
Single typed `PipelineContext`, single interop seam under
`NativeInterop/` (one `LibraryImport` site per export). Use
`BANANA_NATIVE_PATH` for library resolution.

### Step 4 — `src/typescript/api/` (driven by 008)

Fastify + Bun + Prisma 7. Use `prisma.config.ts` (NOT inline `url`) per
[prisma-schema-snapshot.md](legacy-baseline/inventories/prisma-schema-snapshot.md).
Route ownership table is `008/contracts/route-ownership.md` — every Fastify
route declares which Prisma model it persists into.

### Step 5 — `src/typescript/{shared/ui,react,electron,react-native}/` (driven by 009, 010, 011)

- `shared/ui` exposes only via `@banana/ui` public index (`009/contracts/shared-ui-public-surface.md`).
- React app uses `VITE_BANANA_API_BASE_URL`, Bun, single canonical configs (`009`).
- Electron uses `npm ci --omit=dev` + `CXXFLAGS=-fpermissive`; `electron-example` (smoke) ≠ `electron-desktop` (UI). See `010/contracts/runtime-channels.md`.
- React Native: Android via WSLg (KVM-required, profile-gated), iOS web-preview fallback on Linux. See `011/contracts/mobile-runtime-channels.md`.

### Step 6 — `docker/` + `scripts/` (driven by 012)

Recreate the 8 Dockerfiles per [dockerfiles-inventory.md](legacy-baseline/inventories/dockerfiles-inventory.md)
and the canonical channel scripts per `012/contracts/channel-scripts.md`.
Lessons that MUST carry over: CRLF normalization in shell COPY paths,
`.dockerignore` excluding `**/node_modules`, `bun install --cwd shared/ui`
before app install, lockfile installs in Electron.

### Step 7 — `.github/` (driven by 013)

Use [github-ai-surface/](legacy-baseline/github-ai-surface/) as the source
of structure. Recreate:

- `.github/workflows/` (10 files) — see [workflows-inventory.md](legacy-baseline/inventories/workflows-inventory.md). Update `ai-contract-guard.yml` to invoke `python .specify/scripts/validate-ai-contracts.py` (not `scripts/validate-ai-contracts.py`).
- `.github/prompts/` (25 files), `.github/agents/` (28 files), `.github/skills/` (16 dirs), `.github/instructions/` (6 files) — content is reusable verbatim from the snapshot.
- `.github/copilot-instructions.md` — reusable verbatim from the snapshot, but update the SPECKIT block to reference the active feature plan path.

The validator (`.specify/scripts/validate-ai-contracts.py`) auto-detects
whether to read the live `.github/` tree or fall back to the snapshot, so
it stays green throughout the rebuild.

### Step 8 — `tests/` (driven by 014)

Recreate test taxonomy per `014/contracts/test-taxonomy.md`: unit /
integration / e2e / native, with stable `coverage-denominator.json`
maintained by `scripts/sync-banana-api-coverage-denominator.py`. Loud DAL
gating applies to integration; ≤2 fake-update files per change.

### Step 9 — `.wiki/` (driven by 013)

The 12-page human-reference wiki is mirrored at
`.specify/wiki/human-reference/`. Run
`bash .specify/scripts/workflow-sync-wiki.sh` to seed `.wiki/` from
`.specify/wiki/`. The allowlist at
`.specify/wiki/human-reference-allowlist.txt` is enforced by the validator.

## 4. Validation gates

After each step, run:

```bash
python .specify/scripts/validate-ai-contracts.py
```

It must exit `0` with `{"issues": [], ...}`. After the full rebuild, run
the per-spec quickstarts (each spec has a `quickstart.md`) and verify the
test taxonomy in `014/contracts/test-taxonomy.md`.

## 5. Survival tooling

The following `.specify/scripts/` are pre-loaded for use during rebuild:

- `validate-ai-contracts.py` — AI contract guard.
- `wiki-consume-into-specify.sh` — pull `.wiki/` content into `.specify/wiki/`.
- `workflow-sync-wiki.sh` — push `.specify/wiki/` content into `.wiki/`.

## 6. Hard contracts you cannot violate

These propagate across every step:

- `BANANA_PG_CONNECTION` absence → `DbNotConfigured` (loud gate, not silent fail).
- `BANANA_NATIVE_PATH` is the only library resolution mechanism for ASP.NET.
- `VITE_BANANA_API_BASE_URL` is compile-time-injected; no runtime lookup.
- `DATABASE_URL` is consumed via `prisma.config.ts`, never inline `url`.
- Bun is the package manager for `src/typescript/{api,react,react-native,shared/ui}`. Electron is the exception and uses `npm ci`.
- Exit code `42` means runtime preflight failure (use it consistently in launch scripts).
- WSL2/WSLg distro selection order: `BANANA_WSL_DISTRO` → `Ubuntu-24.04` → `Ubuntu`.
- Wiki allowlist contains exactly 12 files; growing it requires explicit spec update.
