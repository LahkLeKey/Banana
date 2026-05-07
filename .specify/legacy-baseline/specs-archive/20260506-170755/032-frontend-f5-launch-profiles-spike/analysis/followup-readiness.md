# Follow-up Readiness -- Slice 035 (deferred)

**Trigger**: A contributor asks for breakpoints in the React/Electron
renderer, or asks for one-key test-runner launches.

## Scope (when triggered)

- Add `chrome` / `msedge` `attach` configurations targeting the
  Vite dev server (port 5173) for renderer-side breakpoints.
- Add an Electron renderer attach config (electron `--remote-debugging-port`).
- Add `bun test` and `dotnet test` launch entries so F5 can run
  the test runners with the debugger attached.

## Out of scope (for slice 035)

- Container-side debugger attach (still owned by container scripts).
- Mobile native debugger (Xcode / Android Studio remain authoritative).

## Prerequisites

- SPIKE 032 launch.json is the structural baseline; slice 035 only
  adds entries.
- No changes to env-var seam, compound model, or container
  boundary.
