# Feature Specification: v2 Regeneration

**Feature Branch**: `005-v2-regeneration`  
**Created**: 2026-04-25  
**Status**: Draft  
**Input**: User description: scaffold the entire repo into Spec Kit so a clean v2 can be built from lessons learned and prior mistakes. v1 works end-to-end but accumulated AI-generated drift; decomposing everything into `.specify/` lets v2 be cleaner and more optimized.

## Context

The current Banana implementation ("v1") works end-to-end across native C, ASP.NET, React/Electron/RN, and a containerized runtime, but has accumulated speculative modules, duplicated abstractions, mixed package managers, and contract drift. This feature decomposes the entire repo into Spec Kit artifacts so v2 can be regenerated cleanly from `.specify/` rather than refactored in place.

Legacy snapshots live under [.specify/legacy-baseline/](../../legacy-baseline/README.md) and are the factual record of v1. v2 target behavior lives in the per-domain child specs enumerated below.

## User Scenarios & Testing *(mandatory)*

### User Story 1 — Single source of truth for every domain (Priority: P1)

As a maintainer, I can open `.specify/specs/` and see one Spec Kit feature per Banana domain, each with `spec.md`, `plan.md`, `tasks.md`, `quickstart.md`, and `contracts/`, plus a paired legacy-baseline snapshot.

**Why this priority**: Without this, v2 regeneration has no anchor and AI agents will keep accreting drift.

**Independent Test**: `find .specify/specs/00{6..14}-* -name spec.md | wc -l` returns 9, and `find .specify/legacy-baseline -name '*.md' | wc -l` returns ≥10 (README + 9 domains).

**Acceptance Scenarios**:

1. **Given** the repo at v1, **When** a maintainer reads `.specify/specs/005-v2-regeneration/spec.md`, **Then** the full per-domain child list, ordering, and dependencies are visible.
2. **Given** any domain spec, **When** opened, **Then** it links to its legacy-baseline file and to the domains it consumes/is-consumed-by.

---

### User Story 2 — Hard external contracts are explicit and preserved (Priority: P1)

As a v2 implementer, I can identify every hard external contract that v2 must preserve (env var names, exit codes, route shapes, ABI symbols, file paths used by automation) without re-reading v1 source.

**Why this priority**: v2 is allowed to change internals freely but must not silently break consumers (frontends, CI, runtime channels).

**Independent Test**: Each child spec lists a "Hard contracts to preserve" section; AI contract guard validates wiki + spec parity.

**Acceptance Scenarios**:

1. **Given** the parent spec, **When** an agent searches for `BANANA_PG_CONNECTION`, `BANANA_NATIVE_PATH`, `VITE_BANANA_API_BASE_URL`, `DATABASE_URL`, or exit code `42`, **Then** the relevant child spec or baseline is found.
2. **Given** a v2 PR, **When** it changes a hard contract, **Then** the corresponding spec must change in the same PR (enforced by review checklist, see `013-ai-orchestration`).

---

### User Story 3 — Domain slices can be regenerated independently (Priority: P2)

As an agent or maintainer, I can pick a single child spec (e.g., `006-native-core`) and regenerate that domain in v2 without forking unrelated domains.

**Why this priority**: Enables incremental v2 delivery and parallel agent work.

**Independent Test**: Each child spec's `quickstart.md` describes a build/test loop scoped to its domain only.

**Acceptance Scenarios**:

1. **Given** child spec `006-native-core`, **When** following its quickstart, **Then** the native build + native tests pass without any ASP.NET or frontend dependency.
2. **Given** child spec `009-react-shared-ui`, **When** following its quickstart, **Then** the React app builds against a mocked or stable API base URL contract.

---

### User Story 4 — Vibe drift is captured, not buried (Priority: P2)

As a maintainer, I can see what v1 over-engineered or duplicated so v2 can deliberately not repeat those mistakes.

**Why this priority**: The whole point of the regeneration is to drop accreted slop, not preserve it.

**Independent Test**: Each `.specify/legacy-baseline/*.md` file has a "Vibe drift / pain points" section.

**Acceptance Scenarios**:

1. **Given** any legacy-baseline file, **When** reviewed, **Then** at least three concrete pain points are listed.
2. **Given** any v2 child spec, **When** it overlaps a pain point, **Then** the spec explicitly addresses it (or marks it out-of-scope with rationale).

---

### Edge Cases

- A v1 module is genuinely correct and need not be redesigned → v2 spec MAY say "Preserve v1 implementation; only port to new structure."
- A v1 hard contract is itself harmful (e.g., string-typed pipeline bag) → v2 spec MAY break it but MUST list affected consumers and migration steps.
- A new domain is discovered mid-regeneration → add a new `.specify/specs/0XX-…` and update this parent's child table.

## Requirements *(mandatory)*

### Functional Requirements

- **FR-001**: The repo MUST contain `.specify/legacy-baseline/<domain>.md` files for the 9 domains enumerated below.
- **FR-002**: The repo MUST contain `.specify/specs/00X-<domain>/` directories (006..014) each with `spec.md`, `plan.md`, `tasks.md`, `quickstart.md`, and `contracts/`.
- **FR-003**: This parent spec MUST enumerate child specs and their cross-domain dependency edges.
- **FR-004**: Each child spec MUST list hard contracts that v2 must preserve verbatim from v1.
- **FR-005**: Each child spec MUST identify pain points from its baseline that v2 explicitly addresses or defers with rationale.
- **FR-006**: The wiki freeze contract from `004-trim-vibe-drift` MUST remain enforced; v2 work MUST NOT expand `.wiki/` outside the allowlist.
- **FR-007**: AI contract validator (`scripts/validate-ai-contracts.py`) MUST pass after scaffolding lands.
- **FR-008**: Cross-domain dependency edges MUST form a DAG (no cycles); the parent spec records the edges and topological order.

### Key Entities

- **Domain Slice**: A child spec (006..014) covering a coherent runtime/code area.
- **Legacy Baseline**: A read-only snapshot of v1 for a domain.
- **Hard Contract**: An externally observable behavior v2 must preserve (env var, exit code, route, symbol, path).
- **Pain Point**: A v1 weakness flagged for v2 to address or deliberately defer.

## Success Criteria *(mandatory)*

### Measurable Outcomes

- **SC-001**: 9 child spec directories exist under `.specify/specs/` (006..014), each with all 5 required files.
- **SC-002**: 9 legacy-baseline files exist under `.specify/legacy-baseline/`.
- **SC-003**: `python scripts/validate-ai-contracts.py` reports `"issues": []` after scaffolding lands.
- **SC-004**: Every child spec links back to this parent and to its baseline (verifiable by grep for `005-v2-regeneration` and `legacy-baseline`).
- **SC-005**: Topological order in this spec is acyclic and includes every child.

## Child Specs (v2 Domain Slices)

| # | Spec | Domain | Baseline | Depends on |
|---|------|--------|----------|------------|
| 1 | `006-native-core` | C core / wrapper / DAL | `native-core.md` | — |
| 2 | `007-aspnet-pipeline` | ASP.NET API + interop | `aspnet-pipeline.md` | 006 |
| 3 | `008-typescript-api` | Fastify + Prisma API | `typescript-api.md` | — |
| 4 | `009-react-shared-ui` | React app + shared UI | `react-shared-ui.md` | 007 or 008 (HTTP) |
| 5 | `010-electron-desktop` | Electron desktop runtime | `electron-desktop.md` | 009, 007 |
| 6 | `011-react-native-mobile` | RN/Expo mobile | `react-native-mobile.md` | 007 or 008 |
| 7 | `012-compose-runtime` | Docker/compose/scripts | `compose-runtime.md` | 006..011 (builds) |
| 8 | `013-ai-orchestration` | Workflows/prompts/agents/skills/Spec Kit | `ai-orchestration.md` | 012 |
| 9 | `014-test-coverage` | Tests + coverage | `test-coverage.md` | 006..011 |

**Topological build order**: 006 → 008 → 007 → 009 → 010, 011 → 012 → 014 → 013.

## Assumptions

- v2 is a regeneration, not a refactor; legacy code remains in place until a domain is ready to swap.
- Each domain's spec stands on its own; coordination is via this parent's dependency table.
- The wiki freeze contract from `004-trim-vibe-drift` continues to gate human-facing docs.
- Issue tracker is intentionally empty (user closed all v1 issues); planning happens in `.specify/`.
- Bun, npm, CMake, .NET, Docker Desktop, and WSL2 toolchains remain available on developer machines.
