import {dlopen, FFIType, suffix} from 'bun:ffi';
import path from 'node:path';

export interface NativeEngineInput {
  readonly moveX: -1|0|1;
  readonly moveZ: -1|0|1;
  readonly dtSeconds?: number;
}

export interface NativeEnginePlayerSync {
  readonly playerId: string;
  readonly playerName: string;
  readonly controllerKind: 'human'|'ai';
  readonly connected: boolean;
  readonly moveX: -1|0|1;
  readonly moveZ: -1|0|1;
}

export interface NativeEngineSnapshotEntity {
  readonly id: number;
  readonly type: string;
  readonly active: boolean;
  readonly x: number;
  readonly y: number;
  readonly z: number;
  readonly state: number;
  readonly playerGuid?: string;
  readonly controllerKind?: 'human'|'ai';
}

export interface NativeEngineSnapshot {
  readonly tick: number;
  readonly timestamp: number;
  readonly isAuthoritative: true;
  readonly entities: Record<string, NativeEngineSnapshotEntity>;
}

export interface NativeEngineService {
  syncPlayers(players: readonly NativeEnginePlayerSync[]): Promise<void>;
  playerSyncMarkSeen(playerGuid: string, currentTimeMs: bigint): Promise<void>;
  playerSyncUpdateStaleness(currentTimeMs: bigint, staleThresholdMs: bigint):
      Promise<void>;
  playerSyncDeactivateStale(): Promise<void>;
  playerSyncCountActive(): Promise<number>;
  tickSerialize(input: NativeEngineInput): Promise<NativeEngineSnapshot>;
  reset(): Promise<void>;
  shutdown(): Promise<void>;
}

type NativeEngineSymbols = {
  engine_init: (width: number, height: number) => number;
  engine_set_move_input: (moveX: number, moveZ: number) => void;
  engine_player_upsert:
      (playerGuid: string, playerName: string, controllerKind: string,
       active: number) => number;
  engine_player_apply_input:
      (playerGuid: string, moveX: number, moveZ: number) => void;
  engine_player_sync_mark_seen: (playerGuid: string, currentTimeMs: bigint) =>
      void;
  engine_player_sync_update_staleness:
      (currentTimeMs: bigint, staleThresholdMs: bigint) => void;
  engine_player_sync_deactivate_stale: () => void;
  engine_player_sync_count_active: () => number;
  engine_tick_serialize: (dtSeconds: number) => string | {toString(): string};
  engine_shutdown: () => void;
};

const BANANA_OK = 0;

function assertAxis(value: number): value is - 1|0|1 {
  return value === -1 || value === 0 || value === 1;
}

function assertSnapshot(snapshot: unknown):
    asserts snapshot is NativeEngineSnapshot {
  if (typeof snapshot !== 'object' || snapshot === null ||
      typeof (snapshot as {tick?: unknown}).tick !== 'number' ||
      typeof (snapshot as {timestamp?: unknown}).timestamp !== 'number' ||
      (snapshot as {isAuthoritative?: unknown}).isAuthoritative !== true ||
      typeof (snapshot as {entities?: unknown}).entities !== 'object' ||
      (snapshot as {entities?: unknown}).entities === null) {
    throw new Error('Native engine snapshot failed schema validation.');
  }
}

function parseSnapshot(jsonSnapshot: string): NativeEngineSnapshot {
  const parsed = JSON.parse(jsonSnapshot) as unknown;
  if (typeof parsed === 'object' && parsed !== null && 'error' in parsed &&
      typeof (parsed as {error?: unknown}).error === 'string') {
    throw new Error(`Native engine returned snapshot error: ${
        (parsed as {error: string}).error}`);
  }

  assertSnapshot(parsed);
  return parsed;
}

function normalizeCString(snapshot: string|{toString(): string}): string {
  return typeof snapshot === 'string' ? snapshot : snapshot.toString();
}

function resolveNativeLibraryCandidates(): string[] {
  const ext = suffix;
  const names = [`libbanana_native.${ext}`, `banana_native.${ext}`];
  const envPath = process.env.BANANA_NATIVE_PATH;
  const candidates: string[] = [];

  if (envPath && envPath.trim().length > 0) {
    const trimmed = envPath.trim();
    if (trimmed.endsWith(`.${ext}`)) {
      candidates.push(trimmed);
    } else {
      for (const name of names) {
        candidates.push(path.join(trimmed, name));
      }
    }
  }

  const repoRoot = path.resolve(process.cwd(), '../../..');
  const fallbackDirs = [
    'out/native/bin',
    'out/v3-native/Debug',
    'out/v3-native/Release',
    'out/v3-native-baseline/Debug',
    'build/native/bin',
    'build/native',
  ];

  for (const name of names) {
    for (const dir of fallbackDirs) {
      candidates.push(path.join(repoRoot, dir, name));
    }
  }

  return Array.from(new Set(candidates));
}

function createNativeBinding(): NativeEngineSymbols {
  const candidates = resolveNativeLibraryCandidates();
  let lastError: unknown = null;

  for (const candidate of candidates) {
    try {
      const library = dlopen(candidate, {
        engine_init: {
          args: [FFIType.i32, FFIType.i32],
          returns: FFIType.i32,
        },
        engine_set_move_input: {
          args: [FFIType.f32, FFIType.f32],
          returns: FFIType.void,
        },
        engine_player_upsert: {
          args:
              [FFIType.cstring, FFIType.cstring, FFIType.cstring, FFIType.i32],
          returns: FFIType.i32,
        },
        engine_player_apply_input: {
          args: [FFIType.cstring, FFIType.f32, FFIType.f32],
          returns: FFIType.void,
        },
        engine_player_sync_mark_seen: {
          args: [FFIType.cstring, FFIType.i64],
          returns: FFIType.void,
        },
        engine_player_sync_update_staleness: {
          args: [FFIType.i64, FFIType.i64],
          returns: FFIType.void,
        },
        engine_player_sync_deactivate_stale: {
          args: [],
          returns: FFIType.void,
        },
        engine_player_sync_count_active: {
          args: [],
          returns: FFIType.i32,
        },
        engine_tick_serialize: {
          args: [FFIType.f32],
          returns: FFIType.cstring,
        },
        engine_shutdown: {
          args: [],
          returns: FFIType.void,
        },
      });

      return library.symbols as NativeEngineSymbols;
    } catch (error) {
      lastError = error;
    }
  }

  throw new Error(
      `Unable to load Banana native library for engine FFI. Candidates: ${
          candidates.join(', ')}. Last error: ${String(lastError)}`);
}

export class NativeFFIEngineService implements NativeEngineService {
  private readonly symbols = createNativeBinding();
  private initialized = false;

  public async syncPlayers(players: readonly NativeEnginePlayerSync[]):
      Promise<void> {
    await this.ensureReady();
    for (const player of players) {
      this.symbols.engine_player_upsert(
          player.playerId, player.playerName, player.controllerKind,
          player.connected ? 1 : 0);
      this.symbols.engine_player_apply_input(
          player.playerId, player.moveX, player.moveZ);
    }
  }

  public async playerSyncMarkSeen(playerGuid: string, currentTimeMs: bigint):
      Promise<void> {
    await this.ensureReady();
    this.symbols.engine_player_sync_mark_seen(playerGuid, currentTimeMs);
  }

  public async playerSyncUpdateStaleness(
      currentTimeMs: bigint, staleThresholdMs: bigint): Promise<void> {
    await this.ensureReady();
    this.symbols.engine_player_sync_update_staleness(
        currentTimeMs, staleThresholdMs);
  }

  public async playerSyncDeactivateStale(): Promise<void> {
    await this.ensureReady();
    this.symbols.engine_player_sync_deactivate_stale();
  }

  public async playerSyncCountActive(): Promise<number> {
    await this.ensureReady();
    return this.symbols.engine_player_sync_count_active();
  }

  public async tickSerialize(input: NativeEngineInput):
      Promise<NativeEngineSnapshot> {
    this.assertInput(input);
    await this.ensureReady();
    this.symbols.engine_set_move_input(input.moveX, input.moveZ);
    const raw = this.symbols.engine_tick_serialize(input.dtSeconds ?? 1 / 30);
    return parseSnapshot(normalizeCString(raw));
  }

  public async reset(): Promise<void> {
    if (this.initialized) {
      this.symbols.engine_shutdown();
      this.initialized = false;
    }

    await this.ensureReady();
  }

  public async shutdown(): Promise<void> {
    if (!this.initialized) return;
    this.symbols.engine_shutdown();
    this.initialized = false;
  }

  private async ensureReady(): Promise<void> {
    if (this.initialized) return;

    const statusCode = this.symbols.engine_init(1280, 720);
    if (statusCode !== BANANA_OK) {
      throw new Error(`engine_init failed with native status ${statusCode}`);
    }

    this.initialized = true;
  }

  private assertInput(input: NativeEngineInput): void {
    if (!assertAxis(input.moveX) || !assertAxis(input.moveZ)) {
      throw new Error('Native engine input axes must be discrete -1, 0, or 1.');
    }
  }
}

export class InMemoryEngineService implements NativeEngineService {
  private tick = 0;
  private x = 0;
  private z = 0;
  private readonly players = new Map < string, {
    id: number;
    name: string;
    controllerKind: 'human'|'ai';
    connected: boolean;
    x: number;
    z: number
  }
  >();
  private nextPlayerEntityId = 3;

  public async syncPlayers(players: readonly NativeEnginePlayerSync[]):
      Promise<void> {
    for (const player of players) {
      const existing = this.players.get(player.playerId);
      if (existing) {
        existing.name = player.playerName;
        existing.controllerKind = player.controllerKind;
        existing.connected = player.connected;
        if (player.controllerKind === 'human') {
          existing.x += player.moveX;
          existing.z += player.moveZ;
        } else {
          existing.x += Math.sin(this.tick / 15 + existing.id) * 0.05;
          existing.z += Math.cos(this.tick / 15 + existing.id) * 0.05;
        }
        continue;
      }

      this.players.set(player.playerId, {
        id: this.nextPlayerEntityId++,
        name: player.playerName,
        controllerKind: player.controllerKind,
        connected: player.connected,
        x: 0,
        z: 0,
      });
    }
  }

  public async tickSerialize(input: NativeEngineInput):
      Promise<NativeEngineSnapshot> {
    if (!assertAxis(input.moveX) || !assertAxis(input.moveZ)) {
      throw new Error(
          'In-memory engine input axes must be discrete -1, 0, or 1.');
    }

    this.tick += 1;
    this.x += input.moveX;
    this.z += input.moveZ;

    const playerEntities = Object.fromEntries(
        Array.from(this.players.entries())
            .map(
                ([playerId, player]) =>
                    [`entity-player-${playerId}`,
                        {
                          id: player.id,
                          type: 'player',
                          active: player.connected,
                          x: player.x,
                          y: 0,
                          z: player.z,
                          state: player.connected ? 1 : 0,
                          playerGuid: playerId,
                          controllerKind: player.controllerKind,
                        },
    ]));

    return {
      tick: this.tick,
      timestamp: Date.now(),
      isAuthoritative: true,
      entities: {
        ...playerEntities,
        'entity-1': {
          id: 1,
          type: 'player',
          active: true,
          x: this.x,
          y: 0,
          z: this.z,
          state: 1,
        },
        'entity-2': {
          id: 2,
          type: 'npc',
          active: true,
          x: 2,
          y: 0,
          z: -2,
          state: 1,
        },
      },
    };
  }

  public async playerSyncMarkSeen(playerGuid: string, currentTimeMs: bigint):
      Promise<void> {
    // In-memory implementation: no-op (staleness tracking not needed in mock)
  }

  public async playerSyncUpdateStaleness(
      currentTimeMs: bigint, staleThresholdMs: bigint): Promise<void> {
    // In-memory implementation: no-op
  }

  public async playerSyncDeactivateStale(): Promise<void> {
    // In-memory implementation: no-op
  }

  public async playerSyncCountActive(): Promise<number> {
    return Array.from(this.players.values())
        .filter((player) => player.connected)
        .length;
  }

  public async reset(): Promise<void> {
    this.tick = 0;
    this.x = 0;
    this.z = 0;
    this.players.clear();
    this.nextPlayerEntityId = 3;
  }

  public async shutdown(): Promise<void> {
    await this.reset();
  }
}

export function createDefaultNativeEngineService(): NativeEngineService {
  const adapterMode =
      (process.env.BANANA_ENGINE_ADAPTER ?? 'ffi').toLowerCase();
  const requireNative = process.env.BANANA_ENGINE_REQUIRE_NATIVE === 'true';

  if (adapterMode === 'inmemory') {
    return new InMemoryEngineService();
  }

  try {
    return new NativeFFIEngineService();
  } catch (error) {
    if (adapterMode === 'ffi-only' || requireNative) {
      throw error;
    }

    return new InMemoryEngineService();
  }
}
