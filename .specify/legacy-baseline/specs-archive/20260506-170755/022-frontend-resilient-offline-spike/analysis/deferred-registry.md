# 022 -- Deferred Registry

## D-022-01 -- Cross-device verdict sync

- **Disposition**: `defer-with-trigger`.
- **Trigger condition**: Customer feedback identifies "I want to see
  my verdicts on another device" OR product introduces account
  signup.
- **Owner when triggered**: api-pipeline-agent (server-side store)
  + react-ui-agent / mobile-runtime-agent (client adapters).
- **Rationale**: Requires a server-side store and an auth boundary.
  The shared primitive's `StorageAdapter` interface keeps the door
  open: a `RemoteSyncAdapter` slots in alongside the per-channel
  ones.

## D-022-02 -- Conflict resolution between two devices

- **Disposition**: `defer-with-trigger`.
- **Trigger condition**: D-022-01 has shipped AND >=1 conflict
  surfaces in the field.
- **Rationale**: No conflicts possible until cross-device sync exists.

## D-022-03 -- Encryption at rest

- **Disposition**: `defer-with-trigger`.
- **Trigger condition**: Verdict payload starts carrying PII OR a
  compliance review flags the local store.
- **Rationale**: Today the verdict payload is verdict label +
  embedding -- no PII. WebCrypto / RN keychain integration would
  add real cost without a stated risk.

## D-022-04 -- iOS background-fetch tuning

- **Disposition**: `defer-with-trigger`.
- **Trigger condition**: Slice 031 ships AND customer feedback
  identifies that drain-on-foreground is not enough.
- **Rationale**: iOS background fetch is deeply best-effort; tuning
  it requires native module work that exceeds slice 031's scope.

## D-022-05 -- Service-worker offline fallback page (web)

- **Disposition**: `defer-with-trigger`.
- **Trigger condition**: Customers report opening the React app
  while offline and seeing a Chrome offline page (not the queued-
  request UX).
- **Rationale**: The queue handles in-session offline. A service
  worker is needed only for cold-start offline. Score this against
  field reports first.
