import { ActorTooltipOverlay } from "../ActorTooltipOverlay";
import { PlayerListOverlay } from "../PlayerListOverlay";
import type {
    GameplayBuildStats,
    GameplayComboResult,
    GameSessionBootstrap,
    GameSessionPlayer,
    GameSessionServerMetrics,
    GameSessionSnapshotEntity,
} from "../../lib/api";
import type { NativeUiFrame } from "@banana/ui";

export interface OverworldHudProps {
    readonly snapshotEntities: Record<string, GameSessionSnapshotEntity>;
    readonly localPlayerId: string | null;
    readonly sessionBootstrap: GameSessionBootstrap | null;
    readonly connectionState: "connecting" | "connected" | "disconnected";
    readonly serverTick: number;
    readonly serverMetrics: GameSessionServerMetrics | null;
    readonly nativeActivePlayerCount: number;
    readonly sessionPlayers: GameSessionPlayer[];
    readonly buildStats: GameplayBuildStats | null;
    readonly comboResult: GameplayComboResult | null;
    readonly buildBusy: boolean;
    readonly nativeUiFrame: NativeUiFrame | null;
    readonly onSetClass: (classType: 0 | 1 | 2) => void;
    readonly onApplyPreset: () => void;
    readonly onEquipGear: () => void;
    readonly onEvaluateCombo: () => void;
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
    buildStats,
    comboResult,
    buildBusy,
    nativeUiFrame,
    onSetClass,
    onApplyPreset,
    onEquipGear,
    onEvaluateCombo,
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

            {sessionBootstrap ? (
                <aside
                    style={{
                        position: "absolute",
                        left: 16,
                        bottom: 16,
                        minWidth: 280,
                        padding: 12,
                        borderRadius: 12,
                        background: "rgba(15, 23, 42, 0.82)",
                        color: "#e2e8f0",
                        border: "1px solid rgba(148, 163, 184, 0.35)",
                        backdropFilter: "blur(8px)",
                        zIndex: 19,
                    }}
                >
                    <div style={{ fontWeight: 700, marginBottom: 8 }}>Build Lab</div>
                    <div style={{ display: "flex", gap: 8, marginBottom: 8, flexWrap: "wrap" }}>
                        <button onClick={() => onSetClass(0)} disabled={buildBusy}>Vanguard</button>
                        <button onClick={() => onSetClass(1)} disabled={buildBusy}>Arcanist</button>
                        <button onClick={() => onSetClass(2)} disabled={buildBusy}>Ranger</button>
                    </div>
                    <div style={{ display: "flex", gap: 8, marginBottom: 8, flexWrap: "wrap" }}>
                        <button onClick={onApplyPreset} disabled={buildBusy}>Apply 3/1/2</button>
                        <button onClick={onEquipGear} disabled={buildBusy}>Equip T2 Weapon</button>
                        <button onClick={onEvaluateCombo} disabled={buildBusy}>Try Combo</button>
                    </div>

                    <div style={{ fontSize: 12, opacity: 0.9, marginBottom: 6 }}>
                        Stats: {buildStats ? `HP ${buildStats.health} · ATK ${buildStats.attack} · DEF ${buildStats.defense} · UTL ${buildStats.utility}` : "uninitialized"}
                    </div>
                    <div style={{ fontSize: 12, opacity: 0.9 }}>
                        Combo: {comboResult ? `${comboResult.triggered ? "triggered" : "not triggered"} · dmg +${comboResult.damageBonusPct}% · mit +${comboResult.mitigationBonusPct}% · party +${comboResult.partySynergyBonusPct}%` : "none"}
                    </div>

                    {nativeUiFrame ? (
                        <div style={{ fontSize: 11, opacity: 0.75, marginTop: 8 }}>
                            ABI: {nativeUiFrame.host} · {nativeUiFrame.surface} · {nativeUiFrame.engineStatus}
                        </div>
                    ) : null}
                </aside>
            ) : null}
        </>
    );
}
