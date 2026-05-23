import type { GameSessionSnapshotEntity } from "../lib/api";

export interface ActorTooltipOverlayProps {
  readonly entities: Record<string, GameSessionSnapshotEntity>;
  readonly localPlayerId: string | null;
}

export function ActorTooltipOverlay(_props: ActorTooltipOverlayProps) {
  return null;
}
