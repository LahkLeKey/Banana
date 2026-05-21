/**
 * @file chunk-packet-deserializer.test.ts
 * @brief Unit tests for binary chunk packet deserialization
 */

import {describe, expect, it} from 'bun:test';

import {deserializeChunkPacket, estimateChunkPacketSize} from './chunk-packet-deserializer';

describe('deserializeChunkPacket', () => {
  /**
   * Create a minimal valid chunk packet buffer for testing
   */
  function createTestPacket(
      chunkX: number,
      chunkZ: number,
      objectCount: number,
      ): Buffer {
    const size = estimateChunkPacketSize(objectCount);
    const buffer = Buffer.alloc(size);
    const view = new DataView(buffer.buffer);

    let offset = 0;

    // Header
    view.setInt32(offset, chunkX, true);
    offset += 4;
    view.setInt32(offset, chunkZ, true);
    offset += 4;
    view.setUint32(offset, 42, true);  // version
    offset += 4;
    view.setBigInt64(offset, BigInt(1000), true);  // generation_tick
    offset += 8;

    // Heights (all zeros)
    offset += 4096;

    // Biomes (all zeros)
    offset += 4096;

    // Objects
    view.setUint16(offset, objectCount, true);
    offset += 2;

    for (let i = 0; i < objectCount; i++) {
      view.setInt32(offset, i + 100, true);  // object_id
      offset += 4;
      view.setUint8(offset, i % 64);  // tile_x
      offset += 1;
      view.setUint8(offset, (i + 1) % 64);  // tile_z
      offset += 1;
      view.setUint8(offset, i % 3);  // resource_type
      offset += 1;
    }

    return buffer;
  }

  it('deserializes minimal packet (no objects)', () => {
    const packet = createTestPacket(5, 10, 0);
    const result = deserializeChunkPacket(packet);

    expect(result.chunkX).toBe(5);
    expect(result.chunkZ).toBe(10);
    expect(result.version).toBe(42);
    expect(result.generationTick).toBe(BigInt(1000));
    expect(result.objects.length).toBe(0);
  });

  it('deserializes packet with single object', () => {
    const packet = createTestPacket(0, 0, 1);
    const result = deserializeChunkPacket(packet);

    expect(result.objects.length).toBe(1);
    expect(result.objects[0].objectId).toBe(100);
    expect(result.objects[0].tileX).toBe(0);
    expect(result.objects[0].tileZ).toBe(1);
    expect(result.objects[0].resourceType).toBe(0);
  });

  it('deserializes packet with multiple objects', () => {
    const packet = createTestPacket(-5, 3, 5);
    const result = deserializeChunkPacket(packet);

    expect(result.chunkX).toBe(-5);
    expect(result.chunkZ).toBe(3);
    expect(result.objects.length).toBe(5);

    for (let i = 0; i < 5; i++) {
      expect(result.objects[i].objectId).toBe(i + 100);
      expect(result.objects[i].tileX).toBe(i % 64);
      expect(result.objects[i].tileZ).toBe((i + 1) % 64);
      expect(result.objects[i].resourceType).toBe(i % 3);
    }
  });

  it('deserializes max object count packet', () => {
    const packet = createTestPacket(0, 0, 256);
    const result = deserializeChunkPacket(packet);

    expect(result.objects.length).toBe(256);
    expect(result.objects[255].objectId).toBe(355);
  });

  it('throws on buffer too small', () => {
    const packet = Buffer.alloc(100);  // Way too small
    expect(() => deserializeChunkPacket(packet)).toThrow('Buffer too small');
  });

  it('throws on invalid object count', () => {
    const packet = Buffer.alloc(estimateChunkPacketSize(256));
    const view = new DataView(packet.buffer);
    // Set object count to 257 (invalid)
    view.setUint16(8212, 257, true);
    expect(() => deserializeChunkPacket(packet))
        .toThrow('Invalid object count');
  });

  it('preserves terrain arrays as separate references', () => {
    const packet = createTestPacket(1, 2, 0);
    const result = deserializeChunkPacket(packet);

    expect(result.heights.length).toBe(4096);
    expect(result.biomes.length).toBe(4096);
    expect(result.heights).not.toBe(result.biomes);
  });

  it('objects array is frozen', () => {
    const packet = createTestPacket(0, 0, 1);
    const result = deserializeChunkPacket(packet);

    expect(Object.isFrozen(result.objects)).toBe(true);
  });
});

describe('estimateChunkPacketSize', () => {
  it('estimates size for zero objects', () => {
    expect(estimateChunkPacketSize(0)).toBe(8214);
  });

  it('estimates size for single object', () => {
    expect(estimateChunkPacketSize(1)).toBe(8214 + 7);
  });

  it('estimates size for multiple objects', () => {
    expect(estimateChunkPacketSize(10)).toBe(8214 + 70);
  });

  it('estimates size for max objects', () => {
    expect(estimateChunkPacketSize(256)).toBe(8214 + 1792);
  });

  it('throws on invalid object count', () => {
    expect(() => estimateChunkPacketSize(-1)).toThrow('Invalid object count');
    expect(() => estimateChunkPacketSize(257)).toThrow('Invalid object count');
  });
});
