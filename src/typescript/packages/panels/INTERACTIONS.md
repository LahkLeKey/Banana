# Panels Interactions

This package includes interaction-oriented Storybook surfaces that focus on behavior, not theming.

## Why Interactions

These stories show how `PanelStage`, `ResizablePanel`, `ResizableDockGrid`, and `PanelOverlay` behave under real user actions such as dragging, docking, collapsing, and opening dialogs.

## Storybook Branches

All interaction stories now live under `Panels / Interactions` with focused categories:

- `Panels / Interactions / Drag and snap`
- `Panels / Interactions / Docking labels`
- `Panels / Interactions / Dialog and overlay`
- `Panels / Interactions / Closable panels`
- `Panels / Interactions / Panel relationships`

## Interaction Index

Use these categories to inspect behavior intent quickly:

- `Drag and snap`: free-panel movement, snap commit events, and scope-aware drag behavior
- `Docking labels`: anchor badges, unlink controls, group resize lock, and dock close/restore flows
- `Dialog and overlay`: modal launch, backdrop close behavior, and escape key handling
- `Closable panels`: collapse, launcher restore, and mixed open/collapsed workspace states
- `Panel relationships`: same-scope versus isolated-scope panel interactions

## Integration Guidance

- Use `PanelStage` plus `hostMode="container"` when interactions should stay inside an application surface.
- Use `interactionScope` to prevent unrelated panel groups from snapping together.
- Use `ResizableDockGrid` when you want anchor labels, unlink actions, and grouped resize behavior.
- Use `PanelOverlay` for dialog-like flows that share the panel system chrome.