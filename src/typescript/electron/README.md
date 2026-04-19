# Banana Electron App

Banana Desktop runtime with two surfaces:

- CLI smoke flow (`npm run smoke`) used by compose and CI.
- Desktop renderer flow powered by Bun + React + shadcn + Tailwind in `renderer/`.

## Renderer Setup (Bun)

```bash
cd src/typescript/shared/ui
bun install

cd src/typescript/electron/renderer
bun install
bun run dev
```

Then launch Electron against the dev server:

```bash
cd src/typescript/electron
BANANA_ELECTRON_RENDERER_URL=http://127.0.0.1:4173 npm run start
```

## Build Desktop Renderer

```bash
cd src/typescript/shared/ui
bun install

cd src/typescript/electron
bun run renderer:build
npm run start
```

`main.js` loads `renderer/dist/index.html` by default and shows a guidance page if the renderer has not been built.

## Shared UI Suite

- Shared renderer/ui primitives are provided by `@banana/ui` from `src/typescript/shared/ui`.
- Renderer Tailwind config consumes `@banana/ui/tailwind/preset` and scans `./node_modules/@banana/ui/src/**/*.{ts,tsx}`.
- Renderer global CSS imports `@banana/ui/styles/tokens.css`.

## Existing Smoke Flow

The existing smoke flow remains unchanged and still runs through Node scripts:

```bash
cd src/typescript/electron
npm install
npm run smoke
```

## Dockerized Desktop Window (WSL2)

Run from WSL2 to launch Electron desktop through Docker Compose with WSLg display forwarding:

```bash
cd /mnt/c/Github/Banana
bash scripts/compose-electron-desktop-wsl2.sh
```

Or from VS Code use the main launch profile `Banana Channels (Container Driven)`.

Direct container-to-WSLg rendering requires Docker Desktop WSL integration enabled for the target distro.

### Reproducible Windows + Docker Desktop + Ubuntu Workflow

Use this path for the standard Microsoft Store Ubuntu + Docker Desktop workflow used across teams.

1. Install Ubuntu (or Ubuntu 24.04) from Microsoft Store.
2. In PowerShell, set Ubuntu as the default distro:

```powershell
wsl --set-default Ubuntu
```

3. In Docker Desktop:
	- Enable `Use the WSL 2 based engine`.
	- Enable `Settings > Resources > WSL Integration` for your Ubuntu distro.
4. Restart WSL:

```powershell
wsl --shutdown
```

5. Validate inside Ubuntu:

```bash
command -v docker
docker version
ls -l /var/run/docker.sock
```

6. Launch Banana desktop from Ubuntu:

```bash
cd /mnt/c/Github/Banana
bash scripts/compose-electron-desktop-wsl2.sh
```

From a Windows shell, the launcher now prefers Ubuntu distros automatically (`Ubuntu-24.04`, then `Ubuntu`).
Set `BANANA_WSL_DISTRO` to override if your team uses a different distro name.
