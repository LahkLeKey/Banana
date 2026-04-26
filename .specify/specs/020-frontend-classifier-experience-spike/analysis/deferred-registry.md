# 020 -- Deferred Registry

Items pushed past slice 025 with explicit triggers.

## D-020-01 -- Drop-to-classify (image) on Electron

- **Disposition**: `defer-with-trigger`.
- **Trigger condition**: Slice 024 ships AND >=1 customer-facing
  request explicitly asks for image ingest on desktop. Tracked by
  product feedback channel.
- **Owner when triggered**: electron-agent + react-ui-agent (drag
  surface lives in renderer).
- **Rationale**: Image ingest requires an upload contract that does
  not exist on the API today; pulling it into 024 would balloon
  scope and force a synchronous API change. Text-paste verdict ships
  first.

## D-020-02 -- Camera capture on React Native

- **Disposition**: `defer-with-trigger`.
- **Trigger condition**: Slice 025 ships AND >=1 customer-facing
  request explicitly asks for camera capture on mobile.
- **Owner when triggered**: mobile-runtime-agent + api-pipeline-agent
  (image upload contract).
- **Rationale**: Same upload-contract gap as D-020-01. Share-sheet
  text ingest delivers the immediate customer value first.

## D-020-03 -- Cross-session verdict history

- **Disposition**: `routed-to-spike`.
- **Routed to**: SPIKE 022 Frontend Resilient Offline.
- **Rationale**: J4 (history) is a persistence concern that overlaps
  with offline queueing. Folding it into SPIKE 022's storage
  contract avoids two parallel persistence layers.

## D-020-04 -- Escalation panel actions (share, rate, save)

- **Disposition**: `defer-with-trigger`.
- **Trigger condition**: Slices 023-025 ship AND customer feedback
  identifies a primary action.
- **Rationale**: The baseline pins the panel to informational-only
  content. Adding actions without a stated customer need risks
  shipping unused affordances.

## D-020-05 -- Token-driven escalation color

- **Disposition**: `routed-to-spike`.
- **Routed to**: SPIKE 021 Frontend Shared Design System.
- **Rationale**: The amber escalation color is currently a literal
  in the baseline. SPIKE 021 will replace it with a token; slices
  026-028 will retrofit slices 023-025.
