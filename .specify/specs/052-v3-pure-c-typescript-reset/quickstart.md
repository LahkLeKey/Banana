# Quickstart: Applying the V3 Reset Baseline

## 1. Confirm the active authority

Use these artifacts first for any downstream V3 planning step:

- `spec.md`
- `plan.md`
- `research.md`
- `data-model.md`
- `contracts/baseline-authority.md`
- `contracts/execution-lanes.md`
- `codebase-scan.md`
- `v3-scope-gate-checklist.md`
- `v3-bootstrap-docket.md`

Do not reopen archived specs or legacy snapshots as primary authority when live source paths disagree.

## 2. Re-run the orchestration gates

From the repository root:

```bash
.specify/scripts/bash/spec-extension-preflight.sh --update-first --json
.specify/scripts/bash/spec-confidence-gate.sh --confidence 92 --step "go-copilot-start" --threshold 80 --notes "startup gate"
```

If confidence falls below 80, stop and get human clarification before continuing.

## 3. Review the retained capability inventory

Use `codebase-scan.md` to confirm:

- the live runtime entry paths,
- the owning lane,
- the excluded support surfaces, and
- the lineage-only references that must not govern new work.

## 4. Run the scope gate

Use `v3-scope-gate-checklist.md` before creating any downstream slice.

Keep a slice in scope only when it:

1. maps to a live runtime entry path,
2. belongs to one execution lane, and
3. has a lane-local validation expectation.

## 5. Assign the lane and handoff target

| Lane | Owned paths | First planning handoff |
|---|---|---|
| A | `src/native/**` | native scaffold stewardship and ABI-safe gameplay work |
| B | `src/typescript/api/**` | API gameplay/session orchestration planning |
| C | `src/typescript/react/**`, `src/typescript/electron/**`, `src/typescript/react-native/**` | frontend runtime-shell planning |
| D | `src/typescript/shared/ui/**` | shared gameplay contract planning |

Only the stitch slices in `contracts/execution-lanes.md` and `v3-bootstrap-docket.md` may span multiple lanes.

## 6. Record planning evidence

For each downstream slice, capture:

- retained capability ID,
- live runtime entry paths,
- owning lane,
- scope-gate decision,
- validation expectation, and
- heartbeat note in `heartbeat-log.md`.

## 7. Run lane-local validation

Use the smallest checks that match the retained scaffold baseline:

```bash
cmake -S src/native -B out/v3-native
cmake --build out/v3-native
npm run build --prefix src/typescript/api
npm run build --prefix src/typescript/react
npm run build --prefix src/typescript/electron
npm run build --prefix src/typescript/react-native
npm run build --prefix src/typescript/shared/ui
```

Use stitch-slice validation only after the dependent lanes already have current evidence.

## 8. Run the shared overworld with Docker Compose

Use a single authoritative runtime room (`overworld`) for local multi-client checks:

```bash
docker compose up --build api-overworld react-overworld
```

Inspect the runtime in one stream:

```bash
docker compose logs -f api-overworld react-overworld
```

Open `http://localhost:5173/session-room` and use **Enter overworld**. Do not require manual room ID entry in this baseline.
