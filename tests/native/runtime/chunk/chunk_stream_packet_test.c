#include "runtime/chunk/chunk_stream_packet.h"
#include "runtime/support/test_support.h"

#include <stdint.h>
#include <string.h>

static TerrainChunk make_chunk(void)
{
    TerrainChunk chunk;

    memset(&chunk, 0, sizeof(chunk));
    chunk.version = 3;
    chunk.generation_tick = 77;
    chunk.generation_fingerprint = 0x1234u;
    chunk.heights[0][0] = 5;
    chunk.heights[63][63] = 201;
    chunk.biomes[0][0] = 2;
    chunk.biomes[63][63] = 4;

    return chunk;
}

static ChunkStreamPacket make_packet_with_objects(void)
{
    ChunkStreamPacket packet;

    memset(&packet, 0, sizeof(packet));
    packet.chunk_x = 4;
    packet.chunk_z = -3;
    packet.version = 9;
    packet.generation_tick = 808;
    packet.generation_fingerprint = 0x55667788u;
    packet.heights[0][0] = 17;
    packet.biomes[0][1] = 6;

    packet.object_count = 2;
    packet.objects[0].object_id = 101;
    packet.objects[0].tile_x = 7;
    packet.objects[0].tile_z = 8;
    packet.objects[0].resource_type = 2;

    packet.objects[1].object_id = 202;
    packet.objects[1].tile_x = 9;
    packet.objects[1].tile_z = 10;
    packet.objects[1].resource_type = 3;

    return packet;
}

static void test_chunk_stream_packet_create_guards_and_success(void **state)
{
    TerrainChunk chunk;
    ChunkStreamPacket packet;

    (void)state;

    chunk = make_chunk();
    memset(&packet, 0, sizeof(packet));

    BANANA_TEST_ASSERT_INT_EQ(chunk_stream_packet_create(NULL, 1, 2, &packet),
                              -1,
                              "chunk packet create must reject null chunk");
    BANANA_TEST_ASSERT_INT_EQ(chunk_stream_packet_create(&chunk, 1, 2, NULL),
                              -1,
                              "chunk packet create must reject null packet output");

    BANANA_TEST_ASSERT_INT_EQ(chunk_stream_packet_create(&chunk, 11, 22, &packet),
                              0,
                              "chunk packet create must succeed for valid input");
    BANANA_TEST_ASSERT_INT_EQ(packet.chunk_x, 11,
                              "chunk packet create must copy chunk x");
    BANANA_TEST_ASSERT_INT_EQ(packet.chunk_z, 22,
                              "chunk packet create must copy chunk z");
    BANANA_TEST_ASSERT_INT_EQ(packet.version, chunk.version,
                              "chunk packet create must copy version");
    BANANA_TEST_ASSERT_INT_EQ(packet.object_count, 0,
                              "chunk packet create must reset object_count");
    BANANA_TEST_ASSERT_INT_EQ(packet.heights[0][0], chunk.heights[0][0],
                              "chunk packet create must copy heights");
    BANANA_TEST_ASSERT_INT_EQ(packet.biomes[63][63], chunk.biomes[63][63],
                              "chunk packet create must copy biomes");
}

static void test_chunk_stream_packet_serialize_guards_and_loop(void **state)
{
    ChunkStreamPacket packet;
    uint8_t buffer[9000];
    int expected_size;

    (void)state;

    packet = make_packet_with_objects();
    memset(buffer, 0, sizeof(buffer));
    expected_size = (int)chunk_stream_packet_estimate_size(packet.object_count);

    BANANA_TEST_ASSERT_INT_EQ(chunk_stream_packet_serialize(NULL, buffer, sizeof(buffer)),
                              -1,
                              "chunk packet serialize must reject null packet");
    BANANA_TEST_ASSERT_INT_EQ(chunk_stream_packet_serialize(&packet, NULL, sizeof(buffer)),
                              -1,
                              "chunk packet serialize must reject null buffer");
    BANANA_TEST_ASSERT_INT_EQ(chunk_stream_packet_serialize(&packet, buffer, (size_t)expected_size - 1),
                              -1,
                              "chunk packet serialize must reject undersized buffers");

    BANANA_TEST_ASSERT_INT_EQ(chunk_stream_packet_serialize(&packet, buffer, sizeof(buffer)),
                              expected_size,
                              "chunk packet serialize must include all object entries");
}

static void test_chunk_stream_packet_deserialize_guard_paths_and_success(void **state)
{
    ChunkStreamPacket packet;
    ChunkStreamPacket decoded;
    uint8_t buffer[9000];
    size_t full_size;
    size_t terrain_only_size;

    (void)state;

    packet = make_packet_with_objects();
    memset(&decoded, 0, sizeof(decoded));
    memset(buffer, 0, sizeof(buffer));

    full_size = (size_t)chunk_stream_packet_serialize(&packet, buffer, sizeof(buffer));
    terrain_only_size = 28u + 8192u;

    BANANA_TEST_ASSERT_INT_EQ(chunk_stream_packet_deserialize(NULL, full_size, &decoded),
                              -1,
                              "chunk packet deserialize must reject null source buffer");
    BANANA_TEST_ASSERT_INT_EQ(chunk_stream_packet_deserialize(buffer, full_size, NULL),
                              -1,
                              "chunk packet deserialize must reject null output packet");
    BANANA_TEST_ASSERT_INT_EQ(chunk_stream_packet_deserialize(buffer, 31, &decoded),
                              -1,
                              "chunk packet deserialize must reject too-small headers");

    BANANA_TEST_ASSERT_INT_EQ(chunk_stream_packet_deserialize(buffer, 28 + 8191, &decoded),
                              -1,
                              "chunk packet deserialize must require full terrain payload");
    BANANA_TEST_ASSERT_INT_EQ(chunk_stream_packet_deserialize(buffer, terrain_only_size, &decoded),
                              -1,
                              "chunk packet deserialize must require object_count field");
    BANANA_TEST_ASSERT_INT_EQ(chunk_stream_packet_deserialize(buffer, full_size - 1, &decoded),
                              -1,
                              "chunk packet deserialize must reject truncated object entries");

    buffer[terrain_only_size + 0] = 1;
    buffer[terrain_only_size + 1] = 1;
    BANANA_TEST_ASSERT_INT_EQ(chunk_stream_packet_deserialize(buffer, terrain_only_size + 2, &decoded),
                              -1,
                              "chunk packet deserialize must reject object_count over limit");

    (void)chunk_stream_packet_serialize(&packet, buffer, sizeof(buffer));
    BANANA_TEST_ASSERT_INT_EQ(chunk_stream_packet_deserialize(buffer, full_size, &decoded),
                              (int)full_size,
                              "chunk packet deserialize must decode full packet");
    BANANA_TEST_ASSERT_INT_EQ(decoded.object_count, packet.object_count,
                              "chunk packet deserialize must restore object_count");
    BANANA_TEST_ASSERT_INT_EQ(decoded.objects[1].object_id, packet.objects[1].object_id,
                              "chunk packet deserialize must decode object payload");
}

static void test_chunk_stream_packet_estimate_size_scales_with_object_count(void **state)
{
    size_t base;
    size_t with_three;

    (void)state;

    base = chunk_stream_packet_estimate_size(0);
    with_three = chunk_stream_packet_estimate_size(3);

    BANANA_TEST_ASSERT_INT_EQ((int)(with_three - base),
                              21,
                              "chunk packet estimate size must add seven bytes per object");
}

BANANA_TEST_MAIN(
    BANANA_TEST_CASE(test_chunk_stream_packet_create_guards_and_success),
    BANANA_TEST_CASE(test_chunk_stream_packet_serialize_guards_and_loop),
    BANANA_TEST_CASE(test_chunk_stream_packet_deserialize_guard_paths_and_success),
    BANANA_TEST_CASE(test_chunk_stream_packet_estimate_size_scales_with_object_count)
)
