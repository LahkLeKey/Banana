import { ActorTooltipOverlay } from "../ActorTooltipOverlay";
import { PlayerListOverlay } from "../PlayerListOverlay";
import type {
    GameSessionBootstrap,
    GameSessionPlayer,
    GameSessionServerMetrics,
    GameSessionSnapshotEntity,
} from "../../lib/api";

export interface OverworldHudProps {
    readonly snapshotEntities: Record<string, GameSessionSnapshotEntity>;
    readonly localPlayerId: string | null;
    readonly sessionBootstrap: GameSessionBootstrap | null;
    readonly connectionState: "connecting" | "connected" | "disconnected";
    readonly serverTick: number;
    readonly serverMetrics: GameSessionServerMetrics | null;
    readonly nativeActivePlayerCount: number;
    readonly sessionPlayers: GameSessionPlayer[];
}

export function OverworldHud({
    snapshotEntities,
    localPlayerId,
    sessionBootstrap,
    connectionState,
    serverTick,
    serverMetrics,
    nativeActivePlayerCount,
    sessionPlayers,
}: OverworldHudProps) {
    return (
        <>
            <ActorTooltipOverlay entities={snapshotEntities} localPlayerId={localPlayerId} />

            {sessionBootstrap ? (
                <PlayerListOverlay
                    sessionId={sessionBootstrap.sessionId}
                    connectionState={connectionState}
                    serverTick={serverTick}
                    serverMetrics={serverMetrics}
                    nativeActivePlayerCount={nativeActivePlayerCount}
                    players={sessionPlayers}
                />
            ) : null}
        </>
    );
}
