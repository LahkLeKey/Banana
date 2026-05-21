# Contract: V3 Execution Lanes

## Purpose

Define the bounded ownership model for downstream V3 planning from the reset baseline.

## Lane definitions

| Lane | Owner boundary | Owned paths | Current retained capabilities | Downstream planning focus | Lane-local validation |
|---|---|---|---|---|---|
| A | Native gameplay | `src/native/**` | `native-abi-baseline` | Native gameplay logic, ABI-safe exports, and native-only domain growth rooted in the current C scaffold | `cmake -S src/native -B out/v3-native && cmake --build out/v3-native` |
| B | API gameplay | `src/typescript/api/**` | `api-shell-baseline` | Route -> service -> pipeline -> native interop planning rooted in the current API shell | `npm run build --prefix src/typescript/api` |
| C | Frontend gameplay | `src/typescript/react/**`, `src/typescript/electron/**`, `src/typescript/react-native/**` | `react-shell-baseline`, `electron-shell-baseline`, `react-native-shell-baseline` | Runtime-shell, gameplay UX, and client runtime orchestration planning rooted in the current frontend shells | `npm run build --prefix src/typescript/react && npm run build --prefix src/typescript/electron && npm run build --prefix src/typescript/react-native` |
| D | Shared gameplay contracts | `src/typescript/shared/ui/**` | `shared-contract-shell-baseline` | Shared gameplay data contracts, reusable primitives, and cross-lane contract planning rooted in `@banana/ui` | `npm run build --prefix src/typescript/shared/ui` |

## Ownership rules

1. Every retained capability belongs to exactly one lane until a declared stitch slice begins.
2. A lane may not claim or modify another lane's owned paths during a lane-local slice.
3. Cross-lane work must be expressed as a stitch slice with explicit dependencies and evidence.
4. Documentation changes may span lanes only when they update active planning authority for the same retained capability set.
5. Archived lineage may inform rationale, but it cannot assign ownership or override live source evidence.

## Reserved stitch points

The reset baseline reserves four cross-lane joins for future work:

- **S1 — Native/API gameplay handshake**: Lane A + Lane B — native gameplay capability is exposed through an API-native interop seam.
- **S2 — Shared contract adoption**: Lane D + one consuming lane — a shared gameplay contract becomes required by a native, API, or frontend slice.
- **S3 — Frontend/API gameplay session handshake**: Lane B + Lane C — frontend gameplay shells consume active API gameplay session flows.
- **S4 — Multi-lane gameplay integration**: Lane A + Lane B + Lane C + Lane D — an active gameplay capability requires end-to-end integration across all lanes.

## Dependency order and merge cadence

1. Start with lane-local baseline slices in A, B, C, and D using live scaffold evidence only.
2. Launch a stitch slice only after each dependency lane has current lane-local evidence.
3. Merge lane-local slices independently when they preserve scope and authority rules.
4. Merge stitch slices only when all dependent lanes are green and the stitch contract is explicit.

## Fallback rules

- If one lane loses evidence parity, freeze only the stitch slices that depend on it.
- Keep unrelated lanes moving when their owned paths and evidence remain valid.
- Record the blocker and owning lane in `heartbeat-log.md` before handoff.

## Out-of-scope rules

The following do not belong to any current V3 execution lane:

- training, workbench, or control-plane slices
- workflow-only changes with no gameplay runtime effect
- support placeholders under `scripts`, `tests`, `docker`, `docs`, or `data`
- archived specs, legacy snapshots, and historical claims without live source evidence
