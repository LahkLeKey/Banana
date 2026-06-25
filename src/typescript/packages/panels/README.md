# banana-panels

Composable React panels for container-scoped workspaces, docked operator consoles, and resizable multi-panel application surfaces.

<img width="2557" height="1270" alt="image" src="https://github.com/user-attachments/assets/90757942-4b17-426c-989b-73942bf01d92" />

## What This Package Is For

Use `banana-panels` when your product needs one or more of these patterns:

- draggable and resizable workspace panels
- docked context surfaces anchored to layout corners
- container-scoped panel systems inside application screens
- modal overlays that match the same panel design language
- runtime theming for panel chrome, gradients, typography, and status accents

## Install

```bash
npm install banana-panels react react-dom
```

## Live Demo

Production Storybook demo for the npm package:

- https://panels.banana.engineer/

Direct interaction demo route:

- https://panels.banana.engineer/?path=/story/panels-interactions-docking-labels--default

## Get Started

The most important integration pattern is:

1. create a relatively positioned host element
2. mount a `PanelStage` inside it
3. render `ResizablePanel` and/or `ResizableDockGrid` with `hostMode="container"`

```tsx
import { PanelStage } from 'banana-panels';
import { ResizablePanel } from 'banana-panels/resizable-panel';

export function Example() {
  return (
    <div style={{ position: 'relative', minHeight: 560 }}>
      <PanelStage label="Example workspace">
        <ResizablePanel
          id="example-panel"
          title="Example Panel"
          hostMode="container"
          x={16}
          y={16}
          width={360}
          height={240}
          onMove={() => {}}
          onResize={() => {}}
          onFocus={() => {}}
          onAnchorCommit={() => {}}
        >
          Container-scoped panel content.
        </ResizablePanel>
      </PanelStage>
    </div>
  );
}
```

## Common Usage Scenarios

### Free Panel Workspace

Use this for builder UIs, control rooms, inspectors, and multi-pane editing surfaces.

Core primitives:

- `PanelStage`
- `ResizablePanel`

### Docked Context Surface

Use this when panels should seed into corners and stay grouped as supporting context around a main task.

Core primitives:

- `PanelStage`
- `ResizableDockGrid`

### Modal or Interruptive Step

Use this when a user needs a temporary overlay that still looks and behaves like the rest of the panel system.

Core primitives:

- `PanelOverlay`
- `PanelBase`

### Themeable Workspace

Use this when consumers need runtime theming for product skins, branded workspaces, or scenario-based visual presets.

Core primitives:

- `theme-pipeline`
- `tailwind/preset`

## Storybook Surfaces

### Themes

Theme stories show runtime styling systems, visual codenames, and container-scoped panel layouts.

### Interactions

Interaction stories now live under `Panels / Interactions` and are organized around behavioral categories such as drag and snap, docking labels, dialog flows, closable panels, and cross-panel relationships.

Interaction guide:

- [INTERACTIONS.md](./INTERACTIONS.md)

## Package Wiki

Use the package wiki for component-by-component guidance.

Wiki index:

- [wiki/README.md](./wiki/README.md)

Recommended reading order:

1. [wiki/getting-started.md](./wiki/getting-started.md)
2. [wiki/components/panel-stage.md](./wiki/components/panel-stage.md)
3. [wiki/components/resizable-panel.md](./wiki/components/resizable-panel.md)
4. [wiki/components/resizable-dock-grid.md](./wiki/components/resizable-dock-grid.md)
5. [wiki/components/theme-pipeline.md](./wiki/components/theme-pipeline.md)
6. [wiki/components/panel-overlay.md](./wiki/components/panel-overlay.md)
7. [wiki/components/panel-base.md](./wiki/components/panel-base.md)
8. [wiki/components/composition-and-behavior.md](./wiki/components/composition-and-behavior.md)

## Public API Overview

Main entrypoints developers typically need:

- `banana-panels`
- `banana-panels/panel-stage`
- `banana-panels/resizable-panel`
- `banana-panels/resizable-dock-grid`
- `banana-panels/panel-base`
- `banana-panels/panel-overlay`
- `banana-panels/theme-pipeline`
- `banana-panels/tailwind/preset`

Lower-level APIs are available for advanced integrations, but most consumers should start with the primitives above.

## Maintainer Notes

Local Storybook:

```bash
cp .env.example .env
bun run storybook
```

Static Storybook build:

```bash
bun run build-storybook
```

Repository-root helper commands:

```bash
bun run storybook:panels:build
bun run storybook:panels:local
PANELS_STORYBOOK_DEPLOY_USER=<ssh-user> bun run storybook:panels:deploy
```

First npm publish workflow:

```bash
cd src/typescript/packages/panels
bun install
bun run build
npm publish --access public
```
