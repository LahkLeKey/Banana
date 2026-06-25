# ResizableDockGrid

`ResizableDockGrid` provides a corner-seeded dock layout that still uses the same resizable panel primitive underneath.

## Import

```tsx
import { ResizableDockGrid } from 'banana-panels/resizable-dock-grid';
```

## Example

```tsx
<ResizableDockGrid
  hostMode="container"
  entries={[
    {
      id: 'filters',
      title: 'Filters',
      corner: 'top-left',
      visible: true,
      defaultWidth: 320,
      defaultHeight: 180,
      content: <div>Filter surface</div>,
    },
  ]}
/>
```

## Notes

- Dock entries are best used for context and support surfaces.
- `hostMode="container"` keeps dock behavior scoped to the stage instead of the viewport.
- Dock panels now use dock-local interaction scoping to avoid snapping interference with unrelated free panels.
