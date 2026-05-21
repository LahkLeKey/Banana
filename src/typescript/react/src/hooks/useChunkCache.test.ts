import '../test-setup';

import {act, cleanup, renderHook} from '@testing-library/react';
import {afterEach, describe, expect, it} from 'bun:test';

import {estimateChunkPacketSize} from '../lib/chunk-packet-deserializer';

import {useChunkCache} from './useChunkCache';

function createChunkPacket(chunkX: number, chunkZ: number): Uint8Array {
  const buffer = new ArrayBuffer(estimateChunkPacketSize(0));
  const view = new DataView(buffer);

  let offset = 0;
  view.setInt32(offset, chunkX, true);
  offset += 4;
  view.setInt32(offset, chunkZ, true);
  offset += 4;
  view.setUint32(offset, 1, true);
  offset += 4;
  view.setBigInt64(offset, 123n, true);
  offset += 8;

  offset += 4096;                   // heights
  offset += 4096;                   // biomes
  view.setUint16(offset, 0, true);  // no objects

  return new Uint8Array(buffer);
}

describe('useChunkCache', () => {
  const originalFetch = globalThis.fetch;

  afterEach(() => {
    globalThis.fetch = originalFetch;
    cleanup();
  });

  it('caches chunk responses and avoids duplicate fetches', async () => {
    let fetchCalls = 0;
    globalThis.fetch =
        (async () => {
          fetchCalls += 1;
          const packet = createChunkPacket(2, 3);
          return new Response(packet as unknown as BodyInit, {
            status: 200,
            headers: {'Content-Type': 'application/octet-stream'},
          });
        }) as typeof fetch;

    const {result} = renderHook(
        () => useChunkCache({
          apiBase: 'http://localhost:8081',
          maxChunks: 8,
        }),
    );

    let firstChunk;
    await act(async () => {
      firstChunk = await result.current.requestChunk(2, 3);
    });

    let secondChunk;
    await act(async () => {
      secondChunk = await result.current.requestChunk(2, 3);
    });

    expect(fetchCalls).toBe(1);
    expect(firstChunk).toBeDefined();
    expect(secondChunk).toBeDefined();
    expect(secondChunk).toBe(firstChunk);
    expect(result.current.stats.hits).toBe(1);
    expect(result.current.stats.misses).toBe(1);
  });

  it('evicts least-recently-used chunk when cache is full', async () => {
    let fetchCalls = 0;
    globalThis.fetch =
        (async (input: RequestInfo|URL) => {
          fetchCalls += 1;
          const url = typeof input === 'string' ? input : input.toString();
          const match = /\/chunk\/(-?\d+)\/(-?\d+)$/.exec(url);
          if (!match) {
            return new Response('bad url', {status: 400});
          }

          const chunkX = Number.parseInt(match[1], 10);
          const chunkZ = Number.parseInt(match[2], 10);
          const packet = createChunkPacket(chunkX, chunkZ);
          return new Response(packet as unknown as BodyInit, {
            status: 200,
            headers: {'Content-Type': 'application/octet-stream'},
          });
        }) as typeof fetch;

    const {result} = renderHook(
        () => useChunkCache({
          apiBase: 'http://localhost:8081',
          maxChunks: 2,
        }),
    );

    await act(async () => {
      await result.current.requestChunk(0, 0);
      await result.current.requestChunk(1, 0);
    });

    await act(async () => {
      await result.current.requestChunk(1, 0);
    });

    await act(async () => {
      await result.current.requestChunk(2, 0);
    });

    let chunk00;
    let chunk10;
    let chunk20;
    await act(async () => {
      chunk00 = result.current.getCachedChunk(0, 0);
      chunk10 = result.current.getCachedChunk(1, 0);
      chunk20 = result.current.getCachedChunk(2, 0);
    });

    expect(chunk00).toBeNull();
    expect(chunk10).not.toBeNull();
    expect(chunk20).not.toBeNull();
    expect(result.current.stats.evictions).toBe(1);
    expect(fetchCalls).toBe(3);
  });
});
