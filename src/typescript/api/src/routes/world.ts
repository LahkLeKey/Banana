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

/**
 * ChunkStreamPacket binary format (from C):
 * - Header (20 bytes): chunk_x (i32), chunk_z (i32), version (u32),
 * generation_tick (i64)
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

class InMemoryWorldService implements WorldService {
  public async getChunkBinary(chunkX: number, chunkZ: number): Promise<Buffer> {
    const objectCount = 0;
    const totalSize = 8214 + objectCount * 7;
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

export async function registerWorldRoutes(app: FastifyInstance): Promise<void> {
  let worldService: WorldService;
  try {
    worldService = new NativeWorldService();
  } catch (error) {
    app.log.warn(
        {err: error},
        'Native world FFI unavailable, falling back to in-memory world service');
    worldService = new InMemoryWorldService();
  }

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
}
