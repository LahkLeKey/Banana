import {dlopen, FFIType, ptr, suffix} from "bun:ffi";
import path from "node:path";

export interface AntiCheatScoreResult {
    readonly score: number;
    readonly flagged: boolean;
    readonly integrityHash: number;
}

export interface UsermodeHeartbeatInput {
    readonly sessionId: string;
    readonly heartbeatSequence: number;
    readonly simulationTick: number;
    readonly debuggerPresent: boolean;
    readonly codeIntegrityViolations: number;
    readonly memoryMapAnomalies: number;
    readonly timingAnomalies: number;
}

export interface DriverHeartbeatInput {
    readonly sessionId: string;
    readonly heartbeatSequence: number;
    readonly suspiciousRing12DriverCount: number;
    readonly unsignedDriverCount: number;
    readonly hiddenModuleCount: number;
}

export interface AntiCheatInteropAdapter {
    resetSession(sessionId: string): Promise<void>;
    submitUsermodeHeartbeat(input: UsermodeHeartbeatInput): Promise<AntiCheatScoreResult>;
    submitDriverTelemetry(input: DriverHeartbeatInput): Promise<AntiCheatScoreResult>;
    getSessionStatus(sessionId: string): Promise<AntiCheatScoreResult>;
}

const BANANA_OK = 0;

type NativeAntiCheatSymbols = {
    banana_anticheat_reset_session: (sessionIdPtr: bigint) => number;
    banana_anticheat_submit_usermode_heartbeat :
        (sessionIdPtr: bigint, heartbeatSequence: number, simulationTick: number,
         debuggerPresent: number, codeIntegrityViolations: number, memoryMapAnomalies: number,
         timingAnomalies: number, outScorePtr: bigint, outFlaggedPtr: bigint) => number;
    banana_anticheat_submit_driver_telemetry :
        (sessionIdPtr: bigint, heartbeatSequence: number, suspiciousRing12DriverCount: number,
         unsignedDriverCount: number, hiddenModuleCount: number, outScorePtr: bigint,
         outFlaggedPtr: bigint) => number;
    banana_anticheat_get_session_status : (sessionIdPtr: bigint, outScorePtr: bigint,
                                           outFlaggedPtr: bigint, outIntegrityHashPtr: bigint) =>
        number;
};

function cStringPtr(value: string): bigint
{
    return ptr(Buffer.from(`${value}\0`, "utf8"));
}

function boolToInt(value: boolean): number
{
    return value ? 1 : 0;
}

function resolveNativeLibraryCandidates(): string[]
{
    const ext = suffix;
    const names = [
        `libbanana_native.${ext}`,
        `banana_native.${ext}`,
    ];

    const envPath = process.env.BANANA_NATIVE_PATH;
    const candidates: string[] = [];

    if (envPath && envPath.trim().length > 0)
    {
        const trimmed = envPath.trim();
        if (trimmed.endsWith(`.${ext}`))
        {
            candidates.push(trimmed);
        }
        else
        {
            for (const name of names)
            {
                candidates.push(path.join(trimmed, name));
            }
        }
    }

    const repoRoot = path.resolve(process.cwd(), "../../..");
    const fallbackDirs = [
        "out/native/bin",
        "build/native/bin",
        "build/native",
        "build/native/Debug",
        "build/native/Release",
        "build/cmake-tools",
        "build/Release",
        "build/sanitizers",
        "build/native-static-analysis",
        "out/native-anticheat-check/Release",
    ];
    for (const name of names)
    {
        for (const dir of fallbackDirs)
        {
            candidates.push(path.join(repoRoot, dir, name));
        }
    }

    return Array.from(new Set(candidates));
}

function createNativeBinding(): NativeAntiCheatSymbols
{
    const candidates = resolveNativeLibraryCandidates();
    let lastError: unknown = null;

    for (const candidate of candidates)
    {
        try
        {
            const symbols = dlopen(candidate, {
                banana_anticheat_reset_session : {
                    args : [ FFIType.ptr ],
                    returns : FFIType.i32,
                },
                banana_anticheat_submit_usermode_heartbeat : {
                    args : [
                        FFIType.ptr,
                        FFIType.u32,
                        FFIType.u32,
                        FFIType.i32,
                        FFIType.i32,
                        FFIType.i32,
                        FFIType.i32,
                        FFIType.ptr,
                        FFIType.ptr,
                    ],
                    returns : FFIType.i32,
                },
                banana_anticheat_submit_driver_telemetry : {
                    args : [
                        FFIType.ptr,
                        FFIType.u32,
                        FFIType.i32,
                        FFIType.i32,
                        FFIType.i32,
                        FFIType.ptr,
                        FFIType.ptr,
                    ],
                    returns : FFIType.i32,
                },
                banana_anticheat_get_session_status : {
                    args : [ FFIType.ptr, FFIType.ptr, FFIType.ptr, FFIType.ptr ],
                    returns : FFIType.i32,
                },
            });
            return symbols.symbols as NativeAntiCheatSymbols;
        }
        catch (error)
        {
            lastError = error;
        }
    }

    throw new Error(`Unable to load Banana native library for anti-cheat FFI. Candidates: ${
        candidates.join(", ")}. Last error: ${String(lastError)}`);
}

function assertStatus(operation: string, statusCode: number): void
{
    if (statusCode !== BANANA_OK)
    {
        throw new Error(`${operation} failed with native status ${statusCode}`);
    }
}

export class NativeFFIAntiCheatInteropAdapter implements AntiCheatInteropAdapter
{
    private readonly symbols = createNativeBinding();

    async resetSession(sessionId: string): Promise<void>
    {
        const statusCode = this.symbols.banana_anticheat_reset_session(cStringPtr(sessionId));
        assertStatus("banana_anticheat_reset_session", statusCode);
    }

    async submitUsermodeHeartbeat(input: UsermodeHeartbeatInput): Promise<AntiCheatScoreResult>
    {
        const score = new Int32Array(1);
        const flagged = new Int32Array(1);
        const statusCode = this.symbols.banana_anticheat_submit_usermode_heartbeat(
            cStringPtr(input.sessionId), input.heartbeatSequence >>> 0, input.simulationTick >>> 0,
            boolToInt(input.debuggerPresent), ensureNonNegative(input.codeIntegrityViolations),
            ensureNonNegative(input.memoryMapAnomalies), ensureNonNegative(input.timingAnomalies),
            ptr(score), ptr(flagged));
        assertStatus("banana_anticheat_submit_usermode_heartbeat", statusCode);

        const status = await this.getSessionStatus(input.sessionId);
        return {
            score : score[0],
            flagged : flagged[0] !== 0,
            integrityHash : status.integrityHash,
        };
    }

    async submitDriverTelemetry(input: DriverHeartbeatInput): Promise<AntiCheatScoreResult>
    {
        const score = new Int32Array(1);
        const flagged = new Int32Array(1);
        const statusCode = this.symbols.banana_anticheat_submit_driver_telemetry(
            cStringPtr(input.sessionId), input.heartbeatSequence >>> 0,
            ensureNonNegative(input.suspiciousRing12DriverCount),
            ensureNonNegative(input.unsignedDriverCount),
            ensureNonNegative(input.hiddenModuleCount), ptr(score), ptr(flagged));
        assertStatus("banana_anticheat_submit_driver_telemetry", statusCode);

        const status = await this.getSessionStatus(input.sessionId);
        return {
            score : score[0],
            flagged : flagged[0] !== 0,
            integrityHash : status.integrityHash,
        };
    }

    async getSessionStatus(sessionId: string): Promise<AntiCheatScoreResult>
    {
        const score = new Int32Array(1);
        const flagged = new Int32Array(1);
        const integrityHash = new Uint32Array(1);
        const statusCode = this.symbols.banana_anticheat_get_session_status(
            cStringPtr(sessionId), ptr(score), ptr(flagged), ptr(integrityHash));
        assertStatus("banana_anticheat_get_session_status", statusCode);
        return {
            score : score[0],
            flagged : flagged[0] !== 0,
            integrityHash : integrityHash[0] >>> 0,
        };
    }
}

type AntiCheatState = {
    score: number; flagged : boolean; integrityHash : number; lastUsermodeHeartbeat : number;
    lastDriverHeartbeat : number;
    lastSimulationTick : number;
};

function hashSession(sessionId: string): number
{
    let hash = 2166136261;
    for (let i = 0; i < sessionId.length; i += 1)
    {
        hash ^= sessionId.charCodeAt(i) & 0xff;
        hash = Math.imul(hash, 16777619) >>> 0;
    }
    return hash >>> 0;
}

function mixHash(hash: number, value: number): number
{
    const mixed = Math.imul((hash ^ (value >>> 0)) >>> 0, 16777619);
    return mixed >>> 0;
}

function ensureNonNegative(value: number): number
{
    if (!Number.isFinite(value) || value < 0)
        return 0;
    return Math.floor(value);
}

export class InMemoryAntiCheatInteropAdapter implements AntiCheatInteropAdapter
{
    private readonly sessions = new Map<string, AntiCheatState>();

    async resetSession(sessionId: string): Promise<void>
    {
        this.sessions.set(sessionId, {
            score : 0,
            flagged : false,
            integrityHash : hashSession(sessionId),
            lastUsermodeHeartbeat : 0,
            lastDriverHeartbeat : 0,
            lastSimulationTick : 0,
        });
    }

    async submitUsermodeHeartbeat(input: UsermodeHeartbeatInput): Promise<AntiCheatScoreResult>
    {
        const state = this.ensureSession(input.sessionId);

        let delta = 0;
        if (input.heartbeatSequence <= state.lastUsermodeHeartbeat)
        {
            delta += 2;
        }
        else if (state.lastUsermodeHeartbeat > 0 &&
                 input.heartbeatSequence - state.lastUsermodeHeartbeat > 8)
        {
            delta += 1;
        }

        if (input.simulationTick + 2 < state.lastSimulationTick)
        {
            delta += 2;
        }

        if (input.debuggerPresent)
            delta += 2;
        delta += ensureNonNegative(input.codeIntegrityViolations) * 3;
        delta += ensureNonNegative(input.memoryMapAnomalies) * 2;
        delta += ensureNonNegative(input.timingAnomalies);

        if (delta === 0 && state.score > 0)
        {
            state.score -= 1;
        }
        else
        {
            state.score += delta;
        }

        if (state.score < 0)
            state.score = 0;
        state.flagged = state.score >= 8;

        if (input.heartbeatSequence > state.lastUsermodeHeartbeat)
        {
            state.lastUsermodeHeartbeat = input.heartbeatSequence;
        }
        if (input.simulationTick > state.lastSimulationTick)
        {
            state.lastSimulationTick = input.simulationTick;
        }

        state.integrityHash = mixHash(state.integrityHash, input.heartbeatSequence);
        state.integrityHash = mixHash(state.integrityHash, input.simulationTick);
        state.integrityHash =
            mixHash(state.integrityHash, (input.debuggerPresent ? 1 : 0) +
                                             ensureNonNegative(input.codeIntegrityViolations) * 5 +
                                             ensureNonNegative(input.memoryMapAnomalies) * 7 +
                                             ensureNonNegative(input.timingAnomalies) * 11);

        return {
            score : state.score,
            flagged : state.flagged,
            integrityHash : state.integrityHash,
        };
    }

    async submitDriverTelemetry(input: DriverHeartbeatInput): Promise<AntiCheatScoreResult>
    {
        const state = this.ensureSession(input.sessionId);

        let delta = 0;
        if (input.heartbeatSequence <= state.lastDriverHeartbeat)
        {
            delta += 2;
        }
        else if (state.lastDriverHeartbeat > 0 &&
                 input.heartbeatSequence - state.lastDriverHeartbeat > 8)
        {
            delta += 1;
        }

        delta += ensureNonNegative(input.suspiciousRing12DriverCount) * 4;
        delta += ensureNonNegative(input.unsignedDriverCount) * 2;
        delta += ensureNonNegative(input.hiddenModuleCount) * 3;

        if (delta === 0 && state.score > 0)
        {
            state.score -= 1;
        }
        else
        {
            state.score += delta;
        }

        if (state.score < 0)
            state.score = 0;
        state.flagged = state.score >= 8;

        if (input.heartbeatSequence > state.lastDriverHeartbeat)
        {
            state.lastDriverHeartbeat = input.heartbeatSequence;
        }

        state.integrityHash = mixHash(state.integrityHash, input.heartbeatSequence);
        state.integrityHash =
            mixHash(state.integrityHash, ensureNonNegative(input.suspiciousRing12DriverCount) * 13 +
                                             ensureNonNegative(input.unsignedDriverCount) * 17 +
                                             ensureNonNegative(input.hiddenModuleCount) * 19);

        return {
            score : state.score,
            flagged : state.flagged,
            integrityHash : state.integrityHash,
        };
    }

    async getSessionStatus(sessionId: string): Promise<AntiCheatScoreResult>
    {
        const state = this.ensureSession(sessionId);
        return {
            score : state.score,
            flagged : state.flagged,
            integrityHash : state.integrityHash,
        };
    }

    private ensureSession(sessionId: string): AntiCheatState
    {
        const existing = this.sessions.get(sessionId);
        if (existing)
            return existing;

        const created: AntiCheatState = {
            score : 0,
            flagged : false,
            integrityHash : hashSession(sessionId),
            lastUsermodeHeartbeat : 0,
            lastDriverHeartbeat : 0,
            lastSimulationTick : 0,
        };
        this.sessions.set(sessionId, created);
        return created;
    }
}

export function createDefaultAntiCheatInteropAdapter(): AntiCheatInteropAdapter
{
    const adapterMode = (process.env.BANANA_ANTICHEAT_ADAPTER ?? "ffi").toLowerCase();
    const requireNative = process.env.BANANA_ANTICHEAT_REQUIRE_NATIVE === "true";

    if (adapterMode === "inmemory")
    {
        return new InMemoryAntiCheatInteropAdapter();
    }

    try
    {
        return new NativeFFIAntiCheatInteropAdapter();
    }
    catch (error)
    {
        if (adapterMode === "ffi-only" || requireNative)
        {
            throw error;
        }
        return new InMemoryAntiCheatInteropAdapter();
    }
}
