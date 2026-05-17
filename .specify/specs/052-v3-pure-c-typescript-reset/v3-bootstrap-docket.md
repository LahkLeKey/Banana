# V3 Bootstrap Docket

Date: 2026-05-17
Authority: Spec 052

## Lane-local slice inventory

| Slice | Lane | Retained capability focus | Owned paths | Acceptance evidence |
|---|---|---|---|---|
| `A0-native-baseline` | A | `native-abi-baseline` | `src/native/**` | native configure/build smoke stays green |
| `B0-api-shell-baseline` | B | `api-shell-baseline` | `src/typescript/api/**` | API package build smoke stays green |
| `C0-frontend-shell-baseline` | C | `react-shell-baseline`, `electron-shell-baseline`, `react-native-shell-baseline` | `src/typescript/react/**`, `src/typescript/electron/**`, `src/typescript/react-native/**` | frontend package build smokes stay green |
| `D0-shared-contract-baseline` | D | `shared-contract-shell-baseline` | `src/typescript/shared/ui/**` | shared package build smoke stays green |

## Reserved stitch slices

| Slice | Lanes | Trigger to start | Purpose |
|---|---|---|---|
| `S1-gameplay-contract-stitch` | B + C + D | B0, C0, and D0 all have current evidence | Introduce shared gameplay contracts across API and frontend shells |
| `S2-native-api-boundary-stitch` | A + B | A0 and B0 both have current evidence | Reintroduce native/API gameplay boundary contracts |

## Dependency and merge order

1. Maintain evidence parity for A0, B0, C0, and D0 independently.
2. Start `S1-gameplay-contract-stitch` only after B0, C0, and D0 are green.
3. Start `S2-native-api-boundary-stitch` only after A0 and B0 are green.
4. Keep daily merge cadence for lane-local slices; require dependency review for stitch slices.

## Partial-failure fallback

- If a lane-local slice fails, keep unrelated lane-local slices moving.
- Freeze only the stitch slices that depend on the failed lane.
- Record the blocker, fallback owner, and recovery evidence in `heartbeat-log.md`.

## Handoff template

- Lane:
- Current retained capability:
- Current slice:
- Last green evidence:
- Downstream dependency:
- Blockers:
- Next action:
