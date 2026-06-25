# Getting Started

`@banana/panels` is a composable panel system for building container-scoped workspaces, review surfaces, and docked operator consoles.

## Install

```bash
npm install @banana/panels react react-dom
```

## First Panel

```tsx
import { PanelStage } from '@banana/panels';
import { ResizablePanel } from '@banana/panels/resizable-panel';

export function Demo() {
  return (
    <div style={{ position: 'relative', minHeight: 480 }}>
      <PanelStage label="Demo stage">
        <ResizablePanel
          id="demo-panel"
          title="Demo Panel"
          hostMode="container"
          x={16}
          y={16}
          width={320}
          height={220}
          onMove={() => {}}
          onResize={() => {}}
          onFocus={() => {}}
          onAnchorCommit={() => {}}
        >
          Hello panels.
        </ResizablePanel>
      </PanelStage>
    </div>
  );
}
```

## First Dock Layout

```tsx
import { PanelStage } from '@banana/panels';
import { ResizableDockGrid } from '@banana/panels/resizable-dock-grid';

export function DockDemo() {
  return (
    <div style={{ position: 'relative', minHeight: 540 }}>
      <PanelStage label="Dock stage">
        <ResizableDockGrid
          hostMode="container"
          entries={[
            {
              id: 'queue',
              title: 'Queue',
              corner: 'top-left',
              visible: true,
              defaultWidth: 320,
              defaultHeight: 180,
              content: <div>Queued work</div>,
            },
            {
              id: 'activity',
              title: 'Activity',
              corner: 'bottom-left',
              visible: true,
              defaultWidth: 420,
              defaultHeight: 180,
              content: <div>Recent events</div>,
            },
          ]}
        />
      </PanelStage>
    </div>
  );
}
```

## Recommended Integration Pattern

- Wrap panel surfaces in a relatively positioned host element.
- Use `PanelStage` to make container scoping explicit.
- Use `hostMode="container"` for both free panels and dock grids.
- Add `PanelBase` or `PanelOverlay` when you want non-resizable or modal panel surfaces in the same design language.
