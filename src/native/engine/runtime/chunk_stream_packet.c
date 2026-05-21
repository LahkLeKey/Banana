/**
 * @file chunk_stream_packet.c
 * @brief Implementation of chunk stream packet serialization
 */

#include "chunk_stream_packet.h"
#include <string.h>

int chunk_stream_packet_create(const TerrainChunk *chunk, int chunk_x, int chunk_z,
                               ChunkStreamPacket *out_packet)
{
    if (!chunk || !out_packet)
    {
        return -1;
    }

    memset(out_packet, 0, sizeof(ChunkStreamPacket));

    out_packet->chunk_x = chunk_x;
    out_packet->chunk_z = chunk_z;
    out_packet->version = chunk->version;
    out_packet->generation_tick = chunk->generation_tick;

    /* Copy height and biome data */
    memcpy(out_packet->heights, chunk->heights, sizeof(chunk->heights));
    memcpy(out_packet->biomes, chunk->biomes, sizeof(chunk->biomes));

    /* Object list will be populated separately via chunk_stream_packet_add_object */
    out_packet->object_count = 0;

    return 0;
}

int chunk_stream_packet_serialize(const ChunkStreamPacket *packet, uint8_t *buffer,
                                  size_t buffer_size)
{
    if (!packet || !buffer)
    {
        return -1;
    }

    size_t required_size = chunk_stream_packet_estimate_size(packet->object_count);
    if (buffer_size < required_size)
    {
        return -1;
    }

    size_t offset = 0;

    /* Header */
    memcpy(buffer + offset, &packet->chunk_x, sizeof(int32_t));
    offset += sizeof(int32_t);
    memcpy(buffer + offset, &packet->chunk_z, sizeof(int32_t));
    offset += sizeof(int32_t);
    memcpy(buffer + offset, &packet->version, sizeof(uint32_t));
    offset += sizeof(uint32_t);
    memcpy(buffer + offset, &packet->generation_tick, sizeof(int64_t));
    offset += sizeof(int64_t);

    /* Terrain data (4096 bytes each) */
    memcpy(buffer + offset, packet->heights, sizeof(packet->heights));
    offset += sizeof(packet->heights);
    memcpy(buffer + offset, packet->biomes, sizeof(packet->biomes));
    offset += sizeof(packet->biomes);

    /* Object count */
    memcpy(buffer + offset, &packet->object_count, sizeof(uint16_t));
    offset += sizeof(uint16_t);

    /* Object entries */
    for (int i = 0; i < packet->object_count; i++)
    {
        const ChunkStreamObjectEntry *obj = &packet->objects[i];
        memcpy(buffer + offset, &obj->object_id, sizeof(int32_t));
        offset += sizeof(int32_t);
        memcpy(buffer + offset, &obj->tile_x, sizeof(uint8_t));
        offset += sizeof(uint8_t);
        memcpy(buffer + offset, &obj->tile_z, sizeof(uint8_t));
        offset += sizeof(uint8_t);
        memcpy(buffer + offset, &obj->resource_type, sizeof(uint8_t));
        offset += sizeof(uint8_t);
    }

    return (int)offset;
}

int chunk_stream_packet_deserialize(const uint8_t *buffer, size_t buffer_size,
                                    ChunkStreamPacket *out_packet)
{
    if (!buffer || !out_packet || buffer_size < 24) /* Minimum header size */
    {
        return -1;
    }

    size_t offset = 0;

    /* Header */
    memcpy(&out_packet->chunk_x, buffer + offset, sizeof(int32_t));
    offset += sizeof(int32_t);
    memcpy(&out_packet->chunk_z, buffer + offset, sizeof(int32_t));
    offset += sizeof(int32_t);
    memcpy(&out_packet->version, buffer + offset, sizeof(uint32_t));
    offset += sizeof(uint32_t);
    memcpy(&out_packet->generation_tick, buffer + offset, sizeof(int64_t));
    offset += sizeof(int64_t);

    /* Check we have terrain data */
    if (buffer_size < offset + 8192) /* 4096 + 4096 */
    {
        return -1;
    }

    /* Terrain data */
    memcpy(out_packet->heights, buffer + offset, sizeof(out_packet->heights));
    offset += sizeof(out_packet->heights);
    memcpy(out_packet->biomes, buffer + offset, sizeof(out_packet->biomes));
    offset += sizeof(out_packet->biomes);

    /* Object count */
    if (buffer_size < offset + sizeof(uint16_t))
    {
        return -1;
    }

    memcpy(&out_packet->object_count, buffer + offset, sizeof(uint16_t));
    offset += sizeof(uint16_t);

    if (out_packet->object_count > CHUNK_STREAM_MAX_OBJECTS)
    {
        return -1;
    }

    /* Object entries - each is exactly 7 bytes */
    for (int i = 0; i < out_packet->object_count; i++)
    {
        if (buffer_size < offset + 7)
        {
            return -1;
        }

        ChunkStreamObjectEntry *obj = &out_packet->objects[i];
        memcpy(&obj->object_id, buffer + offset, sizeof(int32_t));
        offset += sizeof(int32_t);
        memcpy(&obj->tile_x, buffer + offset, sizeof(uint8_t));
        offset += sizeof(uint8_t);
        memcpy(&obj->tile_z, buffer + offset, sizeof(uint8_t));
        offset += sizeof(uint8_t);
        memcpy(&obj->resource_type, buffer + offset, sizeof(uint8_t));
        offset += sizeof(uint8_t);
    }

    return (int)offset;
}

size_t chunk_stream_packet_estimate_size(int object_count)
{
    /* Header: 4 + 4 + 4 + 8 = 20 bytes */
    size_t size = 20;
    /* Heights: 64x64 = 4096 bytes */
    size += 4096;
    /* Biomes: 64x64 = 4096 bytes */
    size += 4096;
    /* Object count: 2 bytes */
    size += 2;
    /* Objects: each 7 bytes (4 + 1 + 1 + 1) */
    size += (size_t)object_count * 7;

    return size;
}
