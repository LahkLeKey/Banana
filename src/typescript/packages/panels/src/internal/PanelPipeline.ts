import type {CSSProperties} from 'react';

export type PanelStageStyles<TStage extends string> =
    Partial<Record<TStage, CSSProperties>>;

/**
 * composePanelStages merges a default style map with stage overrides so panel
 * implementations can share a consistent extension pipeline.
 */
export function composePanelStages<TStage extends string>(
    defaults: Record<TStage, CSSProperties>,
    overrides?: PanelStageStyles<TStage>,
    ): Record<TStage, CSSProperties> {
  const merged = {} as Record<TStage, CSSProperties>;
  const stages = Object.keys(defaults) as TStage[];

  for (const stage of stages) {
    merged[stage] = {
      ...defaults[stage],
      ...(overrides?.[stage] ?? {}),
    };
  }

  return merged;
}
