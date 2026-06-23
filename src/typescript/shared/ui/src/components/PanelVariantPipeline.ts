export type PanelBehaviorLayer =
    |'base'|'movable'|'resizable'|'dockable'|'modal';

export type PanelBehaviorOptions = {
    readonly movable?: boolean;
    readonly resizable?: boolean;
    readonly dockable?: boolean;
    readonly modal?: boolean;
};

export type PanelBehaviorPipeline = {
  readonly layers: ReadonlyArray<PanelBehaviorLayer>; readonly tokens:
                                                                   ReadonlyArray<
                                                                       string>;
};

const LAYER_TO_TOKEN: Record<PanelBehaviorLayer, string> = {
  base: 'base',
  movable: 'move',
  resizable: 'resize',
  dockable: 'dock',
  modal: 'modal',
};

/**
 * buildPanelBehaviorPipeline creates an ordered behavior chain for panel
 * shells. This keeps capability layering explicit and consistent across panel
 * variants.
 */
export function buildPanelBehaviorPipeline({
  movable = false,
  resizable = false,
  dockable = false,
  modal = false,
}: PanelBehaviorOptions): PanelBehaviorPipeline {
  const layers: PanelBehaviorLayer[] = ['base'];

  if (movable) {
    layers.push('movable');
  }
  if (resizable) {
    layers.push('resizable');
  }
  if (dockable) {
    layers.push('dockable');
  }
  if (modal) {
    layers.push('modal');
  }

  return {
    layers,
    tokens: layers.map((layer) => LAYER_TO_TOKEN[layer]),
  };
}
