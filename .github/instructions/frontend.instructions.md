---
name: Frontend Standards
description: Guidance for Banana React and Electron code under src/typescript.
applyTo: "src/typescript/**/*.{ts,tsx,js,json,css}"
---

# Frontend Standards

- Use `react-ui-agent` for `src/typescript/react/src` UI, state, and styling work.
- Use `electron-agent` for `src/typescript/electron` runtime, preload, and desktop bridge work.
- Use `react-agent` only when React and Electron or multiple frontend helpers need coordination.
- Use Bun-first workflows in `src/typescript/react`; do not replace them with npm unless the existing app already requires npm, such as Electron.
- Keep API access centralized and typed, and preserve `VITE_BANANA_API_BASE_URL` as the React runtime contract.
- Maintain compatibility with the compose-based local runtime described in `README.md` and `docker-compose.yml`.
- Prefer minimal, typed UI changes and avoid introducing frontend dependencies without clear need.
- If a frontend change affects backend payload shape, document the contract assumption and coordinate with API tests or prompts.
