# Banana React App

Bun + React + Tailwind starter application for Banana API integration.

## Quick Start

1. Install dependencies:
   - `bun install`
2. Configure environment:
   - copy `.env.example` to `.env` and adjust `VITE_BANANA_API_BASE_URL` if needed
3. Start dev server:
   - `bun run dev`

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
