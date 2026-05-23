# Quickstart: Gameplay Visual Verification

## Goal

Bring up the gameplay runtime, enter the overworld, and verify class/build/gear/combo interactions visually.

## Option A: Local Fallback Runtime (Recommended for iterative development)

1. Start API service:

```bash
cd src/typescript/api
BANANA_ENGINE_ADAPTER=inmemory HOST=0.0.0.0 PORT=8081 bun run dev
```

2. Start React client in another terminal:

```bash
cd src/typescript/react
VITE_BANANA_API_BASE_URL=http://localhost:8081 bun run dev -- --host 0.0.0.0 --port 5173
```

3. Open the client:

- URL: http://localhost:5173/session-room

4. Enter game flow:

- Click "Enter overworld".
- Confirm navigation to `/game-engine`.
- Confirm `Build Lab` panel is visible.

5. Visual gameplay checks:

- Click `Ranger` and verify class update message.
- Click `Apply 3/1/2` and verify stats update.
- Click `Equip T2 Weapon` and verify attack/stat changes.
- Click `Try Combo` and verify combo result message.

## Option B: Container Runtime Channel

From repo root:

```bash
docker compose up -d api-overworld react-overworld
```

Client URL:

- http://localhost:5173/session-room

If API bootstrap fails due script line endings in containerized Linux shell, normalize before execution in startup command (already applied in compose command path).

## Shutdown

Local runtime:

- Stop terminal processes with Ctrl+C.

Container runtime:

```bash
docker compose down
```

## Steam Candidate Build And Validation

1. Build playable candidate executable:

```bash
bash scripts/build-steam-playable-candidate.sh
```

2. Run readiness lanes against produced executable:

```bash
bash scripts/steam-uat-readiness.sh artifacts/steam-build/dev-candidate/win-unpacked/BananaEngineer.exe
```

3. Generate SteamPipe config for first Steamworks build (AppID 4767150):

```powershell
powershell -ExecutionPolicy Bypass -File scripts/steam-upload-first-build.ps1 -DepotId <your_depot_id>
```

4. Upload the first SteamPipe build after config review:

```powershell
powershell -ExecutionPolicy Bypass -File scripts/steam-upload-first-build.ps1 -DepotId <your_depot_id> -Execute -SteamUsername <steam_partner_username>
```

Alternative local orchestrators:

```powershell
powershell -ExecutionPolicy Bypass -File scripts/steam-publish-on-demand.ps1 -DepotId <your_depot_id> -BuildCandidate -SteamCredential (Get-Credential)
```

```bash
DEPOT_ID=<your_depot_id> STEAM_USERNAME=<steam_partner_username> STEAM_PASSWORD=<steam_partner_password> BUILD_CANDIDATE=true scripts/steam-publish-on-demand.sh
```

5. In Steamworks Publish tab, submit unpublished changes once build/depot assignment is correct.

### CI/CD Publish Lane

- Workflow: `.github/workflows/steam-publish.yml`
- Trigger: GitHub Actions `workflow_dispatch`
- Required repository secrets:
	- `STEAM_USERNAME`
	- `STEAM_PASSWORD`
	- `STEAM_GUARD_CODE` (optional, when Steam Guard challenge is required)
- Required workflow input:
	- `depot_id`

6. Steam-library specific validation remains manual (pending until first build exists in Steamworks):

- Add the produced executable to Steam library entry and verify launch path.
- Execute checklist in `.specify/specs/001-steamworks-release-readiness/checklists/steam-uat-readiness.md`.

## Storefront Copy Target

Use [steam-storefront-copy.md](contracts/steam-storefront-copy.md) as the canonical
reference for Steam page text, disclosure language, controller support wording,
and minimum system requirements.

When the store page or gameplay claims change, update the storefront contract and
the binary contract together so the public page stays aligned with the shipped
build.

## Steam Library UAT Helpers

1. Generate a timestamped readiness report and log bundle:

```bash
bash scripts/generate-steam-uat-report.sh artifacts/steam-build/dev-candidate/win-unpacked/BananaEngineer.exe
```

2. Windows helper for Steam library launch workflow:

```powershell
powershell -ExecutionPolicy Bypass -File scripts/steam-library-uat.ps1 -CandidateExe artifacts/steam-build/dev-candidate/win-unpacked/BananaEngineer.exe
```

Default helper behavior now attempts `steam://run/4767150` before falling back to non-Steam guidance.

Optional (if Steam game id is known):

```powershell
powershell -ExecutionPolicy Bypass -File scripts/steam-library-uat.ps1 -CandidateExe artifacts/steam-build/dev-candidate/win-unpacked/BananaEngineer.exe -SteamGameId <id>
```

## Storefront Readiness Workstreams

- Review `checklists/steam-storefront-readiness.md` for the public Steam page lanes.
- Keep controller support, AI disclosure, system requirements, and capsule assets aligned with `contracts/steam-storefront-copy.md`.
- Review `contracts/steam-storefront-assets.md` before changing any page image set.
- Use the storefront checklist as the release gate for the page copy while `steam-uat-readiness.md` remains the gate for the playable build.
