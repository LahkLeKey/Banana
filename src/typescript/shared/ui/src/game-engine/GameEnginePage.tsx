import { useEffect, useLayoutEffect, useRef, useState } from "react";

import type { EngineStatus } from "../components/GameEngineTypes";
import { GameEngineUIService, type GameEngineUIState } from "../domains/GameEngineUIService";
import { NetcodeDomain } from "../domains/NetcodeDomain";
import { DeterministicLocalSimulator, PredictionDomain } from "../domains/PredictionDomain";
import { ReplicationDomain } from "../domains/ReplicationDomain";
import type { GameSessionBootstrap, GameSessionPlayer } from "./api";
import { resolveApiBaseResolutionError } from "./api";
import { isWebGL2Available, loadEngineAssetVersion, startDevEngineVersionWatcher } from "./engineVersion";
import { buildDancingBananaParticles } from "./particles";
import { attachRuntimeOrchestrator } from "./runtimeOrchestrator";
import { ensureGameSessionBootstrap } from "./sessionBootstrap";
import { attachGameSessionSocket } from "./sessionSocket";
import { type BananaEngineModule, type DancingBananaParticle, PBJ_PICKUP_COLLECT_RADIUS, PBJ_PICKUP_WORLD_POSITION, type WorldPosition } from "./types";
import { attachViewportCanvasSizing } from "./viewportSizing";

declare global {
    interface Window {
        BananaEngine?: (config: {
            canvas: HTMLCanvasElement;
            locateFile?: (path: string, prefix: string) => string;
            printErr?: (text: string) => void;
            onAbort?: (reason: string) => void;
        }) => Promise<BananaEngineModule>;
        __bananaEngineModule?: BananaEngineModule;
    }
}

export function GameEnginePage() {
    const canvasRef = useRef<HTMLCanvasElement>(null);
    const viewportRef = useRef<HTMLDivElement>(null);
    const moduleRef = useRef<BananaEngineModule | null>(null);
    const scriptRef = useRef<HTMLScriptElement | null>(null);
    const websocketRef = useRef<WebSocket | null>(null);
    const outboundSequenceRef = useRef(0);
    const outboundTickRef = useRef(0);
    const netcodeRef = useRef<NetcodeDomain | null>(null);
    const predictionRef = useRef<PredictionDomain | null>(null);
    const replicationRef = useRef<ReplicationDomain | null>(null);
    const renderedRemotePlayerIdsRef = useRef<Set<string>>(new Set());
    const socketCleanupRef = useRef<(() => void) | null>(null);
    const runtimeCleanupRef = useRef<(() => void) | null>(null);

    const uiServiceRef = useRef<GameEngineUIService | null>(null);
    const renderHealthTimerRef = useRef<number>(0);

    const [status, setStatus] = useState<EngineStatus>("idle");
    const [error, setError] = useState<string | null>(null);
    const [engineAssetVersion, setEngineAssetVersion] = useState<string>("pending");
    const [sessionBootstrap, setSessionBootstrap] = useState<GameSessionBootstrap | null>(null);
    const localPlayerIdRef = useRef<string | null>(null);
    const playerRosterSignatureRef = useRef("");
    const [localPlayerWorldPosition, setLocalPlayerWorldPosition] = useState<WorldPosition | null>(null);
    const [pbjPickupAvailable, setPbjPickupAvailable] = useState(true);
    const [bananaParticles, setBananaParticles] = useState<DancingBananaParticle[]>([]);
    const particleIdRef = useRef(0);
    const particleClearTimerRef = useRef<number>(0);

    const applySessionPlayers = (players: GameSessionPlayer[]) => {
        const nextPlayers = players ?? [];
        const signature = nextPlayers
            .map((player) => `${player.playerId}:${player.playerName}:${player.connected ? 1 : 0}`)
            .join("|");

        if (signature === playerRosterSignatureRef.current) {
            return;
        }

        playerRosterSignatureRef.current = signature;
    };

    const spawnDancingBananaParticles = () => {
        const particles = buildDancingBananaParticles(particleIdRef.current);

        particleIdRef.current += particles.length;
        setBananaParticles(particles);
        window.clearTimeout(particleClearTimerRef.current);
        particleClearTimerRef.current = window.setTimeout(() => {
            setBananaParticles([]);
        }, 4800);
    };

    useEffect(() => {
        if (!pbjPickupAvailable || !localPlayerWorldPosition) {
            return;
        }

        const pickupDistance = Math.hypot(
            localPlayerWorldPosition.x - PBJ_PICKUP_WORLD_POSITION.x,
            localPlayerWorldPosition.z - PBJ_PICKUP_WORLD_POSITION.z
        );

        if (pickupDistance > PBJ_PICKUP_COLLECT_RADIUS) {
            return;
        }

        setPbjPickupAvailable(false);
        spawnDancingBananaParticles();
    }, [pbjPickupAvailable, localPlayerWorldPosition]);

    useEffect(() => {
        let cancelled = false;

        const ensureSession = async () => {
            const result = await ensureGameSessionBootstrap();
            if (cancelled) {
                return;
            }

            if (result.error) {
                setError(result.error);
                return;
            }

            if (result.bootstrap) {
                setSessionBootstrap(result.bootstrap);
            }
        };

        void ensureSession();

        return () => {
            cancelled = true;
        };
    }, []);

    useEffect(() => {
        if (!sessionBootstrap) return;

        const resolutionError = resolveApiBaseResolutionError();
        if (resolutionError) {
            setError(resolutionError);
            return;
        }

        socketCleanupRef.current?.();
        socketCleanupRef.current = attachGameSessionSocket({
            sessionBootstrap,
            moduleRef,
            websocketRef,
            outboundSequenceRef,
            outboundTickRef,
            localPlayerIdRef,
            renderedRemotePlayerIdsRef,
            predictionRef,
            netcodeRef,
            replicationRef,
            setError,
            setLocalPlayerWorldPosition,
            applySessionPlayers,
        });

        return () => {
            socketCleanupRef.current?.();
            socketCleanupRef.current = null;
            window.clearTimeout(particleClearTimerRef.current);
        };
    }, [sessionBootstrap]);

    useLayoutEffect(() => {
        const canvas = canvasRef.current;
        const viewport = viewportRef.current;
        if (!canvas || !viewport) return;

        // biome-ignore lint: useUniqueElementIds: Emscripten native code hardcodes #canvas.
        canvas.id = "canvas";

        return attachViewportCanvasSizing(canvas, viewport);
    }, []);

    useEffect(() => {
        const stopWatcher = startDevEngineVersionWatcher(() => {
            window.location.reload();
        });
        return () => {
            stopWatcher?.();
        };
    }, []);

    useEffect(() => {
        let canceled = false;

        const loadVersion = async () => {
            const version = await loadEngineAssetVersion();
            if (!canceled) {
                setEngineAssetVersion(version);
            }
        };

        void loadVersion();
        return () => {
            canceled = true;
        };
    }, []);

    const handleNativeContextAction = (clientX: number, clientY: number) => {
        const mod = moduleRef.current;
        const viewport = viewportRef.current;
        if (!mod || !viewport) {
            return;
        }

        const bounds = viewport.getBoundingClientRect();
        const normalizedX = Math.max(0, Math.min(1, (clientX - bounds.left) / Math.max(1, bounds.width)));
        const normalizedY = Math.max(0, Math.min(1, (clientY - bounds.top) / Math.max(1, bounds.height)));

        mod.ccall("engine_handle_right_click_normalized", "number", ["number", "number"], [normalizedX, normalizedY]);
    };

    useEffect(() => {
        if (engineAssetVersion === "pending") return;

        const canvas = canvasRef.current;
        const viewport = viewportRef.current;
        if (!canvas || !viewport) return;

        const hasPreloadedEngineFactory = typeof window.BananaEngine === "function";
        if (!hasPreloadedEngineFactory && !isWebGL2Available()) {
            setStatus("unavailable");
            setError(
                "This browser/device cannot create a WebGL2 context. Open on a WebGL2-capable browser/device or enable hardware acceleration."
            );
            return;
        }

        setStatus("loading");

        const startEngine = async () => {
            if (!window.BananaEngine) {
                setStatus("error");
                setError("BananaEngine factory not found after script load");
                return;
            }

            const uiService = new GameEngineUIService(viewport);
            uiServiceRef.current = uiService;

            if (!netcodeRef.current) {
                netcodeRef.current = new NetcodeDomain({ tickRate: 30, maxPrediction: 60 }, uiService.getInputAggregator());
            }
            if (!predictionRef.current) {
                predictionRef.current = new PredictionDomain(new DeterministicLocalSimulator(), 120);
            }
            if (!replicationRef.current) {
                replicationRef.current = new ReplicationDomain({ maxHistory: 120 });
            }

            const unsubscribeStateChanges = uiService.onStateChange((uiState: GameEngineUIState) => {
                setStatus(uiState.engineStatus);
                setError(uiState.error ?? null);
            });

            let runtimeInitFailed = false;
            const applyRuntimeFatal = (rawMessage: unknown) => {
                const normalized = typeof rawMessage === "string" ? rawMessage : String(rawMessage ?? "");
                const message = normalized.toLowerCase();
                if (!message) return;
                const isFatalInit =
                    message.includes("webgl2 context creation failed") ||
                    message.includes("window_create failed") ||
                    message.includes("engine_init failed") ||
                    message.includes("abort");
                if (!isFatalInit) return;
                runtimeInitFailed = true;
            };

            try {
                const mod = await window.BananaEngine({
                    canvas,
                    locateFile: (path, prefix) => `${prefix}${path}?v=${engineAssetVersion}`,
                    printErr: (text) => {
                        applyRuntimeFatal(text);
                    },
                    onAbort: (reason) => {
                        applyRuntimeFatal(reason);
                    },
                });

                if (runtimeInitFailed) {
                    unsubscribeStateChanges();
                    return;
                }

                moduleRef.current = mod;
                window.__bananaEngineModule = mod;

                uiService.setEngineStatus("running");

                runtimeCleanupRef.current?.();
                runtimeCleanupRef.current = attachRuntimeOrchestrator({
                    mod,
                    canvas,
                    viewport,
                    uiService,
                    sessionBootstrap,
                    websocketRef,
                    outboundSequenceRef,
                    outboundTickRef,
                    netcodeRef,
                    predictionRef,
                    replicationRef,
                    renderHealthTimerRef,
                });
            } catch (e) {
                uiService.setEngineStatus("error", e instanceof Error ? e.message : "Engine initialization failed");
                unsubscribeStateChanges();
            }
        };

        if (typeof window.BananaEngine === "function") {
            void startEngine();
        } else {
            const script = document.createElement("script");
            script.src = `/wasm/engine.js?v=${engineAssetVersion}`;
            scriptRef.current = script;

            script.onerror = () => {
                setStatus("unavailable");
                setError("engine.js not found. Run: bash scripts/build-engine-wasm.sh (requires Emscripten SDK)");
            };

            script.onload = () => {
                void startEngine();
            };

            document.head.appendChild(script);
        }

        return () => {
            runtimeCleanupRef.current?.();
            runtimeCleanupRef.current = null;
            window.clearTimeout(renderHealthTimerRef.current);

            scriptRef.current?.remove();
            delete window.__bananaEngineModule;
            moduleRef.current = null;
            uiServiceRef.current = null;
            netcodeRef.current = null;
            predictionRef.current = null;
            replicationRef.current = null;
        };
    }, [engineAssetVersion, sessionBootstrap]);

    return (
        <div
            ref={viewportRef}
            style={{
                position: "fixed",
                inset: 0,
                width: "100vw",
                height: "100vh",
                minHeight: "100dvh",
                margin: 0,
                padding: 0,
                overflow: "hidden",
                backgroundColor: "#000",
                touchAction: "none",
            }}
        >
            <canvas
                id="canvas"
                ref={canvasRef}
                onContextMenu={(event) => {
                    event.preventDefault();
                    handleNativeContextAction(event.clientX, event.clientY);
                }}
                style={{
                    display: "block",
                    width: "100%",
                    height: "100%",
                    imageRendering: "pixelated",
                    touchAction: "none",
                }}
            />

            {(status === "error" || status === "unavailable") && error && (
                <div
                    style={{
                        position: "absolute",
                        top: 16,
                        left: 16,
                        right: 16,
                        zIndex: 20,
                        padding: 12,
                        borderRadius: 8,
                        background: "rgba(40, 0, 0, 0.85)",
                        color: "#ffd7d7",
                        border: "1px solid rgba(255, 130, 130, 0.45)",
                    }}
                >
                    <div style={{ marginBottom: 8, fontWeight: 600 }}>Engine Runtime Error</div>
                    <div style={{ marginBottom: 10 }}>{error}</div>
                    <button onClick={() => window.location.reload()}>Reload</button>
                </div>
            )}

            {bananaParticles.length > 0 && (
                <>
                    <style>{`
            @keyframes bananaDanceBurst {
              0% { opacity: 0; transform: translate3d(0, 10px, 0) scale(0.5) rotate(0deg); }
              12% { opacity: 1; transform: translate3d(0, -4px, 0) scale(1) rotate(-8deg); }
              45% { opacity: 1; transform: translate3d(-12px, -36px, 0) scale(1.1) rotate(12deg); }
              100% { opacity: 0; transform: translate3d(0, -150px, 0) scale(0.6) rotate(28deg); }
            }
          `}</style>
                    <div
                        style={{
                            position: "absolute",
                            inset: 0,
                            zIndex: 34,
                            pointerEvents: "none",
                            overflow: "hidden",
                        }}
                    >
                        {bananaParticles.map((particle) => (
                            <span
                                key={particle.id}
                                style={{
                                    position: "absolute",
                                    left: `${particle.leftPct}%`,
                                    top: `${particle.topPct}%`,
                                    display: "inline-block",
                                    fontSize: `${particle.sizeRem}rem`,
                                    animationName: "bananaDanceBurst",
                                    animationDuration: `${particle.durationMs}ms`,
                                    animationDelay: `${particle.delayMs}ms`,
                                    animationTimingFunction: "cubic-bezier(0.2, 0.9, 0.25, 1)",
                                    animationFillMode: "forwards",
                                    willChange: "transform, opacity",
                                    filter: "drop-shadow(0 3px 8px rgba(0,0,0,0.45))",
                                }}
                            >
                                {particle.glyph}
                            </span>
                        ))}
                    </div>
                </>
            )}
        </div>
    );
}