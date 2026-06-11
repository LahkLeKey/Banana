# Panel Component Architecture

All resizable/dockable UI components should build on the shared `PanelBase` architecture from `@banana/ui`.

## Component Hierarchy

```
┌─ PanelBase (foundation)
│  ├─ EnhancedPanel (with sections + status + header tools)
│  ├─ TabbedPanelGroup (multi-tab interface)
│  └─ Custom Panel (wrap PanelBase directly)
│
├─ ResizablePanel (adds resize handles + collapse)
├─ PanelOverlay (modal expanded view)
├─ PanelGroup (multiple resizable panels)
│
└─ ResizableDockGrid (orchestrates all corner docks)
   └─ Layout system (top-left, top-right, bottom-left, bottom-right)
```

## Usage Patterns

### Pattern 1: Simple Panel with Status

Use `PanelBase` for straightforward content:

```typescript
import { PanelBase } from '@banana/ui';

function MyPanel() {
  return (
    <PanelBase
      title="PANEL TITLE"
      padding="8px"
      gap="8px"
    >
      {/* Your content here */}
    </PanelBase>
  );
}
```

### Pattern 2: Complex Panel with Sections

Use `EnhancedPanel` when you have multiple sections, status indicators, or header controls:

```typescript
import { EnhancedPanel } from '@banana/ui';

function ComplexPanel() {
  const sections = [
    {
      title: 'Section One',
      content: <div>Content A</div>,
    },
    {
      title: 'Section Two',
      content: <div>Content B</div>,
    },
  ];

  return (
    <EnhancedPanel
      title="COMPLEX PANEL"
      status="success"
      statusMessage="All systems operational"
      sections={sections}
      headerTools={<button>Action</button>}
    />
  );
}
```

### Pattern 3: Tabbed Interface

Use `TabbedPanelGroup` for panels with multiple views:

```typescript
import { TabbedPanelGroup } from '@banana/ui';

function TabbedPanel() {
  const entries = [
    {
      id: 'overview',
      title: 'OVERVIEW',
      content: <div>Overview content</div>,
    },
    {
      id: 'details',
      title: 'DETAILS',
      content: <div>Details content</div>,
    },
  ];

  return (
    <TabbedPanelGroup
      entries={entries}
      defaultActiveId="overview"
    />
  );
}
```

### Pattern 4: State Management

Use `usePanelState` hook for common panel behaviors:

```typescript
import { usePanelState } from '@banana/ui';

function ManagedPanel() {
  const { isLoading, error, setLoading, setError, toggleCollapse } = usePanelState();

  // Use in your component
  return (
    <button onClick={toggleCollapse}>
      Collapse/Expand
    </button>
  );
}
```

## Integration in DataSciencePlaygroundPage

All notebook panels are orchestrated via `ResizableDockGrid`:

```typescript
const dockEntries = [
  {
    id: 'explorer',
    corner: 'top-left',
    visible: showExplorer,
    content: <SafeNotebookExplorerPanel />,
    title: 'FILE EXPLORER',
    defaultWidth: 360,
    defaultHeight: 320,
  },
  // ... more entries
];

return (
  <main>
    {/* Main gameplay surface */}
    <SafeNotebookGameplaySurface {...props} />
    
    {/* Resizable panels in corners */}
    <ResizableDockGrid entries={dockEntries} />
  </main>
);
```

## Styling

All panels inherit from `@banana/ui` theme:

- **Background**: `rgba(7, 19, 34, 0.85)` — dark blue-grey
- **Border**: `1px solid rgba(20, 184, 166, 0.2)` — teal accent
- **Text**: `rgba(226, 232, 240, 0.85)` — light slate
- **Header**: `rgba(226, 232, 240, 0.8)` — uppercase, bold

### Variants

- `default` — standard spacing and sizing
- `compact` — reduced padding for space-constrained panels
- `wide` — expanded padding for comfortable reading

## Migration Guide

### Updating Existing Panels

1. **Identify panel type** — Does it need status? Multiple sections? Tabs?
2. **Choose base component**:
   - Simple → `PanelBase`
   - Status + sections → `EnhancedPanel`
   - Multiple views → `TabbedPanelGroup`
3. **Replace old styling** — Remove custom border/background/padding
4. **Integrate state** — Use `usePanelState` for loading/error/collapse
5. **Test resizing** — Ensure panel works with ResizableDockGrid

### Example: NotebookHealthPanel

**Before:**
```typescript
function NotebookHealthPanel() {
  return (
    <div style={{ display: 'flex', flexDirection: 'column' }}>
      <RouteSubActionBar meta={statusMeta} />
      <div style={{ padding: '10px 12px' }}>{content}</div>
    </div>
  );
}
```

**After:**
```typescript
function NotebookHealthPanel() {
  return (
    <EnhancedPanel
      title="ROUTE REGISTRY"
      status={status}
      statusMessage={statusMessage}
      sections={sections}
    />
  );
}
```

## Best Practices

✅ **Do:**
- Build on `PanelBase` or `EnhancedPanel`
- Use `usePanelState` for common behaviors
- Keep sections focused and logically grouped
- Use status indicators for async states
- Respect the theming system

❌ **Don't:**
- Create custom styled divs — wrap PanelBase
- Mix different panel styling approaches
- Hardcode colors instead of using theme values
- Ignore ResizableDockGrid orchestration
- Make panels too wide (max ~50vw recommended)

## Future Enhancements

- [ ] Panel persistence (localStorage)
- [ ] Drag-to-reorder panels
- [ ] Custom panel templates
- [ ] Analytics integration (panel open/close events)
- [ ] Keyboard shortcuts for panel focus
