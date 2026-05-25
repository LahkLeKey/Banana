/**
 * @file world.ts
 * @brief API routes for world/terrain data (chunks, resources, spawned objects)
 *
 * Routes:
 * - GET /api/world/chunk/:x/:z - Stream binary ChunkStreamPacket for terrain
 * chunk
 */

import {dlopen, FFIType, type Pointer, ptr, suffix} from 'bun:ffi';
import type {FastifyInstance, FastifyReply, FastifyRequest} from 'fastify';
import path from 'node:path';

import {bootstrapPersistentWorldOrchestrationDomain, type PersistentWorldOrchestrationDomain,} from '../domains/persistent-world-orchestration/index.ts';
import {persistentWorldRevisitBaselineUnavailable,} from '../lib/errors/domainErrors.ts';

/**
 * ChunkStreamPacket binary format (from C):
 * - Header (28 bytes): chunk_x (i32), chunk_z (i32), version (u32),
 * generation_tick (i64), generation_fingerprint (u64)
 * - Heights (4096 bytes): 64x64 uint8_t array
 * - Biomes (4096 bytes): 64x64 uint8_t array
 * - Objects (2 + N*7 bytes): count (u16), then N entries of {id (i32), tile_x
 * (u8), tile_z (u8), resource_type (u8)}
 */

export interface ChunkStreamRequest {
  readonly x: number;
  readonly z: number;
}

export interface WorldService {
  getChunkBinary(chunkX: number, chunkZ: number): Promise<Buffer>;
}

type RevisitCoordinates = {
  worldId: string; laneId: string; chunkX: number; chunkY: number;
  partitionEpoch: number;
};

type WorldRouteOptions = {
  persistentWorldOrchestrationDomain?: PersistentWorldOrchestrationDomain;
};

class InMemoryWorldService implements WorldService {
  public async getChunkBinary(chunkX: number, chunkZ: number): Promise<Buffer> {
    const objectCount = 0;
    const totalSize = 8222 + objectCount * 7;
    const buffer = Buffer.alloc(totalSize);
    const view =
        new DataView(buffer.buffer, buffer.byteOffset, buffer.byteLength);

    let offset = 0;
    view.setInt32(offset, chunkX, true);
    offset += 4;
    view.setInt32(offset, chunkZ, true);
    offset += 4;
    view.setUint32(offset, 1, true);
    offset += 4;
    view.setBigInt64(offset, 0n, true);
    offset += 8;
    view.setBigUint64(offset, 0n, true);
    offset += 8;

    for (let z = 0; z < 64; z++) {
      for (let x = 0; x < 64; x++) {
        const idx = z * 64 + x;
        const h =
            (Math.abs(chunkX * 17 + chunkZ * 31 + x * 7 + z * 13) % 120) + 40;
        buffer[offset + idx] = h;
      }
    }
    offset += 4096;

    for (let z = 0; z < 64; z++) {
      for (let x = 0; x < 64; x++) {
        const idx = z * 64 + x;
        const biome = Math.abs(chunkX + chunkZ + x + z) % 5;
        buffer[offset + idx] = biome;
      }
    }
    offset += 4096;

    view.setUint16(offset, objectCount, true);
    return buffer;
  }
}

type WorldSymbols = {
  banana_native_v3_world_init: (seed: number, cache_size: number) => number;
  banana_native_v3_world_chunk_estimate_size: (object_count: number) => number;
  banana_native_v3_world_chunk_serialize:
      (chunk_x: number, chunk_z: number, buffer: Pointer, buffer_len: number) =>
          number;
};

function resolveWorldLibraryCandidates(): string[] {
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

function createWorldBinding(): WorldSymbols {
  const candidates = resolveWorldLibraryCandidates();
  let lastError: unknown = null;

  for (const candidate of candidates) {
    try {
      const library = dlopen(candidate, {
        banana_native_v3_world_init: {
          args: [FFIType.u32, FFIType.i32],
          returns: FFIType.i32,
        },
        banana_native_v3_world_chunk_estimate_size: {
          args: [FFIType.i32],
          returns: FFIType.i32,
        },
        banana_native_v3_world_chunk_serialize: {
          args: [FFIType.i32, FFIType.i32, FFIType.ptr, FFIType.i32],
          returns: FFIType.i32,
        },
      });

      return library.symbols as unknown as WorldSymbols;
    } catch (error) {
      lastError = error;
    }
  }

  throw new Error(
      `Unable to load Banana native library for world FFI. Candidates: ${
          candidates.join(', ')}. Last error: ${String(lastError)}`);
}

export class NativeWorldService implements WorldService {
  private readonly symbols = createWorldBinding();
  private initialized = false;

  public async getChunkBinary(chunkX: number, chunkZ: number): Promise<Buffer> {
    try {
      if (!this.initialized) {
        const initCode = this.symbols.banana_native_v3_world_init(1337, 512);
        if (initCode !== 0) {
          throw new Error(`World init failed: error code ${initCode}`);
        }
        this.initialized = true;
      }

      const estimatedSerializeSize =
          this.symbols.banana_native_v3_world_chunk_estimate_size(256);
      if (estimatedSerializeSize <= 0) {
        throw new Error(
            `World estimate size failed: error code ${estimatedSerializeSize}`);
      }

      const serializeBuffer = Buffer.alloc(estimatedSerializeSize);
      const serializeSize = this.symbols.banana_native_v3_world_chunk_serialize(
          chunkX,
          chunkZ,
          ptr(serializeBuffer),
          serializeBuffer.length,
      );

      if (serializeSize < 0) {
        throw new Error(
            `Failed to serialize chunk packet: error code ${serializeSize}`);
      }

      return serializeBuffer.slice(0, serializeSize);
    } catch (error) {
      throw new Error(`World service error fetching chunk (${chunkX}, ${
          chunkZ}): ${String(error)}`);
    }
  }
}

export async function registerWorldRoutes(
    app: FastifyInstance,
    options: WorldRouteOptions = {},
    ): Promise<void> {
  let worldService: WorldService;
  try {
    worldService = new NativeWorldService();
  } catch (error) {
    app.log.warn(
        {err: error},
        'Native world FFI unavailable, falling back to in-memory world service');
    worldService = new InMemoryWorldService();
  }

  const persistentWorldDomain = options.persistentWorldOrchestrationDomain ??
      bootstrapPersistentWorldOrchestrationDomain(app);

  /**
   * GET /api/world/chunk/:x/:z
   * Returns binary ChunkStreamPacket for the requested chunk coordinates
   * Response: application/octet-stream, 8KB-15KB depending on object count
   */
  app.get<{Params: {x: string; z: string}}>(
      '/api/world/chunk/:x/:z',
      async (
          request: FastifyRequest<{Params: {x: string; z: string}}>,
          reply: FastifyReply,
          ) => {
        const chunkX = parseInt(request.params.x, 10);
        const chunkZ = parseInt(request.params.z, 10);

        if (isNaN(chunkX) || isNaN(chunkZ)) {
          return reply.status(400).send({
            error: 'Invalid chunk coordinates',
            message: 'Coordinates must be valid integers',
          });
        }

        try {
          const chunkData = await worldService.getChunkBinary(chunkX, chunkZ);

          reply.type('application/octet-stream');
          reply.header('Content-Length', chunkData.length);
          reply.header('X-Chunk-X', String(chunkX));
          reply.header('X-Chunk-Z', String(chunkZ));

          return reply.send(chunkData);
        } catch (error) {
          app.log.error({err: error, chunkX, chunkZ}, 'Failed to fetch chunk');
          return reply.status(500).send({
            error: 'Chunk fetch failed',
            message: String(error),
          });
        }
      },
  );

  app.post<{
    Body: {
      worldId: string; laneId: string; chunkX: number; chunkY: number;
      partitionEpoch: number;
    }
  }>(
      '/api/world/chunk/replay',
      async (
          request: FastifyRequest<{
            Body: {
              worldId: string; laneId: string; chunkX: number; chunkY: number;
              partitionEpoch: number;
            }
          }>,
          reply: FastifyReply,
          ) => {
        const baseline = persistentWorldDomain.revisitBaselineService
                             .getAuthoritativeBaseline(request.body);
        const authoritative = persistentWorldDomain.replayMergeService
                                  .ensureAuthoritativeBaseline(baseline);
        let chunkData: Buffer;

        try {
          chunkData = await worldService.getChunkBinary(
              baseline.areaIdentity.chunkX,
              baseline.areaIdentity.chunkY,
          );
        } catch (error) {
          app.log.error({err: error}, 'Failed to replay chunk baseline');
          throw persistentWorldRevisitBaselineUnavailable({
            cause: error instanceof Error ? error.message : String(error),
            chunkX: baseline.areaIdentity.chunkX,
            chunkY: baseline.areaIdentity.chunkY,
          });
        }

        return reply.status(200).send({
          areaIdentity: authoritative.areaIdentity,
          baselineId: authoritative.baselineId,
          areaStateVersion: authoritative.areaStateVersion,
          orchestrationPathId:
              String(authoritative.baselinePayload.orchestrationPathId ?? ''),
          chunk: {
            chunkX: authoritative.areaIdentity.chunkX,
            chunkY: authoritative.areaIdentity.chunkY,
            sizeBytes: chunkData.byteLength,
          },
        });
      },
  );

  app.post<{
    Body: RevisitCoordinates &
        {
          baseAreaStateVersion: number;
          idempotencyKey: string;
          deltaPayload: Record<string, unknown>;
        }
  }>(
      '/api/world/state/delta',
      async (
          request: FastifyRequest<{
            Body: RevisitCoordinates &
                {
                  baseAreaStateVersion: number;
                  idempotencyKey: string;
                  deltaPayload: Record<string, unknown>;
                }
          }>,
          reply: FastifyReply,
          ) => {
        const {baseAreaStateVersion, idempotencyKey, deltaPayload, ...coords} =
            request.body;

        const baseline = persistentWorldDomain.revisitBaselineService
                             .getAuthoritativeBaseline(coords);
        persistentWorldDomain.replayMergeService.ensureAuthoritativeBaseline(
            baseline);
        const merged = persistentWorldDomain.replayMergeService.applyDelta({
          areaIdentity: baseline.areaIdentity,
          baselineId: baseline.baselineId,
          baseAreaStateVersion,
          idempotencyKey,
          deltaPayload,
        });

        return reply.status(200).send({
          areaIdentity: merged.areaIdentity,
          baselineId: merged.baselineId,
          areaStateVersion: {
            areaId: merged.areaIdentity.areaId,
            areaStateVersion: merged.areaStateVersion,
            appliedDeltaThroughSequence: merged.appliedDeltaThroughSequence,
            canonicalStateSignature: merged.canonicalStateSignature,
          },
        });
      },
  );

  app.get<{Params: {areaId: string}}>(
      '/api/world/state/:areaId',
      async (
          request: FastifyRequest<{Params: {areaId: string}}>,
          reply: FastifyReply,
          ) => {
        const replayed = persistentWorldDomain.replayMergeService.replay(
            request.params.areaId);
        if (!replayed) {
          return reply.status(404).send({
            error: 'not_found',
            message: 'authoritative area state not found',
          });
        }

        return reply.status(200).send({
          areaIdentity: replayed.areaIdentity,
          baselineId: replayed.baselineId,
          areaStateVersion: {
            areaId: replayed.areaIdentity.areaId,
            areaStateVersion: replayed.areaStateVersion,
            appliedDeltaThroughSequence: replayed.appliedDeltaThroughSequence,
            canonicalStateSignature: replayed.canonicalStateSignature,
          },
        });
      },
  );

  app.post<{
    Body: {
      generationInputFingerprint: string | number; retryAttempt: number;
      lastFailureCode: number;
    }
  }>(
      '/api/world/retry/orchestrate',
      async (
          request: FastifyRequest<{
            Body: {
              generationInputFingerprint: string | number; retryAttempt: number;
              lastFailureCode: number;
            }
          }>,
          reply: FastifyReply,
          ) => {
        const decision = persistentWorldDomain.retryOrchestrationService.decide(
            request.body);
        const operatorContext =
            persistentWorldDomain.failureContextService.build(
                request.body.lastFailureCode,
                decision.classification,
            );

        return reply.status(200).send({
          classification: decision.classification,
          shouldRetry: decision.shouldRetry,
          retryFingerprint: decision.retryFingerprint,
          maxRetryAttempts: decision.maxRetryAttempts,
          operatorContext,
        });
      },
  );

  app.post<{
    Body: {
      baselineAreaCount: number; deltaRecordCount: number;
      progressionLedgerEntryCount: number;
      averageBaselineMetadataBytes?: number;
      averageDeltaRecordBytes?: number;
      averageProgressionLedgerBytes?: number;
    }
  }>(
      '/api/world/storage/diagnostics',
      async (
          request: FastifyRequest<{
            Body: {
              baselineAreaCount: number; deltaRecordCount: number;
              progressionLedgerEntryCount: number;
              averageBaselineMetadataBytes?: number;
              averageDeltaRecordBytes?: number;
              averageProgressionLedgerBytes?: number;
            }
          }>,
          reply: FastifyReply,
          ) => {
        const diagnostics =
            persistentWorldDomain.storageBudgetService.simulate(request.body);

        return reply.status(200).send(diagnostics);
      },
  );

  app.post<
      {Body: {nativeGeneratorVersion: string; clientProfileVersion: string;}}>(
      '/api/world/version/diagnostics',
      async (
          request: FastifyRequest<{
            Body:
                {nativeGeneratorVersion: string; clientProfileVersion: string;}
          }>,
          reply: FastifyReply,
          ) => {
        const report = persistentWorldDomain.versionDriftService.check({
          nativeGeneratorVersion: request.body.nativeGeneratorVersion,
          apiContractVersion: persistentWorldDomain.contractVersion,
          clientProfileVersion: request.body.clientProfileVersion,
        });

        return reply.status(200).send(report);
      },
  );
}
