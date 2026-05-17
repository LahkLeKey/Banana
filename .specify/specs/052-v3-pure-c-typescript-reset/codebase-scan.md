# Codebase Scan: Spec 052 Active Runtime Inventory

Date: 2026-05-17

## Active retained capability inventory

| Capability ID | Lane | Surface | Live runtime entry paths | Evidence record summary | Retained decision |
|---|---|---|---|---|---|
| `native-abi-baseline` | A | native | `src/native/CMakeLists.txt`, `src/native/include/banana_native_v3.h`, `src/native/scaffold/native_entry.c` | Native build and exported ABI scaffold remain the only live gameplay-adjacent implementation surface | Retained |
| `api-shell-baseline` | B | api | `src/typescript/api/package.json`, `src/typescript/api/src/index.ts` | API package exists as a scaffold orchestration shell with a placeholder entry point and build script | Retained |
| `react-shell-baseline` | C | frontend | `src/typescript/react/package.json`, `src/typescript/react/src/index.ts` | React shell exists as a placeholder runtime surface | Retained |
| `electron-shell-baseline` | C | frontend | `src/typescript/electron/package.json`, `src/typescript/electron/main.js` | Electron shell exposes the only live JavaScript runtime boot function in the frontend lane | Retained |
| `react-native-shell-baseline` | C | frontend | `src/typescript/react-native/package.json`, `src/typescript/react-native/index.ts` | React Native shell exists as a placeholder runtime surface | Retained |
| `shared-contract-shell-baseline` | D | shared-contract | `src/typescript/shared/ui/package.json`, `src/typescript/shared/ui/src/index.ts` | Shared UI package exposes the baseline shared contract/export surface | Retained |

## Supporting but excluded surfaces

| Surface | Current state | Scope-gate result | Reason |
|---|---|---|---|
| `scripts/README.md` | Placeholder delivery guidance | Excluded | Not a customer-facing runtime surface |
| `tests/README.md` | Placeholder testing guidance | Excluded | No retained gameplay capability by itself |
| `docker/README.md` | Placeholder runtime/delivery guidance | Excluded | Delivery support only |
| `docs/` and `data/` placeholders | Supporting context only | Excluded | Not active runtime authority |

## Non-authoritative lineage references

| Reference | Status | Migration note |
|---|---|---|
| `.specify/legacy-baseline/specs-archive/2026-05-v3-reset` | Archived lineage | Keep for traceability only |
| `.legacy/**` | Archived lineage | Use only for history and comparison |
| Historical claims such as `startSession`, `apiHealth`, or `normalizeMovement` | Non-authoritative | Do not treat as retained capability names until live source paths expose them again |

## Evidence-capture rules

1. Every retained capability must cite at least one live runtime entry path.
2. Every retained capability must map to exactly one execution lane.
3. Excluded surfaces stay visible in this scan so downstream planners can explain why they do not govern baseline truth.
