# Theme Pipeline

The theme pipeline controls runtime CSS-variable styling for panel surfaces.

## Import

```tsx
import { applyPanelTheme, resetPanelTheme } from '@banana/panels/theme-pipeline';
```

## Example

```tsx
applyPanelTheme('aurora-grid-dark');
// ...render or update panel workspace...
resetPanelTheme();
```

## What It Controls

- panel surface/background colors
- border/chrome colors
- desktop gradient
- header/taskbar backgrounds
- font family
- header/subtle text colors
