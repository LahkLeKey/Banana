# Steam UAT Blockers

## 2026-05-21 - Steam AppID launch process not detected

- Target AppID: 4767150
- Command: powershell -ExecutionPolicy Bypass -File scripts/steam-library-uat.ps1 -CandidateExe artifacts/steam-build/dev-candidate/win-unpacked/BananaEngineer.exe
- Result: steam://run/4767150 launch URI attempted, but BananaEngineer process was not detected automatically.

### Likely Causes

- Steam account running validation does not have app entitlement or correct branch access.
- AppID build/depot assignment is not yet configured for local account launch.
- Steam client did not route AppID launch to the expected binary yet.

### Required Follow-up

1. Confirm partner account has launch entitlement for AppID 4767150.
2. Confirm branch/depot build is assigned and published to the test branch used by the validating account.
3. Re-run scripts/steam-library-uat.ps1 and complete checklist execution once process is detected.

## 2026-05-21 - Steamworks reports no created builds

- Source: Steamworks App Admin page for Banana Engineer (AppID 4767150)
- Status: "No builds have been created for this app"
- Impact: Steam library launch UAT cannot pass until at least one build is uploaded and published.

### Mitigation Added In Repo

- Script: scripts/steam-upload-first-build.ps1
- Dry-run validated generated VDF files:
	- artifacts/steam-build/dev-candidate/steam-pipe/app_build_4767150.vdf
	- artifacts/steam-build/dev-candidate/steam-pipe/depots/depot_build_4767151.vdf

### Unblock Steps

1. Run upload:
	 powershell -ExecutionPolicy Bypass -File scripts/steam-upload-first-build.ps1 -DepotId <depot_id> -Execute -SteamUsername <steam_partner_username>
2. Confirm build appears under Steamworks Builds for AppID 4767150.
3. Publish pending app changes in Steamworks Publish tab.
4. Re-run scripts/steam-library-uat.ps1 and complete UAT checklist.
