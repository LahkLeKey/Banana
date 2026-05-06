# Quickstart: Frontend Config Drift Closure

## 1. Start canonical frontend React flow

```bash
bash scripts/compose-run-profile.sh --profile runtime --action up
bash scripts/compose-profile-ready.sh --profile runtime
bash scripts/compose-run-profile.sh --profile apps --action up
bash scripts/compose-profile-ready.sh --profile apps
```

## 2. Verify config contract layers

```bash
docker compose --profile apps config | grep -E "VITE_BANANA_API_BASE_URL|BANANA_API_BASE_URL|EXPO_PUBLIC_BANANA_API_BASE_URL"
docker compose exec react-app printenv VITE_BANANA_API_BASE_URL
```

Expected baseline:

- `VITE_BANANA_API_BASE_URL=http://localhost:8080`
- `BANANA_API_BASE_URL=http://localhost:8080`
- `EXPO_PUBLIC_BANANA_API_BASE_URL=http://localhost:8080`

## 3. Verify rendered behavior

Open `http://localhost:5173` and confirm the UI does not show `API base: <unset>`.

## 4. Drift recovery sequence

```bash
bash scripts/compose-run-profile.sh --profile apps --action down
bash scripts/compose-run-profile.sh --profile runtime --action down
docker compose --profile apps down --remove-orphans
```

Then relaunch using the canonical commands from step 1.

## 5. Stop stacks

```bash
bash scripts/compose-run-profile.sh --profile apps --action down
bash scripts/compose-run-profile.sh --profile runtime --action down
```
