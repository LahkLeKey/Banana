import {dlopen, FFIType, type Pointer, ptr} from 'bun:ffi';

import {resolveBananaNativeLibraryCandidates,} from '../lib/native-library-candidates.ts';

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

export interface NativeEngineBuildStats {
  readonly health: number;
  readonly attack: number;
  readonly defense: number;
  readonly utility: number;
}

export interface NativeEngineComboResult {
  readonly triggered: boolean;
  readonly damageBonusPct: number;
  readonly mitigationBonusPct: number;
  readonly partySynergyBonusPct: number;
}

export interface NativeEngineService {
  syncPlayers(players: readonly NativeEnginePlayerSync[]): Promise<void>;
  setPlayerClass(playerId: string, classType: 0|1|2): Promise<void>;
  setPlayerAllocations(
      playerId: string, offensePoints: number, defensePoints: number,
      utilityPoints: number): Promise<void>;
  equipPlayerGear(
      playerId: string, slot: 0|1|2, tier: number, attackBonus: number,
      defenseBonus: number, utilityBonus: number): Promise<void>;
  getPlayerBuildStats(playerId: string): Promise<NativeEngineBuildStats>;
  evaluatePlayerCombo(
      playerId: string, firstSkill: string, secondSkill: string,
      elapsedMs: number, partySize: number): Promise<NativeEngineComboResult>;
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
  engine_player_build_set_class: (playerGuid: string, classType: number) =>
      number;
  engine_player_build_set_allocations:
      (playerGuid: string, offensePoints: number, defensePoints: number,
       utilityPoints: number) => number;
  engine_player_build_equip:
      (playerGuid: string, slot: number, tier: number, attackBonus: number,
       defenseBonus: number, utilityBonus: number) => number;
  engine_player_build_get_stat: (playerGuid: string, statName: string) =>
      number;
  engine_player_combo_evaluate:
      (playerGuid: string, firstSkill: string, secondSkill: string,
       elapsedMs: number, partySize: number, outDamageBonusPtr: Pointer,
       outMitigationBonusPtr: Pointer, outPartySynergyBonusPtr: Pointer) =>
          number;
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

function createNativeBinding(): NativeEngineSymbols {
  const candidates = resolveBananaNativeLibraryCandidates({
    fallbackDirs: [
      'out/native/bin',
      'out/v3-native/Debug',
      'out/v3-native/Release',
      'out/v3-native-baseline/Debug',
      'build/native/bin',
      'build/native',
    ],
  });
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
        engine_player_build_set_class: {
          args: [FFIType.cstring, FFIType.i32],
          returns: FFIType.i32,
        },
        engine_player_build_set_allocations: {
          args: [FFIType.cstring, FFIType.i32, FFIType.i32, FFIType.i32],
          returns: FFIType.i32,
        },
        engine_player_build_equip: {
          args: [
            FFIType.cstring,
            FFIType.i32,
            FFIType.i32,
            FFIType.i32,
            FFIType.i32,
            FFIType.i32,
          ],
          returns: FFIType.i32,
        },
        engine_player_build_get_stat: {
          args: [FFIType.cstring, FFIType.cstring],
          returns: FFIType.i32,
        },
        engine_player_combo_evaluate: {
          args: [
            FFIType.cstring,
            FFIType.cstring,
            FFIType.cstring,
            FFIType.i32,
            FFIType.i32,
            FFIType.ptr,
            FFIType.ptr,
            FFIType.ptr,
          ],
          returns: FFIType.i32,
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

      return library.symbols as unknown as NativeEngineSymbols;
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

  public async setPlayerClass(playerId: string, classType: 0|1|2):
      Promise<void> {
    await this.ensureReady();
    const rc = this.symbols.engine_player_build_set_class(playerId, classType);
    if (rc !== BANANA_OK) {
      throw new Error(`engine_player_build_set_class failed with status ${rc}`);
    }
  }

  public async setPlayerAllocations(
      playerId: string, offensePoints: number, defensePoints: number,
      utilityPoints: number): Promise<void> {
    await this.ensureReady();
    const rc = this.symbols.engine_player_build_set_allocations(
        playerId, offensePoints, defensePoints, utilityPoints);
    if (rc !== BANANA_OK) {
      throw new Error(
          `engine_player_build_set_allocations failed with status ${rc}`);
    }
  }

  public async equipPlayerGear(
      playerId: string, slot: 0|1|2, tier: number, attackBonus: number,
      defenseBonus: number, utilityBonus: number): Promise<void> {
    await this.ensureReady();
    const rc = this.symbols.engine_player_build_equip(
        playerId, slot, tier, attackBonus, defenseBonus, utilityBonus);
    if (rc !== BANANA_OK) {
      throw new Error(`engine_player_build_equip failed with status ${rc}`);
    }
  }

  public async getPlayerBuildStats(playerId: string):
      Promise<NativeEngineBuildStats> {
    await this.ensureReady();

    const health =
        this.symbols.engine_player_build_get_stat(playerId, 'health');
    const attack =
        this.symbols.engine_player_build_get_stat(playerId, 'attack');
    const defense =
        this.symbols.engine_player_build_get_stat(playerId, 'defense');
    const utility =
        this.symbols.engine_player_build_get_stat(playerId, 'utility');

    if (health < 0 || attack < 0 || defense < 0 || utility < 0) {
      throw new Error(
          'engine_player_build_get_stat failed for at least one stat');
    }

    return {health, attack, defense, utility};
  }

  public async evaluatePlayerCombo(
      playerId: string, firstSkill: string, secondSkill: string,
      elapsedMs: number, partySize: number): Promise<NativeEngineComboResult> {
    await this.ensureReady();

    const damage = Buffer.alloc(4);
    const mitigation = Buffer.alloc(4);
    const partySynergy = Buffer.alloc(4);
    const triggered = this.symbols.engine_player_combo_evaluate(
        playerId,
        firstSkill,
        secondSkill,
        elapsedMs,
        partySize,
        ptr(damage),
        ptr(mitigation),
        ptr(partySynergy),
    );
    if (triggered < 0) {
      throw new Error(
          `engine_player_combo_evaluate failed with status ${triggered}`);
    }

    return {
      triggered: triggered === 1,
      damageBonusPct: damage.readInt32LE(0),
      mitigationBonusPct: mitigation.readInt32LE(0),
      partySynergyBonusPct: partySynergy.readInt32LE(0),
    };
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
  private readonly buildStats = new Map<string, NativeEngineBuildStats>();
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

      if (!this.buildStats.has(player.playerId)) {
        this.buildStats.set(player.playerId, {
          health: 120,
          attack: 18,
          defense: 16,
          utility: 8,
        });
      }
    }
  }

  public async setPlayerClass(playerId: string, classType: 0|1|2):
      Promise<void> {
    const defaults: Record<0|1|2, NativeEngineBuildStats> = {
      0: {health: 120, attack: 18, defense: 16, utility: 8},
      1: {health: 90, attack: 24, defense: 8, utility: 18},
      2: {health: 100, attack: 20, defense: 12, utility: 14},
    };
    this.buildStats.set(playerId, defaults[classType]);
  }

  public async setPlayerAllocations(
      playerId: string, offensePoints: number, defensePoints: number,
      utilityPoints: number): Promise<void> {
    const base = this.buildStats.get(playerId) ??
        {health: 120, attack: 18, defense: 16, utility: 8};
    this.buildStats.set(playerId, {
      health: base.health + defensePoints * 3,
      attack: base.attack + offensePoints * 2,
      defense: base.defense + defensePoints * 2,
      utility: base.utility + utilityPoints * 2,
    });
  }

  public async equipPlayerGear(
      playerId: string, slot: 0|1|2, tier: number, attackBonus: number,
      defenseBonus: number, utilityBonus: number): Promise<void> {
    const base = this.buildStats.get(playerId) ??
        {health: 120, attack: 18, defense: 16, utility: 8};
    this.buildStats.set(playerId, {
      ...base,
      attack: base.attack + attackBonus,
      defense: base.defense + defenseBonus,
      utility: base.utility + utilityBonus,
    });
  }

  public async getPlayerBuildStats(playerId: string):
      Promise<NativeEngineBuildStats> {
    return this.buildStats.get(playerId) ??
        {health: 120, attack: 18, defense: 16, utility: 8};
  }

  public async evaluatePlayerCombo(
      playerId: string, firstSkill: string, secondSkill: string,
      elapsedMs: number, partySize: number): Promise<NativeEngineComboResult> {
    const triggered = elapsedMs > 0 && elapsedMs <= 1500;
    return {
      triggered,
      damageBonusPct: triggered ? (partySize > 1 ? 16 : 22) : 0,
      mitigationBonusPct: triggered ? 8 : 0,
      partySynergyBonusPct: triggered && partySize > 1 ? 10 : 0,
    };
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
    this.buildStats.clear();
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
