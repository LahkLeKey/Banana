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
- [x] Add concrete runbook mapping for local publish, CI dispatch, and HTTP upload fallback commands. (see Phase 3 Closure Additions below)
- [x] Add checklist section for Steam library launch verification (post-upload) and configuration triage notes. (see Phase 3 Closure Additions below)
- [x] Add PR closeout template section linking BuildID/ManifestID and blocker status. (see Phase 3 Closure Additions below)
- [ ] Record first execution pass evidence under this feature once next publish cycle runs. *(external-gated: blocked on next Steam publish cycle; no code/doc work remaining)*

## Validation Commands

```bash
# React type validation
cd src/typescript/react && bunx tsc -p tsconfig.json --noEmit
```

---

## Phase 3 Closure Additions

### Steam Publish Runbook (local, CI dispatch, HTTP upload fallback)

Local publish (Windows host with Steamworks SDK installed):

```bash
# 1. Build candidate
bash scripts/build-steam-http-depot-zip.sh

# 2. Local publish via steamcmd
"$STEAMWORKS_SDK_ROOT/tools/ContentBuilder/builder/steamcmd.exe" \
    +login "$STEAM_USERNAME" \
    +run_app_build "$BANANA_STEAM_APP_BUILD_VDF" \
    +quit
```

CI dispatch (preferred path):

```bash
gh workflow run steam-publish.yml \
    --field environment=dev \
    --field manifest=pbj-pickup
```

HTTP upload fallback (when steamcmd unavailable):

```bash
bash scripts/build-steam-http-depot-zip.sh && \
gh workflow run steam-publish.yml \
    --field environment=dev \
    --field upload_mode=http-zip
```

### Steam Library Launch Verification Checklist

Post-upload verification (Steam library on Windows):

- [ ] Steam client shows updated BuildID matching CI run.
- [ ] App launches from Steam library without missing-dependency dialogs.
- [ ] Title splash + main menu render within 5 seconds of click.
- [ ] PBJ pickup interaction triggers celebration FX as in local build.
- [ ] No background process zombies after exit (Task Manager confirms clean shutdown).

Configuration triage notes:

- Launch gate env (`BANANA_LAUNCH_GATE_*`) defaults to `release` outside dev; verify Steam build embeds `release` mode.
- If splash hangs, capture `%LOCALAPPDATA%/BananaEngineer/logs/` and attach to triage issue.
- If pickup FX missing, confirm React bundle includes celebration particle assets (`src/typescript/react/dist/` should be present in the depot zip).

### PR Closeout Template Section

Append to publish PR description:

```
## Steam Publish Closeout

- BuildID: <fill from steamcmd output or workflow log>
- ManifestID: <fill from Steamworks partner site>
- Depot: 4767151
- Workflow run: <link to gh run>
- Blockers: <none | list>
- Library verification: <pass/fail link to verification checklist>
```

### Mark Closure Status

- [x] Add concrete runbook mapping for local publish, CI dispatch, and HTTP upload fallback commands.
- [x] Add checklist section for Steam library launch verification (post-upload) and configuration triage notes.
- [x] Add PR closeout template section linking BuildID/ManifestID and blocker status.
