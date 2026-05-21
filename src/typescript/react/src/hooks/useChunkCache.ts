/**
 * @file useChunkCache.ts
 * @brief React hook for LRU chunk caching with WebSocket integration
 *
 * Manages caching of deserialized terrain chunks with LRU eviction.
 * Automatically evicts least-recently-used chunks when cache reaches max size.
 */

import {useCallback, useEffect, useRef, useState} from 'react';

import {deserializeChunkPacket, DeserializedChunkPacket,} from '../lib/chunk-packet-deserializer';

export interface ChunkCacheOptions {
  readonly maxChunks?: number;  // Default 64 chunks (~1MB total)
  readonly apiBase?: string;    // Default process.env.VITE_BANANA_API_BASE_URL
}

interface CacheEntry {
  chunk: DeserializedChunkPacket;
  lastAccessMs: number;
  createdMs: number;
}

type ChunkKey = `${number},${number}`;  // "x,z" string key

/**
 * React hook for managing chunk cache
 *
 * @param options Cache configuration
 * @returns {chunks, getCacheKey, requestChunk, clear, stats}
 */
export function useChunkCache(options: ChunkCacheOptions = {}) {
  const maxChunks = options.maxChunks ?? 64;
  const apiBase = options.apiBase ??
      (import.meta.env.VITE_BANANA_API_BASE_URL as string) ??
      'http://localhost:8081';

  const cacheRef = useRef<Map<ChunkKey, CacheEntry>>(new Map());
  const [cacheStats, setCacheStats] =
      useState({hits: 0, misses: 0, evictions: 0});

  /**
   * Generate cache key from chunk coordinates
   */
  const getCacheKey =
      useCallback((chunkX: number, chunkZ: number): ChunkKey => {
        return `${chunkX},${chunkZ}` as ChunkKey;
      }, []);

  /**
   * Get chunk from cache (updates LRU timestamp)
   */
  const getCachedChunk = useCallback(
      (chunkX: number, chunkZ: number):
          DeserializedChunkPacket|null => {
            const key = getCacheKey(chunkX, chunkZ);
            const entry = cacheRef.current.get(key);
            if (!entry) {
              return null;
            }

            // Update LRU timestamp
            entry.lastAccessMs = Date.now();
            setCacheStats((prev) => ({...prev, hits: prev.hits + 1}));
            return entry.chunk;
          },
      [getCacheKey],
  );

  /**
   * Evict least-recently-used chunk
   */
  const evictLRU = useCallback(() => {
    const cache = cacheRef.current;
    if (cache.size === 0) return;

    // Find least recently used (oldest lastAccessMs)
    let lruKey: ChunkKey|null = null;
    let lruTime = Infinity;

    for (const [key, entry] of cache.entries()) {
      if (entry.lastAccessMs < lruTime) {
        lruTime = entry.lastAccessMs;
        lruKey = key;
      }
    }

    if (lruKey) {
      cache.delete(lruKey);
      setCacheStats((prev) => ({...prev, evictions: prev.evictions + 1}));
    }
  }, []);

  /**
   * Add chunk to cache, evicting LRU if necessary
   */
  const setCachedChunk = useCallback(
      (chunk: DeserializedChunkPacket) => {
        const cache = cacheRef.current;
        const key = getCacheKey(chunk.chunkX, chunk.chunkZ);

        // Evict if at capacity
        if (cache.size >= maxChunks && !cache.has(key)) {
          evictLRU();
        }

        cache.set(key, {
          chunk,
          lastAccessMs: Date.now(),
          createdMs: Date.now(),
        });
      },
      [getCacheKey, maxChunks, evictLRU],
  );

  /**
   * Request chunk from API
   * Returns cached chunk if available, fetches from API otherwise
   */
  const requestChunk = useCallback(
      async(chunkX: number, chunkZ: number):
          Promise<DeserializedChunkPacket> => {
            // Check cache first
            const cached = getCachedChunk(chunkX, chunkZ);
            if (cached) {
              return cached;
            }

            setCacheStats((prev) => ({...prev, misses: prev.misses + 1}));

            // Fetch from API
            const response =
                await fetch(`${apiBase}/api/world/chunk/${chunkX}/${chunkZ}`, {
                  method: 'GET',
                  headers: {
                    'Accept': 'application/octet-stream',
                  },
                });

            if (!response.ok) {
              throw new Error(`Chunk fetch failed: ${response.status} ${
                  response.statusText}`);
            }

            const buffer = await response.arrayBuffer();
            const chunk = deserializeChunkPacket(new Uint8Array(buffer));

            // Cache the deserialized chunk
            setCachedChunk(chunk);

            return chunk;
          },
      [getCachedChunk, setCachedChunk, apiBase],
  );

  /**
   * Clear entire cache
   */
  const clear = useCallback(() => {
    cacheRef.current.clear();
    setCacheStats({hits: 0, misses: 0, evictions: 0});
  }, []);

  /**
   * Get cache statistics
   */
  const getStats = useCallback(
      () => ({
        ...cacheStats,
        size: cacheRef.current.size,
        maxSize: maxChunks,
      }),
      [cacheStats, maxChunks],
  );

  return {
    requestChunk,
    getCachedChunk,
    clear,
    stats: getStats(),
  };
}

/**
 * Hook for efficient chunk requests within a specific view radius
 *
 * @param playerChunkX Player's chunk X coordinate
 * @param playerChunkZ Player's chunk Z coordinate
 * @param radius Chunk radius to request (e.g., radius=2 → 5x5 chunks centered
 *     on player)
 * @param chunkCache Result from useChunkCache hook
 * @returns Promise that resolves when all chunks in radius are loaded
 */
export function useChunkRadiusLoader(
    playerChunkX: number,
    playerChunkZ: number,
    radius: number,
    chunkCache: ReturnType<typeof useChunkCache>,
) {
  const [isLoading, setIsLoading] = useState(false);
  const [error, setError] = useState<Error|null>(null);

  const loadChunksInRadius = useCallback(async () => {
    setIsLoading(true);
    setError(null);

    try {
      const promises: Promise<DeserializedChunkPacket>[] = [];

      for (let dx = -radius; dx <= radius; dx++) {
        for (let dz = -radius; dz <= radius; dz++) {
          const chunkX = playerChunkX + dx;
          const chunkZ = playerChunkZ + dz;
          promises.push(chunkCache.requestChunk(chunkX, chunkZ));
        }
      }

      await Promise.all(promises);
    } catch (err) {
      setError(err instanceof Error ? err : new Error(String(err)));
    } finally {
      setIsLoading(false);
    }
  }, [playerChunkX, playerChunkZ, radius, chunkCache]);

  return {
    loadChunksInRadius,
    isLoading,
    error,
  };
}
