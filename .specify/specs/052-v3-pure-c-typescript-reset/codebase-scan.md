# Codebase Scan: Spec 052 Active Runtime Inventory

Date: 2026-05-18

## Active retained capability inventory

| Capability ID | Lane | Surface | Live runtime entry paths | Evidence record summary | Retained decision |
|---|---|---|---|---|---|
| `native-abi-baseline` | A | native | `src/native/CMakeLists.txt`, `src/native/include/banana_native_v3.h`, `src/native/scaffold/native_entry.c` | Native build and exported ABI scaffold; exports only `banana_native_v3_abi_version()` and `banana_native_v3_ping()` | Retained |
| `api-shell-baseline` | B | api | `src/typescript/api/package.json`, `src/typescript/api/src/index.ts` | Package shell with placeholder build/dev scripts and scaffold export | Retained |
| `react-shell-baseline` | C | frontend | `src/typescript/react/package.json`, `src/typescript/react/src/index.ts` | React shell with placeholder build/dev scripts and scaffold export | Retained |
| `electron-shell-baseline` | C | frontend | `src/typescript/electron/package.json`, `src/typescript/electron/main.js` | Electron shell with minimal runtime boot function | Retained |
| `react-native-shell-baseline` | C | frontend | `src/typescript/react-native/package.json`, `src/typescript/react-native/index.ts` | React Native shell with placeholder build/dev scripts and scaffold export | Retained |
| `shared-contract-shell-baseline` | D | shared-contract | `src/typescript/shared/ui/package.json`, `src/typescript/shared/ui/src/index.ts` | Shared UI package shell with single root export | Retained |

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
| Superseded Specs 050 and 051 | Archived lineage | Do not reopen for baseline authority decisions |
| Historical capability claims such as `startSession`, `apiHealth`, or `normalizeMovement` | Non-authoritative | Do not treat as retained capability names until live source paths expose them again |

## Evidence-capture rules

1. Every retained capability must cite at least one live runtime entry path.
2. Every retained capability must map to exactly one execution lane.
3. Excluded surfaces stay visible in this scan so downstream planners can explain why they do not govern baseline truth.
4. Archived or speculative references may support rationale only; they cannot be sole evidence for retained scope.
