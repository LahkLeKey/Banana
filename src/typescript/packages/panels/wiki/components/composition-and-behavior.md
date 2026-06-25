# Composition and Behavior APIs

These lower-level exports exist for teams that need to customize panel behavior beyond the main primitives.

## Relevant Exports

- `@banana/panels/panel-pipeline`
- `@banana/panels/panel-variant-pipeline`
- `@banana/panels/styles`
- `@banana/panels/interactions`
- `@banana/panels/view-model`
- `@banana/panels/resize`
- `@banana/panels/drag-snap`
- `@banana/panels/raf-buffered-dispatch`

## Recommendation

Start with `PanelStage`, `ResizablePanel`, `ResizableDockGrid`, `PanelBase`, and `PanelOverlay` first. Only drop to these lower layers when you are building a custom composition system or integrating with an existing renderer/runtime.
