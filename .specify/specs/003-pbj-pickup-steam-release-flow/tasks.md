# Tasks: PBJ Pickup Celebration and Steam Release Tracking

## Phase 1: Gameplay PBJ Pickup

- [x] Add session-scoped PBJ pickup availability state in GameEnginePage.
- [x] Track local player world position from snapshot entities for pickup distance checks.
- [x] Add pickup interaction UI with distance feedback and collect action gating.
- [x] Consume pickup on success and emit runtime interaction status message.

## Phase 2: Celebration Visual FX

- [x] Add dancing banana particle event generation on successful pickup.
- [x] Render non-blocking animated particle overlay in viewport.
- [x] Ensure particle effect auto-clears and does not persist across cleanup.
- [x] Validate no TypeScript compile errors in React workspace after changes.

## Phase 3: Steam Publish and Release Tracking Scaffold

- [x] Create new feature spec package for PBJ + release tracking execution.
- [ ] Add concrete runbook mapping for local publish, CI dispatch, and HTTP upload fallback commands.
- [ ] Add checklist section for Steam library launch verification (post-upload) and configuration triage notes.
- [ ] Add PR closeout template section linking BuildID/ManifestID and blocker status.
- [ ] Record first execution pass evidence under this feature once next publish cycle runs.

## Validation Commands

```bash
# React type validation
cd src/typescript/react && bunx tsc -p tsconfig.json --noEmit
```
