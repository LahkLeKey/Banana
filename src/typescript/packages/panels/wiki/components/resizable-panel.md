# ResizablePanel

`ResizablePanel` is the primary movable/resizable window primitive.

## Import

```tsx
import { ResizablePanel } from '@banana/panels/resizable-panel';
```

## Typical Usage

```tsx
<ResizablePanel
  id="queue"
  title="Queue"
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
  Panel content
</ResizablePanel>
```

## Notes

- Use `hostMode="container"` inside `PanelStage`.
- Provide stable `id` values for snapping and grouping.
- Use `interactionScope` implicitly through higher-level hosts like dock grids, or explicitly when composing advanced workspaces.
