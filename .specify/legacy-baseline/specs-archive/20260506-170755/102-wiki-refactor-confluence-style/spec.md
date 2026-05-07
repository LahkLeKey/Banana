# Feature Specification: Wiki Refactor — Confluence-Style Information Architecture

**Feature Branch**: `102-wiki-refactor-confluence-style`
**Created**: 2026-05-01
**Status**: Draft
**Wave**: sixth
**Domain**: docs / devex
**Depends on**: none (self-contained; consumes existing `.wiki/` content)

---

## Problem Statement

The current Banana wiki is a flat collection of markdown pages under `.wiki/` that was grown
organically alongside the codebase. It has several structural problems:

- **No clear information hierarchy** — all pages sit at one level with no space/section grouping.
- **Mixed audiences** — AI-facing auto-generated content (`Auto-GitHub/`, `Auto-Prompts/`) lives
  alongside human onboarding pages (`First-Day-Checklist.md`, `How-A-Request-Works.md`).
- **No consistent page template** — pages vary in format, depth, and linking conventions.
- **Navigation is hand-maintained** — `Home.md` navigation block drifts when pages are added.
- **Discovery is hard** — there's no concept of a "space", "section breadcrumb", or "child page" tree.
- **Confluence-familiar teams can't orient quickly** — engineers who work with Confluence expect
  spaces → sections → pages with parent/child context and a "related pages" sidebar.

---

## Goals

1. Reorganize `.wiki/` into a Confluence-style **space → section → page** hierarchy.
2. Establish consistent page templates for each section type (overview, how-to, reference, ADR, runbook).
3. Auto-generate the nav/index pages from directory structure so they never drift.
4. Keep `.specify/wiki/human-reference/` as the AI-consumable mirror (flat, allowlist-controlled) and
   update the sync script to handle the new folder structure.
5. Separate human pages from auto-generated AI-audit pages at the folder level.
6. Produce a Home page that reads like a Confluence space home: clear purpose, "about this space"
   summary, section cards, and a "recently updated" list (populated from git log).

---

## Proposed Information Architecture

```
.wiki/
  Home.md                          ← Space home (auto-generated nav, git-recent list)
  _templates/                      ← Page templates (not published to .specify mirror)
    overview.md
    how-to.md
    reference.md
    runbook.md
    adr.md

  getting-started/                 ← Section: "Getting Started" (onboarding)
    README.md                      ← Section index (auto-generated)
    first-day-checklist.md
    how-a-request-works.md
    build-run-test-commands.md
    repository-layout.md

  architecture/                    ← Section: "Architecture"
    README.md
    overview.md                    ← Replaces Architecture-Diagrams.md
    native-wrapper-abi.md          ← Replaces Why-We-Use-A-Wrapper.md
    database-pipeline-stage.md     ← Replaces How-The-Database-Step-Works.md
    domain-model.md

  operations/                      ← Section: "Operations & Runbooks"
    README.md
    ci-compose-notes.md
    wsl2-runtime-channels.md       ← Replaces WSL2-Mobile-Runtime-Channels.md
    ci-runtime-compatibility.md
    ci-compose-stabilization.md

  security/                        ← Section: "Security"
    README.md
    threat-model.md
    csp-and-headers.md

  data/                            ← Section: "Data & Migrations"
    README.md
    migration-discipline.md
    coverage-exceptions.md

  governance/                      ← Section: "Governance & Contracts"
    README.md
    api-parity-governance.md
    autonomous-self-training.md

  ai-reference/                    ← Section: "AI Reference" (auto-generated; not in human allowlist)
    README.md
    Auto-GitHub/
    Auto-GitHub-Instructions/
    Auto-GitHub-Workflows/
    Auto-Prompts/
    Auto-README.md
```

---

## In Scope

- Migrate all current `.wiki/` flat pages into the new section hierarchy (rename/move, update links).
- Write a `scripts/wiki-scaffold.sh` script that:
  - Generates `README.md` section index pages from directory contents.
  - Regenerates `Home.md` nav block and "recently updated" table from `git log`.
  - Validates that every `.md` file in a human section is present in the allowlist.
- Add page templates under `.wiki/_templates/`.
- Update `scripts/wiki-consume-into-specify.sh` to handle subdirectory source layout and
  flatten to `.specify/wiki/human-reference/` as before (slug-deduplication if filenames collide).
- Update `scripts/wiki-sync-wiki.sh` (publish sync) to reflect new layout.
- Update `human-reference-allowlist.txt` to use new slugs.
- Add a CI workflow `.github/workflows/wiki-lint.yml` that:
  - Runs `wiki-scaffold.sh --dry-run --validate` on PR to catch broken links and orphaned pages.
  - Fails if a human-section page is not in the allowlist.
- Write/update the top-level `Human-Reading-Guide.md` (becomes `getting-started/README.md`) to
  explain section layout for new engineers.

## Out of Scope

- Hosting the wiki in Confluence itself (remains GitHub Wiki markdown).
- Auto-populating content for new sections beyond what already exists.
- Versioned wiki (deferred — track as follow-on to #103 if needed).
- Migrating AI-facing auto-generated pages' content (only structure changes).

---

## User Scenarios

### Scenario A — New engineer onboarding
> Dana joins the team. She opens the wiki Home page and sees section cards: Getting Started,
> Architecture, Operations, Security. She clicks "Getting Started" and lands on a section index
> listing all child pages with one-line descriptions. She reads `first-day-checklist.md`,
> which links to `build-run-test-commands.md` in the same section. She never has to hunt the
> flat root listing.

### Scenario B — On-call engineer finding a runbook
> Marcus is paged for a compose failure. He goes to Operations → `ci-compose-notes.md` directly
> from the section index. The page has a breadcrumb header `Operations > CI Compose Notes`
> and a "related pages" footer pointing to `wsl2-runtime-channels.md`.

### Scenario C — Agent consuming wiki
> A Copilot agent queries `.specify/wiki/human-reference/` and finds flat files with consistent
> slug names. The consume script has already flattened `getting-started/first-day-checklist.md`
> to `first-day-checklist.md` in the mirror, so existing agent prompts that reference those slugs
> continue to work without change.

### Scenario D — Author adding a new page
> A developer adds `operations/deploy-rollback-runbook.md`. She runs
> `bash scripts/wiki-scaffold.sh` locally, which regenerates `operations/README.md` to include
> the new page and updates `Home.md`'s "recently updated" list. She adds the slug to the
> allowlist and the CI lint passes.

---

## Functional Requirements

| ID | Requirement |
|----|-------------|
| WR-01 | All existing human-facing `.wiki/` pages are migrated to the section hierarchy with no broken links. |
| WR-02 | Section `README.md` index pages are auto-generated by `wiki-scaffold.sh` from directory contents. |
| WR-03 | `Home.md` is auto-generated with section cards and a "recently updated" table (top 10 by `git log`). |
| WR-04 | Each human-section page begins with a breadcrumb comment block (`<!-- breadcrumb: Section > Page -->`). |
| WR-05 | Page templates exist for: overview, how-to, reference, runbook, ADR. |
| WR-06 | `wiki-consume-into-specify.sh` handles subdirectory source and flattens to `human-reference/` mirror. |
| WR-07 | `human-reference-allowlist.txt` is updated to new slugs and CI enforces it. |
| WR-08 | `wiki-lint.yml` workflow validates broken links and orphan pages on every PR that touches `.wiki/`. |
| WR-09 | AI-reference pages (`ai-reference/`) are excluded from the allowlist and the human mirror. |
| WR-10 | All existing cross-references between wiki pages resolve after migration. |

---

## Success Criteria

- [ ] `scripts/wiki-scaffold.sh --validate` exits 0 on the migrated layout.
- [ ] `scripts/wiki-consume-into-specify.sh` exits 0 and populates `.specify/wiki/human-reference/` with expected flat slugs.
- [ ] `python scripts/validate-ai-contracts.py` exits 0 (allowlist current).
- [ ] CI `wiki-lint.yml` passes on the migration PR.
- [ ] A new engineer can find `first-day-checklist.md` from Home within 2 clicks.
- [ ] No existing `.specify` agent prompt references a broken wiki slug after migration.

---

## Notes for the Planner

- Run `speckit.plan` to break this into implementation tasks before starting migration.
- The migration should be done in a single atomic PR to avoid a window where `.wiki/` is
  partially restructured and CI lint is broken.
- The `wiki-scaffold.sh` script should be idempotent (safe to re-run; does not clobber
  hand-authored content, only regenerates index/nav blocks between `<!-- AUTO -->` markers).
- Slug collision strategy for the consume script: prefix with section name
  (`operations-ci-compose-notes.md`) only when a conflict exists.
- Agent prompts that reference wiki slugs by filename (e.g., `Build-Run-Test-Commands.md`) will
  need a slug-redirect map or a rename of the target file; enumerate these in the plan artifact.
