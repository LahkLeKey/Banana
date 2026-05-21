import type { GameSessionPlayer, GameSessionServerMetrics } from "../lib/api";

type PlayerListOverlayProps = {
  readonly sessionId: string;
  readonly connectionState: "connecting" | "connected" | "disconnected";
  readonly serverTick: number;
  readonly serverMetrics: GameSessionServerMetrics | null;
  readonly nativeActivePlayerCount: number;
  readonly players: GameSessionPlayer[];
};

export function PlayerListOverlay({
  sessionId,
  connectionState,
  serverTick,
  serverMetrics,
  nativeActivePlayerCount,
  players,
}: PlayerListOverlayProps) {
  return (
    <aside
      style={{
        position: "absolute",
        top: 16,
        right: 16,
        minWidth: 260,
        padding: 12,
        borderRadius: 12,
        background: "rgba(15, 23, 42, 0.82)",
        color: "#e2e8f0",
        border: "1px solid rgba(148, 163, 184, 0.35)",
        backdropFilter: "blur(8px)",
      }}
    >
      <div style={{ display: "flex", justifyContent: "space-between", gap: 12, marginBottom: 8 }}>
        <strong>Overworld</strong>
        <span>{connectionState}</span>
      </div>
      <div style={{ fontSize: 12, opacity: 0.85, marginBottom: 8 }}>Room: {sessionId}</div>
      <div style={{ fontSize: 12, opacity: 0.85, marginBottom: 4 }}>Server tick: {serverTick}</div>
      <div style={{ fontSize: 12, opacity: 0.85, marginBottom: 12 }}>
        TPS {serverMetrics ? serverMetrics.currentTps.toFixed(1) : "--"} / {serverMetrics ? serverMetrics.targetTps.toFixed(0) : "30"} · lag {serverMetrics ? serverMetrics.lagMs.toFixed(1) : "--"}ms
      </div>
      <div style={{ fontSize: 12, opacity: 0.85, marginBottom: 12 }}>
        Native active player entities: {nativeActivePlayerCount}
      </div>
      <ul style={{ listStyle: "none", padding: 0, margin: 0, display: "grid", gap: 8 }}>
        {players.map((player) => (
          <li
            key={player.playerId}
            style={{
              display: "flex",
              justifyContent: "space-between",
              gap: 12,
              alignItems: "center",
            }}
          >
            <span>{player.playerName}</span>
            <span style={{ fontSize: 12, opacity: 0.8 }}>
              {player.connected ? "online" : "offline"} · id {player.playerId.slice(-6)}
            </span>
          </li>
        ))}
        {players.length === 0 ? <li style={{ fontSize: 12, opacity: 0.8 }}>Waiting for players…</li> : null}
      </ul>
    </aside>
  );
}
