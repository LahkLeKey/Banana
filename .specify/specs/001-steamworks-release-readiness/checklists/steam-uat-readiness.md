# Steam UAT Readiness Checklist

Target AppID: `4767150`

Note: Steam partner pages are authentication-gated; run launch validation from authenticated Steam client.
Note: Steamworks currently reports no created builds for AppID 4767150; Steam-library launch UAT is blocked until first build upload + publish.
Note: Public storefront work is tracked separately in `steam-storefront-readiness.md`.

## Runtime Access

- [x] Gameplay client reachable at `/session-room`.
- [x] Transition to `/game-engine` verified.
- [x] Build Lab controls visible in-game.

## Core Gameplay Validation

- [x] Class switch updates runtime feedback.
- [x] Allocation preset updates derived stats.
- [x] Gear equip updates stat profile.
- [x] Combo action returns triggered/not-triggered result with bonuses.

## Automated Quality Gates

- [x] Native focused tests passing.
- [x] API route tests passing.
- [x] React gameplay tests passing.

## Steam-Oriented Follow-up

- [x] Define Steam binary packaging output and artifact naming contract.
- [x] Add Steam UAT readiness runner script (`scripts/steam-uat-readiness.sh`).
- [x] Build playable candidate executable at `artifacts/steam-build/dev-candidate/win-unpacked/BananaEngineer.exe`.
- [x] Verify candidate executable launch process starts successfully (smoke check).
- [x] Add Steam UAT report automation (`scripts/generate-steam-uat-report.sh`).
- [x] Add SteamPipe first-build upload helper (`scripts/steam-upload-first-build.ps1`).
- [x] Add local on-demand publish orchestrators (`scripts/steam-publish-on-demand.sh`, `scripts/steam-publish-on-demand.ps1`).
- [x] Add CI workflow-dispatch publish lane (`.github/workflows/steam-publish.yml`).
- [ ] Create first Steamworks build for AppID 4767150 and assign depots/package access.
- [ ] Publish pending Steamworks app changes after build validation.
- [ ] Validate launch behavior from Steam library entry point.
- [ ] Execute UAT matrix against Steam-installed build.
- [ ] Capture screenshots/video/log evidence for UAT signoff.

## Storefront Follow-up

- [ ] Complete the Steam storefront readiness checklist (`steam-storefront-readiness.md`).
- [ ] Sync public Steam page copy with the storefront contract.
- [ ] Sync AI disclosure, controller support, and system requirements with release evidence.
