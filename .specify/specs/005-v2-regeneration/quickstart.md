# Quickstart — v2 Regeneration (Parent)

This is the navigation entry for the v2 regeneration. It does not run code.

## Read in this order

1. [spec.md](./spec.md) — what we're decomposing and why.
2. [plan.md](./plan.md) — phasing and structure decision.
3. [.specify/legacy-baseline/README.md](../../legacy-baseline/README.md) — index of v1 snapshots.
4. The child slice you are working on, e.g., [`006-native-core/spec.md`](../006-native-core/spec.md).

## Pick a slice

Use the table in [spec.md](./spec.md#child-specs-v2-domain-slices). Respect the
topological order: `006 → 008 → 007 → 009 → 010, 011 → 012 → 014 → 013`.

Each slice maps to a helper agent — see `.github/skills/banana-agent-decomposition/SKILL.md`:

| Slice | Helper agent |
|-------|--------------|
| 006-native-core | `native-c-agent` (or `native-core-agent` / `native-dal-agent` / `native-wrapper-agent`) |
| 007-aspnet-pipeline | `csharp-api-agent` (or `api-pipeline-agent` / `api-interop-agent`) |
| 008-typescript-api | `react-agent` (TypeScript backend) |
| 009-react-shared-ui | `react-ui-agent` |
| 010-electron-desktop | `electron-agent` |
| 011-react-native-mobile | `mobile-runtime-agent` |
| 012-compose-runtime | `compose-runtime-agent` / `infrastructure-agent` |
| 013-ai-orchestration | `workflow-agent` / `technical-writer-agent` |
| 014-test-coverage | `test-triage-agent` / `integration-agent` |

## Validate scaffolding

```bash
python scripts/validate-ai-contracts.py
find .specify/specs/00{6..9}-* .specify/specs/01{0..4}-* -name spec.md | wc -l   # expect 9
find .specify/legacy-baseline -name '*.md' -not -name README.md | wc -l           # expect 9
```

## Start a slice (template)

1. Open the child spec and its baseline side-by-side.
2. Refine the child `plan.md` (Technical Context, Structure Decision, Phasing).
3. Expand the child `tasks.md` into ordered, file-scoped tasks.
4. Open a feature branch named `0XX-<domain>` and implement Phase 2/3 tasks.
5. Keep hard contracts intact until cutover lands and consumers are migrated.

## Do not

- Edit `.wiki/` outside the 12-file allowlist (`.specify/wiki/human-reference-allowlist.txt`).
- Refactor v1 code in this parent PR — scaffolding only.
- Delete a legacy file before its v2 counterpart is wired up and consumers cut over.
