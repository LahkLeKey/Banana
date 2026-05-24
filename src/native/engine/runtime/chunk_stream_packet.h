/**
 * @file chunk_stream_packet.h
 * @brief Serialize and deserialize terrain chunks for network transmission
 *
 * Chunks are transmitted as binary packets with:
 * - Header: chunk coordinates, version, timestamp
 * - Heightfield: 64x64 elevation data (u8)
 * - Biome map: 64x64 biome IDs (u8)
 * - Object manifest: list of spawned objects at this chunk
 *
 * Packet format is optimized for bandwidth on low-latency networks.
 */

#ifndef BANANA_ENGINE_CHUNK_STREAM_PACKET_H
#define BANANA_ENGINE_CHUNK_STREAM_PACKET_H

#include "terrain/terrain_chunks.h"
#include <stdint.h>
#include <stddef.h>

#ifdef __cplusplus
extern "C"
{
#endif

#define CHUNK_STREAM_MAX_OBJECTS 256

    /**
     * Spawned object entry in chunk stream packet
     */
    typedef struct
    {
        int object_id;
        uint8_t tile_x;
        uint8_t tile_z;
        uint8_t resource_type; /* ResourceType enum value */
    } ChunkStreamObjectEntry;

    /**
     * Chunk stream packet ready for transmission
     * Binary format: all fields are packed, no padding
     */
    typedef struct
    {
        /* Header */
        int32_t chunk_x;
        int32_t chunk_z;
        uint32_t version;
        int64_t generation_tick;

        /* Terrain data */
        uint8_t heights[64][64];
        uint8_t biomes[64][64];

        /* Spawned objects */
        uint16_t object_count;
        ChunkStreamObjectEntry objects[CHUNK_STREAM_MAX_OBJECTS];
    } ChunkStreamPacket;

    /**
     * Create a packet from a chunk and its spawned objects
     * @param chunk Terrain chunk data
     * @param chunk_x Chunk coordinate X
     * @param chunk_z Chunk coordinate Z
     * @param out_packet Output packet buffer
     * @return Number of bytes written, or -1 on error
     */
    int chunk_stream_packet_create(const TerrainChunk *chunk, int chunk_x, int chunk_z,
                                   ChunkStreamPacket *out_packet);

    /**
     * Serialize packet to binary buffer
     * @param packet Packet to serialize
     * @param buffer Output buffer
     * @param buffer_size Buffer capacity
     * @return Number of bytes written, or -1 if buffer too small
     */
    int chunk_stream_packet_serialize(const ChunkStreamPacket *packet, uint8_t *buffer,
                                      size_t buffer_size);

    /**
     * Deserialize binary buffer to packet
     * @param buffer Binary data
     * @param buffer_size Data size
     * @param out_packet Output packet
     * @return Number of bytes consumed, or -1 on error
     */
    int chunk_stream_packet_deserialize(const uint8_t *buffer, size_t buffer_size,
                                        ChunkStreamPacket *out_packet);

    /**
     * Get estimated packet size for a chunk with object count
     * @param object_count Number of spawned objects
     * @return Estimated bytes needed
     */
    size_t chunk_stream_packet_estimate_size(int object_count);

#ifdef __cplusplus
}
#endif

#endif /* BANANA_ENGINE_CHUNK_STREAM_PACKET_H */
