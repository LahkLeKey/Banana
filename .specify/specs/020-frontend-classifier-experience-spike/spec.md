# Feature Specification: Frontend Classifier Experience SPIKE

**Feature Branch**: `[020-frontend-classifier-experience-spike]`
**Created**: 2026-04-26
**Status**: Draft -- ready to investigate
**Type**: SPIKE (investigate -> plan a small family of bounded follow-up specs)
**Triggered by**: Strategic refocus from internal value (native ABI, ML
brain composition) to customer-facing value. The end-to-end banana vs
not-banana experience now exists across three frontend channels (React
web, Electron desktop, React Native mobile) but has never been
audited for cross-channel UX coherence, escalation feedback, or error
recovery from the customer's point of view.

## Problem

The classifier surface has matured behind the API (slices 010-017),
but the customer-facing surfaces have grown channel-by-channel:

- **React (web)**: shows the verdict badge (slice 015) but escalation
  + ensemble embedding passthrough (slice 017) is API-only.
- **Electron (desktop)**: smoke container only; no first-class
  customer flow exists yet for verdict surfacing (tray/notifications,
  paste-classify, drop-to-classify).
- **React Native (mobile)**: scaffold exists but no verdict capture
  flow (camera, share-sheet ingest, offline cue).

Without a deliberate audit, any one of these channels can ship a
divergent verdict story (different copy, different escalation cue,
different error recovery). Customers using more than one channel will
see incoherent product behavior.

## Investigation goals

- **G1**: Channel inventory. For React, Electron, React Native: list
  the actual customer-facing entry points TODAY, the verdict shape
  rendered, the empty/error/loading states, and the gaps. Output:
  cross-channel matrix.
- **G2**: Classify customer journeys. Identify the top 3-5 customer
  intents that span the three channels (paste text -> verdict; drop
  image -> verdict; share-sheet -> verdict; review past verdicts;
  escalate to ensemble explanation). Output: journey table with the
  primary channel + secondary channels per intent.
- **G3**: Pick exactly three follow-up implementation slices, one per
  channel, that move the customer experience forward in a coordinated
  way (same verdict copy, same escalation cue, same error states).
- **G4**: Produce a readiness packet for each follow-up slice
  (slice 023, 024, 025) that the implementation slices can pick up
  without re-litigation.

## Out of Scope

- Implementing the chosen UX (those are slices 023-025).
- Shared design tokens or component v2 work (that is SPIKE 021).
- Offline / persistence work (that is SPIKE 022).
- API or native changes (slices 014-017 already shipped the contracts).

## Deliverables

- `analysis/channel-inventory.md` -- per-channel verdict surface
  matrix (entry points, verdict shape, empty/error/loading states,
  gaps).
- `analysis/customer-journeys.md` -- the 3-5 cross-channel intents and
  their primary/secondary channels.
- `analysis/copy-and-cue-baseline.md` -- the canonical verdict copy,
  escalation cue, and error wording that all three follow-up slices
  must adopt verbatim.
- `analysis/followup-readiness-react.md` -- bounded scope for slice
  023 (React verdict UX polish + escalation surface).
- `analysis/followup-readiness-electron.md` -- bounded scope for
  slice 024 (Electron paste-classify + tray/notification).
- `analysis/followup-readiness-react-native.md` -- bounded scope for
  slice 025 (RN capture flow + share-sheet ingest).
- `analysis/deferred-registry.md` -- any items pushed past slice 025.

## Success Criteria

- Inventory covers every existing customer-facing screen / window /
  surface across the three channels.
- Exactly three follow-up slices recommended, one per channel.
- Each readiness packet is small enough to keep its implementation
  slice <=20 tasks.
- Verdict copy + escalation cue + error wording are pinned in
  `copy-and-cue-baseline.md` so the three follow-up slices cannot
  drift from each other.
- No code, asset, or contract changes land in this SPIKE.

## Validation lane

```
# SPIKE-only -- no build/test required.
# Verification is human review of the seven deliverables above.
ls .specify/specs/020-frontend-classifier-experience-spike/analysis/
```

## Downstream

This SPIKE unlocks three coordinated implementation slices:

- **023 React Verdict UX Polish** -- consume slice 017's
  `/ml/ensemble/embedding` route, render escalation cue per the
  baseline copy, error recovery per the baseline wording.
- **024 Electron Customer Verdict Surface** -- promote the smoke
  container to a real customer surface (paste-classify, tray icon,
  native notification).
- **025 React Native Capture Flow** -- ingest via camera + share-sheet,
  call the verdict API, render the same baseline cue.

## In-scope files

- `src/typescript/react/**` (READ ONLY -- audit input).
- `src/typescript/electron/**` (READ ONLY -- audit input).
- `src/typescript/react-native/**` (READ ONLY -- audit input).
- `src/typescript/shared/ui/**` (READ ONLY -- audit input only;
  evolution of shared/ui itself belongs to SPIKE 021).
- `.specify/specs/020-frontend-classifier-experience-spike/analysis/**`
  (NEW -- SPIKE outputs).
- `.specify/feature.json` (repoint to 020 during execution).
