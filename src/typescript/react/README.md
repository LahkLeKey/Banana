# Banana React App

Bun + React + Tailwind starter application for Banana API integration.

## Quick Start

1. Install shared UI dependencies:
   - `cd ../shared/ui && bun install`
2. Install app dependencies:
   - `bun install`
3. Configure environment:
   - copy `.env.example` to `.env` and adjust `VITE_BANANA_API_BASE_URL` if needed
4. Start dev server:
   - `bun run dev`

## Shared UI Suite

- Shared primitives and tokens are provided by `@banana/ui` from `src/typescript/shared/ui`.
- Use `@banana/ui` for `Button`, `Input`, `Card`, `Badge`, and `Label`.
- Use `@banana/ui/tailwind/preset` in Tailwind config and include `./node_modules/@banana/ui/src/**/*.{ts,tsx}` in `content`.
- Import `@banana/ui/styles/tokens.css` in the app stylesheet before Tailwind layers.

## API Endpoints Used

- `GET /health`
- `GET /banana?purchases=<int>&multiplier=<int>`
- `POST /batches/create`
- `GET /batches/{id}/status`
- `POST /ripeness/predict`

## Scripts

- `bun run dev`
- `bun run build`
- `bun run preview`
- `bun run check`
