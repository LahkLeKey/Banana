import {create, type StateCreator} from 'zustand';

export type DockCorner = 'top-left'|'top-right'|'bottom-left'|'bottom-right';
export type DockAnchorSide = 'left'|'right'|'top'|'bottom';

export type DockPanelRect = {
  x: number; y: number; width: number; height: number;
};

export type DockAnchorLink = {
  id: string; side: DockAnchorSide;
};

export type DockPanelState = DockPanelRect&{
  corner: DockCorner;
  zIndex: number;
  resetToken: number;
};

export type DockViewport = {
  width: number; height: number;
};

export type DockEntrySeed = {
  id: string; corner: DockCorner;
  defaultWidth?: number;
  defaultHeight?: number;
  resetToken?: number;
};

export type DockLayoutState = {
  panels: Record<string, DockPanelState>; focusOrder: string[];
  anchorLinks: Record<string, DockAnchorLink[]>;
  resizeLockedByPanel: Record<string, boolean>;
  viewport: DockViewport;
};

type DockLayoutsStore = {
  layouts: Record<string, DockLayoutState>;
  initializeLayout:
      (layoutId: string, entries: DockEntrySeed[], viewport: DockViewport) =>
          void;
  focusPanel: (layoutId: string, panelId: string) => void;
  movePanelGroupBy: (
      layoutId: string, panelId: string, delta: {x: number; y: number}) => void;
  resizePanel: (layoutId: string, panelId: string, rect: DockPanelRect) => void;
  addAnchorLink:
      (layoutId: string, sourceId: string, targetId: string,
       sourceSide: DockAnchorSide) => void;
  unlinkPanel: (layoutId: string, panelId: string) => void;
  toggleResizeLock: (layoutId: string, panelId: string) => void;
};

const viewportPadding = 8;
const topInset = 52;
const sideInset = 16;
const bottomInset = 96;
const panelGap = 12;

const clamp = (value: number, min: number, max: number) =>
    Math.max(min, Math.min(max, value));

const oppositeSide = (side: DockAnchorSide): DockAnchorSide => {
  if (side === 'left') return 'right';
  if (side === 'right') return 'left';
  if (side === 'top') return 'bottom';
  return 'top';
};

const getDefaultRect =
    (entry: DockEntrySeed, indexInCorner: number, viewport: DockViewport):
        DockPanelRect => {
          const width = entry.defaultWidth ?? 360;
          const height = entry.defaultHeight ?? 280;
          const stackedOffset = indexInCorner * (height + panelGap);

          if (entry.corner === 'top-left') {
            return {x: sideInset, y: topInset + stackedOffset, width, height};
          }
          if (entry.corner === 'top-right') {
            return {
              x: viewport.width - sideInset - width,
              y: topInset + stackedOffset,
              width,
              height
            };
          }
          if (entry.corner === 'bottom-left') {
            return {
              x: sideInset,
              y: viewport.height - bottomInset - height - stackedOffset,
              width,
              height
            };
          }
          return {
            x: viewport.width - sideInset - width,
            y: viewport.height - bottomInset - height - stackedOffset,
            width,
            height
          };
        };

const clampRectToViewport =
    (rect: DockPanelRect, viewport: DockViewport): DockPanelRect => {
      const width = clamp(
          rect.width, 280,
          Math.max(280, viewport.width - (viewportPadding * 2)));
      const height = clamp(
          rect.height, 200,
          Math.max(200, viewport.height - (viewportPadding * 2)));
      const x = clamp(
          rect.x, viewportPadding,
          Math.max(viewportPadding, viewport.width - viewportPadding - width));
      const y = clamp(
          rect.y, viewportPadding,
          Math.max(
              viewportPadding, viewport.height - viewportPadding - height));
      return {x, y, width, height};
    };

const getGroupMembers =
    (layout: DockLayoutState, panelId: string): string[] => {
      const visited = new Set<string>();
      const queue = [panelId];

      while (queue.length > 0) {
        const current = queue.shift();
        if (!current || visited.has(current)) {
          continue;
        }
        visited.add(current);
        for (const link of layout.anchorLinks[current] ?? []) {
          if (!visited.has(link.id)) {
            queue.push(link.id);
          }
        }
      }

      return [...visited];
    };

const createDockLayoutsStore: StateCreator<DockLayoutsStore> = (set) => ({
  layouts: {},

  initializeLayout:
      (layoutId: string, entries: DockEntrySeed[], viewport: DockViewport) =>
          set((state: DockLayoutsStore) => {
            const current = state.layouts[layoutId];
            const nextPanels: Record<string, DockPanelState> = {};
            const nextFocusOrder =
                current?.focusOrder.filter(
                    (id) => entries.some((entry) => entry.id === id)) ??
                [];

            const byCorner: Record<DockCorner, DockEntrySeed[]> = {
              'top-left': [],
              'top-right': [],
              'bottom-left': [],
              'bottom-right': [],
            };

            for (const entry of entries) {
              byCorner[entry.corner].push(entry);
            }

            for (const entry of entries) {
              const cornerIndex = byCorner[entry.corner].findIndex(
                  (candidate) => candidate.id === entry.id);
              const baseRect = getDefaultRect(entry, cornerIndex, viewport);
              const previous = current?.panels[entry.id];
              const resetToken = entry.resetToken ?? 0;
              const rect = !previous || previous.resetToken !== resetToken ?
                  baseRect :
                  clampRectToViewport(previous, viewport);

              nextPanels[entry.id] = {
                ...rect,
                corner: entry.corner,
                zIndex: previous?.zIndex ?? 10 + nextFocusOrder.length,
                resetToken,
              };

              if (!nextFocusOrder.includes(entry.id)) {
                nextFocusOrder.push(entry.id);
              }
            }

            nextFocusOrder.forEach((id, index) => {
              if (nextPanels[id]) {
                nextPanels[id].zIndex = 10 + index;
              }
            });

            const nextAnchorLinks: Record<string, DockAnchorLink[]> = {};
            const currentAnchorLinks: Record<string, DockAnchorLink[]> =
                current?.anchorLinks ?? {};
            for (const [id, links] of Object.entries(
                     currentAnchorLinks) as [string, DockAnchorLink[]][]) {
              if (!nextPanels[id]) {
                continue;
              }
              const validLinks = links.filter(
                  (link: DockAnchorLink) => Boolean(nextPanels[link.id]));
              if (validLinks.length > 0) {
                nextAnchorLinks[id] = validLinks;
              }
            }

            const nextResizeLocks: Record<string, boolean> = {};
            const currentResizeLocks: Record<string, boolean> =
                current?.resizeLockedByPanel ?? {};
            for (const [id, locked] of Object.entries(
                     currentResizeLocks) as [string, boolean][]) {
              if (nextPanels[id]) {
                nextResizeLocks[id] = locked;
              }
            }

            return {
              layouts: {
                ...state.layouts,
                [layoutId]: {
                  panels: nextPanels,
                  focusOrder: nextFocusOrder,
                  anchorLinks: nextAnchorLinks,
                  resizeLockedByPanel: nextResizeLocks,
                  viewport,
                },
              },
            };
          }),

  focusPanel: (
      layoutId: string, panelId: string) => set((state: DockLayoutsStore) => {
    const layout = state.layouts[layoutId];
    if (!layout?.panels[panelId]) {
      return state;
    }

    const focusOrder =
        [...layout.focusOrder.filter((id: string) => id !== panelId), panelId];
    const panels = {...layout.panels};
    focusOrder.forEach((id: string, index: number) => {
      if (panels[id]) {
        panels[id] = {...panels[id], zIndex: 10 + index};
      }
    });

    return {
      layouts: {
        ...state.layouts,
        [layoutId]: {...layout, focusOrder, panels},
      },
    };
  }),

  movePanelGroupBy:
      (layoutId: string, panelId: string, delta: {x: number; y: number}) =>
          set((state: DockLayoutsStore) => {
            const layout = state.layouts[layoutId];
            if (!layout?.panels[panelId]) {
              return state;
            }

            const members = getGroupMembers(layout, panelId);
            const bounds = members.reduce((acc, memberId) => {
              const panel = layout.panels[memberId];
              if (!panel) {
                return acc;
              }
              return {
                left: Math.min(acc.left, panel.x),
                top: Math.min(acc.top, panel.y),
                right: Math.max(acc.right, panel.x + panel.width),
                bottom: Math.max(acc.bottom, panel.y + panel.height),
              };
            }, {
              left: Number.POSITIVE_INFINITY,
              top: Number.POSITIVE_INFINITY,
              right: Number.NEGATIVE_INFINITY,
              bottom: Number.NEGATIVE_INFINITY,
            });

            const dx = clamp(
                delta.x, viewportPadding - bounds.left,
                layout.viewport.width - viewportPadding - bounds.right);
            const dy = clamp(
                delta.y, viewportPadding - bounds.top,
                layout.viewport.height - viewportPadding - bounds.bottom);

            if (dx === 0 && dy === 0) {
              return state;
            }

            const panels = {...layout.panels};
            for (const memberId of members) {
              const panel = panels[memberId];
              if (!panel) {
                continue;
              }
              panels[memberId] = {
                ...panel,
                x: panel.x + dx,
                y: panel.y + dy,
              };
            }

            return {
              layouts: {
                ...state.layouts,
                [layoutId]: {...layout, panels},
              },
            };
          }),

  resizePanel: (layoutId: string, panelId: string, rect: DockPanelRect) =>
      set((state: DockLayoutsStore) => {
        const layout = state.layouts[layoutId];
        const panel = layout?.panels[panelId];
        if (!layout || !panel) {
          return state;
        }

        const nextRect = clampRectToViewport(rect, layout.viewport);
        const dw = nextRect.width - panel.width;
        const dh = nextRect.height - panel.height;

        const panels = {
          ...layout.panels,
          [panelId]: {
            ...panel,
            ...nextRect,
          },
        };

        if (layout.resizeLockedByPanel[panelId]) {
          const members = getGroupMembers(layout, panelId)
                              .filter((id: string) => id !== panelId);
          for (const memberId of members) {
            const member = panels[memberId];
            if (!member) {
              continue;
            }
            panels[memberId] = {
              ...member,
              ...clampRectToViewport(
                  {
                    x: member.x,
                    y: member.y,
                    width: member.width + dw,
                    height: member.height + dh,
                  },
                  layout.viewport),
            };
          }
        }

        return {
          layouts: {
            ...state.layouts,
            [layoutId]: {...layout, panels},
          },
        };
      }),

  addAnchorLink:
      (layoutId: string, sourceId: string, targetId: string,
       sourceSide: DockAnchorSide) => set((state: DockLayoutsStore) => {
        const layout = state.layouts[layoutId];
        if (!layout?.panels[sourceId] || !layout.panels[targetId]) {
          return state;
        }

        const anchorLinks = {...layout.anchorLinks};
        const put = (fromId: string, toId: string, side: DockAnchorSide) => {
          const next = [
            ...(anchorLinks[fromId] ?? [])
                .filter((link: DockAnchorLink) => link.id !== toId),
            {id: toId, side}
          ];
          anchorLinks[fromId] = next;
        };

        put(sourceId, targetId, sourceSide);
        put(targetId, sourceId, oppositeSide(sourceSide));

        return {
          layouts: {
            ...state.layouts,
            [layoutId]: {...layout, anchorLinks},
          },
        };
      }),

  unlinkPanel: (layoutId: string, panelId: string) =>
      set((state: DockLayoutsStore) => {
        const layout = state.layouts[layoutId];
        if (!layout?.panels[panelId]) {
          return state;
        }

        const anchorLinks: Record<string, DockAnchorLink[]> = {};
        for (const [id, links] of Object.entries(
                 layout.anchorLinks) as [string, DockAnchorLink[]][]) {
          if (id === panelId) {
            continue;
          }
          const filtered =
              links.filter((link: DockAnchorLink) => link.id !== panelId);
          if (filtered.length > 0) {
            anchorLinks[id] = filtered;
          }
        }

        const resizeLockedByPanel = {...layout.resizeLockedByPanel};
        delete resizeLockedByPanel[panelId];

        return {
          layouts: {
            ...state.layouts,
            [layoutId]: {...layout, anchorLinks, resizeLockedByPanel},
          },
        };
      }),

  toggleResizeLock: (layoutId: string, panelId: string) =>
      set((state: DockLayoutsStore) => {
        const layout = state.layouts[layoutId];
        if (!layout?.panels[panelId]) {
          return state;
        }

        const group = getGroupMembers(layout, panelId);
        const locked = layout.resizeLockedByPanel[panelId] ?? false;
        const resizeLockedByPanel = {...layout.resizeLockedByPanel};
        for (const memberId of group) {
          resizeLockedByPanel[memberId] = !locked;
        }

        return {
          layouts: {
            ...state.layouts,
            [layoutId]: {...layout, resizeLockedByPanel},
          },
        };
      }),
});

export const useResizableDockLayoutStore =
    create<DockLayoutsStore>(createDockLayoutsStore);
