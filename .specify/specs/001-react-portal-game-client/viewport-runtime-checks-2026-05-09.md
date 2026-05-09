# Viewport Runtime Checks - 2026-05-09

## Scope

Validate landing and primary workspace routes at desktop and mobile viewport sizes for Banana Engineer continuity.

## Runtime Surface

- URL: `http://127.0.0.1:4173`
- Server: `bun run dev --host 127.0.0.1 --port 4173`
- Env: `VITE_BANANA_API_BASE_URL=https://api.banana.engineer`

## Checks Performed

1. Landing route (`/`) desktop viewport (1366x900)
   - Banner copy present: `Banana Engineer`
   - Lifecycle state visible: `Engine ready. Enter the workspace.`
   - No horizontal overflow.
2. Landing route (`/`) mobile viewport (390x844)
   - Banner copy present: `Banana Engineer`
   - Lifecycle state visible.
   - No horizontal overflow.
3. Workspace route (`/workspace`) desktop viewport (1366x900)
   - Shell heading present: `Mission Workspace`
   - Navigation and primary cards render.
   - No horizontal overflow.
4. Workspace route (`/workspace`) mobile viewport (390x844)
   - Shell heading present.
   - Navigation and primary cards render.
   - No horizontal overflow.

## Screenshot Captures

- Landing desktop viewport captured in integrated browser.
- Landing mobile viewport captured in integrated browser.
- Workspace desktop viewport captured in integrated browser.
- Workspace mobile viewport captured in integrated browser.

## Outcome

- T006 and T011 acceptance evidence is satisfied for desktop/mobile runtime behavior and screenshot continuity.
