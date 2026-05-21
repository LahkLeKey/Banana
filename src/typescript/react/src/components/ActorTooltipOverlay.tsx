import type { GameSessionSnapshotEntity } from "../lib/api";

type ActorTooltipOverlayProps = {
    readonly entities: Record<string, GameSessionSnapshotEntity>;
    readonly localPlayerId: string | null;
};

const WORLD_SIZE = 30;

function clamp(value: number, min: number, max: number): number {
    return Math.max(min, Math.min(max, value));
}

export function ActorTooltipOverlay({ entities, localPlayerId }: ActorTooltipOverlayProps) {
    const labeledPlayers = Object.entries(entities).filter(([, entity]) => {
        if (entity.type !== "player") return false;
        return Boolean(entity.tooltipLabel || entity.ownerPlayerName || entity.ownerPlayerId);
    });

    if (labeledPlayers.length === 0) {
        return null;
    }

    return (
        <div
            aria-label="Actor labels"
            style={{
                position: "absolute",
                inset: 0,
                pointerEvents: "none",
                zIndex: 18,
            }}
        >
            {labeledPlayers.map(([entityKey, entity]) => {
                const left = clamp((entity.x / WORLD_SIZE) * 92 + 4, 4, 96);
                const top = clamp((entity.z / WORLD_SIZE) * 86 + 8, 8, 94);
                const label = entity.tooltipLabel ?? entity.ownerPlayerName ?? entity.ownerPlayerId ?? entityKey;
                const isLocalPlayer = Boolean(localPlayerId && entity.ownerPlayerId === localPlayerId);

                return (
                    <div key={entityKey}>
                        <div
                            style={{
                                position: "absolute",
                                left: `${left}%`,
                                top: `${top}%`,
                                transform: "translate(-50%, -70%)",
                                width: 14,
                                height: 24,
                                borderRadius: 999,
                                background: isLocalPlayer
                                    ? "linear-gradient(180deg, rgba(74, 222, 128, 0.95), rgba(21, 128, 61, 0.88))"
                                    : "linear-gradient(180deg, rgba(147, 197, 253, 0.95), rgba(37, 99, 235, 0.88))",
                                border: isLocalPlayer
                                    ? "1px solid rgba(34, 197, 94, 0.95)"
                                    : "1px solid rgba(96, 165, 250, 0.95)",
                                boxShadow: isLocalPlayer
                                    ? "0 0 14px rgba(34, 197, 94, 0.55)"
                                    : "0 0 14px rgba(96, 165, 250, 0.55)",
                            }}
                        />
                        <div
                            style={{
                                position: "absolute",
                                left: `${left}%`,
                                top: `${top}%`,
                                transform: "translate(-50%, -145%)",
                                display: "inline-flex",
                                alignItems: "center",
                                gap: 6,
                            }}
                        >
                            <div
                                style={{
                                    width: 7,
                                    height: 7,
                                    borderRadius: "999px",
                                    background: isLocalPlayer ? "#22c55e" : "#60a5fa",
                                    boxShadow: isLocalPlayer ? "0 0 8px rgba(34, 197, 94, 0.75)" : "0 0 8px rgba(96, 165, 250, 0.75)",
                                }}
                            />
                            <span
                                title={label}
                                style={{
                                    padding: "3px 8px",
                                    borderRadius: 999,
                                    fontSize: 11,
                                    lineHeight: 1,
                                    color: "#f8fafc",
                                    background: isLocalPlayer ? "rgba(21, 128, 61, 0.82)" : "rgba(30, 41, 59, 0.82)",
                                    border: isLocalPlayer
                                        ? "1px solid rgba(34, 197, 94, 0.8)"
                                        : "1px solid rgba(148, 163, 184, 0.5)",
                                    backdropFilter: "blur(4px)",
                                    whiteSpace: "nowrap",
                                }}
                            >
                                {label}
                            </span>
                        </div>
                    </div>
                );
            })}
        </div>
    );
}
