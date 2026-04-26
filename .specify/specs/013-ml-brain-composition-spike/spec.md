# Feature Specification: ML Brain Composition & Productionization SPIKE

**Feature Branch**: `[013-ml-brain-composition-spike]`
**Created**: 2026-04-26
**Status**: Draft
**Input**: User description: "add a spike for the next follow up specs" (post 010/011/012 cascade)

## Context

The 009 ML Brain Domain SPIKE produced canonical domains (Left = Regression,
Right = Binary, Full = Transformer) and was implemented in three runnable C
slices:

- 010 -- Left Brain Regression Runnable C Code (25/25, anchors locked)
- 011 -- Right Brain Binary Runnable C Code (25/25, threshold guidance locked)
- 012 -- Full Brain Transformer Runnable C Code (30/30, embedding + attention
  diagnostics surfaced via additive ABI `_ex` entrypoint)

Each brain is now individually contract-locked and ctest-validated (native lane
7/7). What is missing is a deliberate, ordered plan for **what to build next**
across the three brains as a coordinated set instead of three parallel siloes.

## In Scope *(mandatory)*

- Identify and rank the next implementation slices that depend on or
  meaningfully compose two or more brains.
- Decide whether the immediate next slice is composition (ensemble routing),
  consumer pull-through (ASP.NET pipeline + Electron/React surfaces), or
  diagnostics productionization (U-001 quantized embedding, attention
  surfacing in the wrapper JSON).
- Produce one bounded, selection-ready follow-up readiness packet per
  candidate slice.
- Confirm or revise the deferred follow-up registry from 009/012
  (especially U-001 quantized embedding).

## Out of Scope *(mandatory)*

- Implementing any of the candidate slices in this SPIKE.
- Re-opening the canonical Left/Right/Full domain assignments from 009.
- Changing locked JSON output contracts of `regression`, `binary`,
  or `transformer` model strings.
- Replacing the additive ABI bumping discipline established in 011/012
  (`BANANA_WRAPPER_ABI_VERSION_MAJOR=2, MINOR=2` at slice close).

## User Scenarios & Testing *(mandatory)*

### User Story 1 -- Composition Strategy Decision (Priority: P1)

As a maintainer, I need a single recommended ensemble/composition strategy so
the next implementation slice can begin without re-litigating routing rules.

**Why this priority**: Composition unblocks downstream value (ASP.NET pipeline,
Electron UI signals) and is the highest-leverage move now that all three
brains are individually runnable.

**Independent Test**: SPIKE artifact selects exactly one composition strategy
(e.g. weighted vote, transformer-as-arbiter, gated cascade) with explicit
rationale grounded in the 010/011/012 anchor evidence.

**Acceptance Scenarios**:

1. **Given** the locked anchors from 010/011/012, **When** the composition
   strategy is reviewed, **Then** the strategy explains how disagreements
   between Left, Right, and Full brains are resolved deterministically.
2. **Given** the chosen strategy, **When** edge anchors are inspected
   (e.g. transformer rejects "yellow plastic toy banana" decoy that binary
   accepts), **Then** the strategy's expected behavior on each anchor is
   documented.

---

### User Story 2 -- Consumer Pull-Through Mapping (Priority: P2)

As a planner, I need the ASP.NET pipeline + React/Electron surface mapped to
the brain ABI so consumer slices can be sequenced after composition lands.

**Why this priority**: Native value is unrealized until a real consumer calls
the new diagnostics ABI; mapping clarifies which managed/JS layer changes are
required.

**Independent Test**: Each brain's public ABI export is mapped to a concrete
consumer (controller, pipeline step, UI component) and a single bounded
slice is named for each non-trivial gap.

**Acceptance Scenarios**:

1. **Given** `banana_classify_banana_transformer_ex`, **When** consumer
   mapping is reviewed, **Then** at least one ASP.NET pipeline step is named
   that would surface the embedding fingerprint or attention weights.
2. **Given** the React UI surface, **When** consumer mapping is reviewed,
   **Then** the displayed signal (e.g. attention badge, ensemble verdict)
   is named with the matching component path.

---

### User Story 3 -- Deferred Follow-Up Registry Refresh (Priority: P3)

As a delivery reviewer, I need the deferred-followup registry from 009/012
re-scored against current state so we either green-light or re-defer each
item with current evidence.

**Why this priority**: U-001 (quantized embedding) and any other deferred
items should not silently rot; SPIKE re-scores them with anchors now in hand.

**Independent Test**: Each currently-deferred item is annotated with one of:
{`ready-now`, `defer-with-trigger`, `drop`} and a one-line rationale.

**Acceptance Scenarios**:

1. **Given** the U-001 quantized embedding entry from
   `012-full-brain-transformer-runnable/analysis/residual-followups.md`,
   **When** the SPIKE re-scores it, **Then** the next-action decision is
   recorded with a concrete trigger (e.g. "open after first cross-process
   embedding consumer lands").
2. **Given** any other 009-era deferred item still open, **When** SPIKE
   review runs, **Then** every entry has a non-empty disposition.

### Edge Cases

- What happens when the three brains disagree on a payload that has no
  recorded anchor? (Composition strategy must specify a default tie-break.)
- What happens when downstream consumers want the diagnostics on the
  hot path? (Must keep the FB-R03 default-off cost guarantee intact.)
- What happens when ABI bump pressure increases (multiple additive exports
  in one slice)? (Must still respect the additive-only convention from
  011/012.)

## Requirements *(mandatory)*

- **C-R01**: SPIKE MUST produce one recommended composition strategy with
  named tie-break rules, anchored to the 010/011/012 calibration tables.
- **C-R02**: SPIKE MUST produce a brain-to-consumer mapping that names at
  least one bounded slice per non-trivial gap (ASP.NET pipeline step,
  Electron preload bridge, React component).
- **C-R03**: SPIKE MUST refresh the deferred-followup registry with a
  disposition for each open item (`ready-now` / `defer-with-trigger` / `drop`).
- **C-R04**: SPIKE MUST NOT change runtime behavior, ABI exports, or
  validation lanes; it is read-only across `src/native/**`.
- **C-R05**: SPIKE deliverables MUST be implementation-ready: each
  recommended next slice has objective, in-scope files, validation lane,
  and contract risk notes.

## Deliverables

- `spec.md` (this file)
- `research.md` -- decisions for composition strategy, consumer mapping,
  and deferred-registry refresh
- `plan.md` -- ordered next-slice candidates with readiness packets
- `tasks.md` -- SPIKE-only tasks (no implementation tasks)
