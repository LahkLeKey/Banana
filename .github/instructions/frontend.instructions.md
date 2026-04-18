---
name: Frontend Standards
description: Guidance for Banana React and Electron code under src/typescript.
applyTo: "src/typescript/**/*.{ts,tsx,js,json,css}"
---

# Frontend Standards

- Use Bun-first workflows in `src/typescript/react`; do not replace them with npm unless the existing app already requires npm, such as Electron.
- Keep API access centralized and typed, and preserve `VITE_BANANA_API_BASE_URL` as the React runtime contract.
- Maintain compatibility with the compose-based local runtime described in `README.md` and `docker-compose.yml`.
- Prefer minimal, typed UI changes and avoid introducing frontend dependencies without clear need.
- If a frontend change affects backend payload shape, document the contract assumption and coordinate with API tests or prompts.
