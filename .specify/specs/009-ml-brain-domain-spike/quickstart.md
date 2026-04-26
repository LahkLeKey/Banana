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

## 3. Produce SPIKE artifacts

Required outputs:

- canonical domain definitions (`left`, `right`, `full`)
- primary family mappings (`regression`, `binary`, `transformer`)
- normalized comparison matrix
- readiness packets for each domain

## 4. Validate readiness criteria

Review artifacts and confirm:

- all model families mapped once as primary
- each domain has bounded follow-up recommendations
- required validation lanes for follow-up slices are explicit

## 5. Prepare handoff to task generation

After review is complete, proceed to `specify tasks` for implementation-oriented slices selected from domain readiness packets.
