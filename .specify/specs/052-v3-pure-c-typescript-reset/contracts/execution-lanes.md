# Contract: V3 Execution Lanes

## Purpose

Define the bounded ownership model for downstream V3 implementation planning.

## Lane definitions

| Lane | Owner boundary | Owned paths | Retained capability set | Lane-local validation |
|---|---|---|---|---|
| A | Native gameplay | `src/native/**` | Native ABI baseline and build surface | `cmake -S src/native -B out/v3-native` and `cmake --build out/v3-native` |
| B | API gameplay | `src/typescript/api/**` | API gameplay orchestration shell | package build smoke for `src/typescript/api` |
| C | Frontend gameplay | `src/typescript/react/**`, `src/typescript/electron/**`, `src/typescript/react-native/**` | React, Electron, and React Native runtime shells | package build smoke for each frontend shell |
| D | Shared gameplay contracts | `src/typescript/shared/ui/**` | Shared gameplay-facing contract/export surface | package build smoke for `src/typescript/shared/ui` |

## Ownership rules

1. Every retained capability belongs to exactly one lane until a declared stitch slice begins.
2. A lane may not claim or modify another lane's owned paths during a lane-local slice.
3. Cross-lane work must be expressed as a stitch slice with explicit dependencies and fallback handling.
4. Documentation changes may span lanes only when they update active planning authority for the same retained capability set.

## Stitch points

The reset baseline reserves two explicit cross-lane joins:

- **Gameplay contract stitch**: Lane B + Lane C + Lane D
- **Native/API boundary stitch**: Lane A + Lane B

## Dependency order and merge cadence

1. Finish lane-local scaffold stewardship in A, B, C, and D.
2. Start the gameplay-contract stitch only after B, C, and D have current evidence.
3. Start the native/API stitch only after A and B have current evidence.
4. Merge lane-local slices independently; merge stitch slices only when all dependencies are green.

## Fallback rules

- If one lane loses evidence parity, freeze only the stitch slices that depend on it.
- Keep unrelated lanes moving when their owned paths and evidence remain valid.
- Record the blocker and owning lane in `heartbeat-log.md` before handoff.

## Out-of-scope rules

The following do not belong to any current V3 execution lane:

- training/workbench/control-plane slices
- workflow-only changes with no gameplay runtime effect
- documentation-only changes that do not update active runtime/planning authority
- legacy or archived implementation carry-forward without current source evidence
