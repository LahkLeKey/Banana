# Banana v1 Deletion Manifest

This document defines the exact set of files that survive the v1 wipe.
After executing the deletion checklist below, the worktree should contain
**only** the preserve set. Everything needed to rebuild lives under
`.specify/` (see [BOOTSTRAP.md](./BOOTSTRAP.md)).

## Preserve set (DO NOT DELETE)

```
.git/
.gitattributes
.gitignore
.specify/
LICENSE
README.md
```

That's it. Six entries. Anything else in the worktree after the wipe is a
bug.

## Deletion set (everything else)

Top-level entries to remove (matches current worktree):

```
.github/                 — AI surface; preserved verbatim under .specify/legacy-baseline/github-ai-surface/
.vscode/                 — workspace settings; v2 regenerates from specs
.wiki/                   — human wiki; preserved at .specify/wiki/human-reference/
artifacts/               — coverage XML files; transient
build/                   — CMake/build outputs; transient
data/                    — runtime data (not-banana corpus, etc.); regenerate from training scripts
docker/                  — Dockerfiles; rebuilt per spec 012
docs/                    — developer docs; replaced by .specify/specs/*/quickstart.md
scripts/                 — shell/python scripts; survival scripts already copied to .specify/scripts/
src/                     — all source; rebuilt per specs 006-014
tests/                   — all tests; rebuilt per spec 014
Banana.code-workspace    — VS Code multi-root config
CMakeLists.txt           — root CMake; rebuilt per spec 006
Directory.Build.props    — root MSBuild; rebuilt per spec 007
README.md                — KEEP (preserve set)
bash.exe.stackdump       — junk artifact
docker-compose.yml       — rebuilt per spec 012
```

## Pre-deletion checklist

Run these BEFORE deleting anything. Any failure = abort the wipe.

1. **Survival scripts present**:
   ```bash
   ls .specify/scripts/validate-ai-contracts.py \
      .specify/scripts/wiki-consume-into-specify.sh \
      .specify/scripts/workflow-sync-wiki.sh
   ```
2. **AI surface snapshot present** (5 dirs + 2 files):
   ```bash
   ls .specify/legacy-baseline/github-ai-surface/{prompts,agents,skills,instructions,workflows} \
      .specify/legacy-baseline/github-ai-surface/copilot-instructions.md
   ```
3. **Inventories present** (11 files):
   ```bash
   ls .specify/legacy-baseline/inventories/*.md | wc -l   # expect 12 (README + 11)
   ```
4. **Wiki mirror present** (12 files):
   ```bash
   find .specify/wiki/human-reference -name '*.md' | wc -l   # expect 12
   ```
5. **Specs present** (10 specs × 5 files = 50 + parent contracts):
   ```bash
   find .specify/specs -name '*.md' | wc -l   # expect 50+
   ```
6. **Validator green from new location** (uses snapshot fallback):
   ```bash
   python .specify/scripts/validate-ai-contracts.py >/dev/null && echo OK
   ```
7. **Working tree clean** (commit any in-flight changes first):
   ```bash
   git status --short
   ```

## Deletion commands

After all 7 checks pass:

```bash
# Top-level dirs
rm -rf .github .vscode .wiki artifacts build data docker docs scripts src tests

# Top-level files (preserve list inverted)
rm -f Banana.code-workspace CMakeLists.txt Directory.Build.props \
      docker-compose.yml bash.exe.stackdump
```

## Post-deletion verification

```bash
# Worktree should match preserve set exactly
ls -A | sort
# Expected output (one per line):
# .git
# .gitattributes
# .gitignore
# .specify
# LICENSE
# README.md
```

```bash
# Validator should still pass (auto-falls-back to snapshot)
python .specify/scripts/validate-ai-contracts.py
# Expected: {"issues": [], ...} exit 0
```

## Recovery

If something goes wrong, the wipe is in a single git commit and reversible:

```bash
git reset --hard HEAD~1   # only if commit not pushed
# or
git revert <wipe-sha>
```

DO NOT push the wipe commit until the post-deletion verification passes.

## Why this is safe

- All v1 contracts (ABI, routes, env, scripts, workflows, Dockerfiles, prompts, agents, skills, instructions, prisma) are preserved as inventories under `.specify/legacy-baseline/inventories/`.
- All AI customization content is preserved verbatim under `.specify/legacy-baseline/github-ai-surface/`.
- All wiki content is preserved at `.specify/wiki/human-reference/`.
- All specs needed to regenerate are at `.specify/specs/{005..014}/`.
- The validator runs identically pre- and post-wipe (auto-fallback).
- Rebuild instructions are at `.specify/BOOTSTRAP.md`.
