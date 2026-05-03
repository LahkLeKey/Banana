# Storybook Local Workflow

Storybook 10.x is installed in `src/typescript/react/` and provides an isolated component browser for the Banana React app.

## Running locally

```bash
cd src/typescript/react
bun run storybook
```

Opens the Storybook UI at **http://localhost:6006**.

## Available stories

| Category | Stories |
|---|---|
| `UI/Button` | Default, Destructive, Outline, Ghost, Disabled, Loading, Small |
| `UI/Input` | Default, WithValue, Disabled, FileInput |
| `UI/Textarea` | Default, WithContent, Disabled |
| `UI/Card` | Default, Minimal |
| `UI/Alert` | Default, Destructive |
| `Panels/EnsemblePanel` | Idle, Loading, VerdictSuccess, VerdictError |
| `Panels/ChatPanel` | Empty, WithMessages, Unavailable |
| `Panels/ErrorBoundary` | FallbackTriggered, CustomFallback, Healthy |

## Building a static export

```bash
cd src/typescript/react
bun run build-storybook
```

Output is written to `src/typescript/react/storybook-static/`. This directory is gitignored; it is only produced locally or by CI.

## Adding a new story

1. Create `<ComponentName>.stories.tsx` alongside the component file.
2. Import `Meta` and `StoryObj` from `@storybook/react-vite`.
3. For play functions, import `fn`, `userEvent`, and `expect` from `storybook/test` (not `@storybook/test`).
4. Run `bun run storybook` to verify the story renders.

## Key configuration files

- `.storybook/main.ts` — framework + stories glob
- `.storybook/preview.ts` — global CSS import + default parameters
