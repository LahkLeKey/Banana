# PanelStage

`PanelStage` is the explicit host/container primitive for `banana-panels`.

## Why It Exists

It makes container-scoped panel composition intentional. Consumers should not have to infer that a parent `div` must be positioned and clipped a certain way.

## Import

```tsx
import { PanelStage } from 'banana-panels';
```

## Example

```tsx
<PanelStage label="Workspace stage">
  {/* Panels and dock grids go here */}
</PanelStage>
```

## Key Props

- `label`: accessible stage label
- `padded`: stage padding toggle
- `clipped`: whether children are clipped to the stage
- `style`: explicit bounds/inset overrides
- `minHeight`: stage minimum height
