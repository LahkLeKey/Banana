/**
 * @file chunk-packet-deserializer.ts
 * @brief Deserialize binary ChunkStreamPacket into TypeScript structures
 *
 * Binary format (all little-endian):
 * - Header (20 bytes): chunk_x (i32), chunk_z (i32), version (u32),
 * generation_tick (i64)
 * - Terrain (8192 bytes): heights[64][64] (u8) + biomes[64][64] (u8)
 * - Objects (2+N*7 bytes): count (u16), then N objects with {id (i32), tile_x
 * (u8), tile_z (u8), resource_type (u8)}
 */

export interface DeserializedChunkPacket {
  readonly chunkX: number;
  readonly chunkZ: number;
  readonly version: number;
  readonly generationTick: bigint;
  readonly heights: Uint8Array;  // 64x64 flattened
  readonly biomes: Uint8Array;   // 64x64 flattened
  readonly objects: ReadonlyArray<{
    readonly objectId: number; readonly tileX: number; readonly tileZ: number; readonly resourceType:
                                                                                            number;
  }>;
}

/**
 * Deserialize binary chunk packet buffer
 * @param buffer Raw binary buffer from API
 * @returns Parsed chunk data or throws on invalid format
 */
export function deserializeChunkPacket(buffer: Buffer|Uint8Array):
    DeserializedChunkPacket {
  const backingBuffer = buffer.buffer;
  const backingOffset = buffer.byteOffset;
  const backingLength = buffer.byteLength;
  const view = new DataView(backingBuffer, backingOffset, backingLength);

  if (buffer.length < 8214) {
    throw new Error(
        `Buffer too small: expected at least 8214 bytes, got ${buffer.length}`);
  }

  let offset = 0;

  // Parse header (20 bytes)
  const chunkX = view.getInt32(offset, true);
  offset += 4;
  const chunkZ = view.getInt32(offset, true);
  offset += 4;
  const version = view.getUint32(offset, true);
  offset += 4;
  const generationTick = view.getBigInt64(offset, true);
  offset += 8;

  // Parse terrain: heights[64][64] (4096 bytes)
  const heights = new Uint8Array(backingBuffer, backingOffset + offset, 4096);

  offset += 4096;

  // Parse terrain: biomes[64][64] (4096 bytes)
  const biomes = new Uint8Array(backingBuffer, backingOffset + offset, 4096);
  offset += 4096;

  // Parse objects
  const objectCount = view.getUint16(offset, true);
  offset += 2;

  if (objectCount > 256) {
    throw new Error(`Invalid object count: ${objectCount} (max 256)`);
  }

  const expectedSize = 8214 + objectCount * 7;
  if (buffer.length < expectedSize) {
    throw new Error(`Buffer too small for objects: expected at least ${
        expectedSize}, got ${buffer.length}`);
  }

  const objects: Array<{
    readonly objectId: number; readonly tileX: number; readonly tileZ: number; readonly resourceType:
                                                                                            number;
  }> = [];

  for (let i = 0; i < objectCount; i++) {
    const objectId = view.getInt32(offset, true);
    offset += 4;
    const tileX = view.getUint8(offset);
    offset += 1;
    const tileZ = view.getUint8(offset);
    offset += 1;
    const resourceType = view.getUint8(offset);
    offset += 1;

    objects.push({objectId, tileX, tileZ, resourceType});
  }

  return {
    chunkX,
    chunkZ,
    version,
    generationTick,
    heights,
    biomes,
    objects: Object.freeze(objects),
  };
}

/**
 * Estimate serialized size for a given object count
 * @param objectCount Number of spawned objects in chunk
 * @returns Expected buffer size in bytes
 */
export function estimateChunkPacketSize(objectCount: number): number {
  if (objectCount < 0 || objectCount > 256) {
    throw new Error(`Invalid object count: ${objectCount}`);
  }
  return 8214 + objectCount * 7;
}
