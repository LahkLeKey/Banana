import type { GameSessionPlayer, GameSessionServerMetrics } from "../lib/api";

export interface PlayerListOverlayProps {
  readonly sessionId: string;
  readonly connectionState: "connecting" | "connected" | "disconnected";
  readonly serverTick: number;
  readonly serverMetrics: GameSessionServerMetrics | null;
  readonly nativeActivePlayerCount: number;
  readonly players: GameSessionPlayer[];
}

export function PlayerListOverlay(_props: PlayerListOverlayProps) {
  return null;
}
