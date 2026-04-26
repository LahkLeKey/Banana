# Quickstart: ML Brain Domain SPIKE

## Purpose

Generate and review the brain-domain SPIKE outputs needed to start follow-up implementation slices.

## Prerequisites

- Repository root: `C:/Github/Banana`
- Active feature pointer set to `.specify/specs/009-ml-brain-domain-spike`
- Access to current native ML source surfaces for evidence-backed mapping

## 1. Confirm active feature

```bash
cd /c/Github/Banana
cat .specify/feature.json
```

Expected `feature_directory`:

- `.specify/specs/009-ml-brain-domain-spike`

## 2. Inspect in-scope ML surfaces

```bash
cd /c/Github/Banana
find src/native/core/domain -type f | sort
find src/native/wrapper/domain/ml -type f | sort
```

## 3. Review SPIKE artifacts (generated 2026-04-26)

All artifacts are in `.specify/specs/009-ml-brain-domain-spike/analysis/`.

### Phase 1 — Setup
```bash
ls .specify/specs/009-ml-brain-domain-spike/analysis/
```

### Phase 2 — Foundational method
```bash
cat .specify/specs/009-ml-brain-domain-spike/analysis/terminology-rules.md
cat .specify/specs/009-ml-brain-domain-spike/analysis/comparison-dimensions.md
cat .specify/specs/009-ml-brain-domain-spike/analysis/confidence-policy.md
```

### Phase 3 — Domain definitions (US1)
```bash
cat .specify/specs/009-ml-brain-domain-spike/analysis/brain-domain-definitions.md
cat .specify/specs/009-ml-brain-domain-spike/analysis/model-family-mappings.md
cat .specify/specs/009-ml-brain-domain-spike/analysis/domain-boundaries.md
cat .specify/specs/009-ml-brain-domain-spike/analysis/domain-risk-register.md
```

### Phase 4 — Tradeoff matrix (US2)
```bash
cat .specify/specs/009-ml-brain-domain-spike/analysis/model-family-tradeoff-matrix.md
cat .specify/specs/009-ml-brain-domain-spike/analysis/domain-fit-scorecards.md
cat .specify/specs/009-ml-brain-domain-spike/analysis/ranking-rationale.md
cat .specify/specs/009-ml-brain-domain-spike/analysis/uncertainty-impact-notes.md
```

### Phase 5 — Readiness packets (US3)
```bash
cat .specify/specs/009-ml-brain-domain-spike/analysis/readiness-packets.md
cat .specify/specs/009-ml-brain-domain-spike/analysis/readiness-packet-left-brain.md
cat .specify/specs/009-ml-brain-domain-spike/analysis/readiness-packet-right-brain.md
cat .specify/specs/009-ml-brain-domain-spike/analysis/readiness-packet-full-brain.md
cat .specify/specs/009-ml-brain-domain-spike/analysis/validation-lane-plan.md
```

## 4. Validate readiness criteria

Review artifacts and confirm:

- [x] All model families mapped once as primary — see [model-family-mappings.md](analysis/model-family-mappings.md)
- [x] Each domain has bounded follow-up recommendations — see [readiness-packets.md](analysis/readiness-packets.md)
- [x] Required validation lanes for follow-up slices are explicit — see [validation-lane-plan.md](analysis/validation-lane-plan.md)
- [x] Success criteria cross-checked — see [success-criteria-evidence.md](analysis/success-criteria-evidence.md)

## 5. Prepare handoff to implementation

Recommended execution order for follow-up slices:

1. **Left Brain** (Regression) — lowest risk; first deliverable is feature contract
2. **Right Brain** (Binary) — core product value; include calibration from start
3. **Full Brain** (Transformer) — highest complexity; requires new native test pattern

After domain selection, proceed to `specify specify` (new feature spec per slice) or `specify tasks` if the selected packet is bounded enough for direct task generation.
