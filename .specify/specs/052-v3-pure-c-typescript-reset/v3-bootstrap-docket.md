# V3 Bootstrap Docket

Date: 2026-05-18
Authority: Spec 052

## Lane-local planning slices

| Slice | Lane | Capability focus | Owned paths | Acceptance evidence |
|---|---|---|---|---|
| `A0-native-baseline` | A | `native-abi-baseline` | `src/native/**` | CMake configure/build stays green and the first native gameplay slice cites ABI-safe entry paths |
| `B0-api-baseline` | B | `api-shell-baseline` | `src/typescript/api/**` | API shell build stays green and the first API gameplay slice preserves route -> service -> pipeline -> native interop layering |
| `C0-frontend-baseline` | C | `react-shell-baseline`, `electron-shell-baseline`, `react-native-shell-baseline` | `src/typescript/react/**`, `src/typescript/electron/**`, `src/typescript/react-native/**` | Frontend shell builds stay green and the first gameplay UX slice maps cleanly to one frontend shell or a declared stitch slice |
| `D0-shared-contract-baseline` | D | `shared-contract-shell-baseline` | `src/typescript/shared/ui/**` | Shared UI build stays green and the first shared gameplay contract slice remains rooted in `@banana/ui` |

## Reserved stitch slices

| Slice | Lanes | Trigger to start | Purpose |
|---|---|---|---|
| `S1-native-api-handshake` | A + B | A lane-local native gameplay capability requires API-native interop | Preserve the native/API seam without collapsing lane ownership |
| `S2-shared-contract-adoption` | D + consumer lane | A lane-local slice needs an active shared gameplay contract | Keep reusable gameplay primitives in one shared authority |
| `S3-api-frontend-session-handshake` | B + C | Frontend gameplay shells need active API gameplay session flows | Preserve the API/frontend runtime contract |
| `S4-multi-lane-gameplay-integration` | A + B + C + D | One retained gameplay capability requires end-to-end integration | Coordinate a true cross-lane gameplay slice without reintroducing stale authority |

## Dependency and merge order

1. Start A0, B0, C0, and D0 in parallel because they own disjoint active paths.
2. Launch a stitch slice only after every dependency lane has current lane-local evidence.
3. Merge lane-local slices independently when they preserve single-lane ownership.
4. Merge stitch slices only after the contract, dependencies, and validation evidence are explicit.

## Partial-failure fallback

- If one lane loses evidence parity, freeze only the stitch slices that depend on it.
- Keep unrelated lanes moving when their owned paths and evidence remain valid.
- Record the blocker, fallback owner, and recovery evidence in `heartbeat-log.md`.

## Handoff template

- Lane:
- Current retained capability:
- Current slice:
- Last green evidence:
- Downstream dependency:
- Blockers:
- Next action:
