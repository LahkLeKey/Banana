/**
 * @file chunk_stream_packet_test.c
 * @brief Unit tests for chunk stream packet serialization
 */

#include "runtime/chunk/chunk_stream_packet.h"
#include "terrain/terrain_chunks.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

static int tests_run = 0;
static int tests_passed = 0;

#define ASSERT_EQ(actual, expected, msg)                                                       \
    do                                                                                         \
    {                                                                                          \
        tests_run++;                                                                           \
        if ((actual) == (expected))                                                            \
        {                                                                                      \
            tests_passed++;                                                                    \
        }                                                                                      \
        else                                                                                   \
        {                                                                                      \
            printf("FAIL: %s (expected %lld, got %lld)\n",                                  \
                   msg,                                                                         \
                   (long long)(expected),                                                       \
                   (long long)(actual));                                                        \
        }                                                                                      \
    } while (0)

void test_create_packet(void)
{
    TerrainChunk chunk;
    memset(&chunk, 0, sizeof(chunk));
    chunk.chunk_x = 5;
    chunk.chunk_z = 10;
    chunk.version = 1;
    chunk.generation_tick = 12345;
    chunk.generation_fingerprint = 0x1122334455667788ULL;

    /* Fill with test data */
    for (int y = 0; y < 64; y++)
    {
        for (int x = 0; x < 64; x++)
        {
            chunk.heights[y][x] = (uint8_t)((x + y) % 256);
            chunk.biomes[y][x] = (uint8_t)(((x * y) + 1) % 5);
        }
    }

    ChunkStreamPacket packet;
    int ret = chunk_stream_packet_create(&chunk, 5, 10, &packet);
    ASSERT_EQ(ret, 0, "Create packet should succeed");

    ASSERT_EQ(packet.chunk_x, 5, "Chunk X should match");
    ASSERT_EQ(packet.chunk_z, 10, "Chunk Z should match");
    ASSERT_EQ(packet.version, 1, "Version should match");
    ASSERT_EQ(packet.generation_fingerprint,
              0x1122334455667788ULL,
              "Generation fingerprint should match");
    ASSERT_EQ(packet.object_count, 0, "Object count should be 0");

    /* Verify data copied */
    ASSERT_EQ(packet.heights[0][0], chunk.heights[0][0], "Height data should match");
    ASSERT_EQ(packet.biomes[63][63], chunk.biomes[63][63], "Biome data should match");
}

void test_serialize_deserialize(void)
{
    TerrainChunk chunk;
    memset(&chunk, 0, sizeof(chunk));
    chunk.chunk_x = 0;
    chunk.chunk_z = 0;
    chunk.version = 42;
    chunk.generation_tick = 999;
    chunk.generation_fingerprint = 0xAABBCCDDEEFF0011ULL;

    for (int y = 0; y < 64; y++)
    {
        for (int x = 0; x < 64; x++)
        {
            chunk.heights[y][x] = (uint8_t)(x + y);
            chunk.biomes[y][x] = (uint8_t)(x % 5);
        }
    }

    ChunkStreamPacket packet;
    chunk_stream_packet_create(&chunk, 0, 0, &packet);

    /* Serialize */
    uint8_t buffer[16384]; /* Large enough for packet + objects */
    int serialized = chunk_stream_packet_serialize(&packet, buffer, sizeof(buffer));
    ASSERT_EQ((serialized > 0), 1, "Serialize should return > 0");

    /* Deserialize */
    ChunkStreamPacket packet2;
    int deserialized = chunk_stream_packet_deserialize(buffer, serialized, &packet2);
    ASSERT_EQ(deserialized, serialized, "Deserialized size should match");

    /* Verify round-trip */
    ASSERT_EQ(packet2.chunk_x, 0, "Chunk X should survive round-trip");
    ASSERT_EQ(packet2.chunk_z, 0, "Chunk Z should survive round-trip");
    ASSERT_EQ(packet2.version, 42, "Version should survive round-trip");
    ASSERT_EQ(packet2.generation_tick, 999, "Tick should survive round-trip");
    ASSERT_EQ(packet2.generation_fingerprint,
              0xAABBCCDDEEFF0011ULL,
              "Fingerprint should survive round-trip");

    /* Verify data */
    ASSERT_EQ(memcmp(packet.heights, packet2.heights, sizeof(packet.heights)), 0,
              "Heights should match");
    ASSERT_EQ(memcmp(packet.biomes, packet2.biomes, sizeof(packet.biomes)), 0,
              "Biomes should match");
}

void test_buffer_too_small(void)
{
    TerrainChunk chunk;
    memset(&chunk, 0, sizeof(chunk));
    chunk.version = 1;

    ChunkStreamPacket packet;
    chunk_stream_packet_create(&chunk, 0, 0, &packet);

    uint8_t buffer[10]; /* Too small */
    int ret = chunk_stream_packet_serialize(&packet, buffer, sizeof(buffer));
    ASSERT_EQ(ret, -1, "Serialize with small buffer should fail");
}

void test_packet_size_estimation(void)
{
    size_t size_no_objects = chunk_stream_packet_estimate_size(0);
    size_t size_10_objects = chunk_stream_packet_estimate_size(10);
    size_t size_100_objects = chunk_stream_packet_estimate_size(100);

    /* Base size: 28 + 4096 + 4096 + 2 = 8222 */
    ASSERT_EQ((size_no_objects == 8222), 1, "Base size should be 8222");

    /* Each object adds 7 bytes */
    ASSERT_EQ((size_10_objects == size_no_objects + 70), 1, "10 objects adds 70 bytes");
    ASSERT_EQ((size_100_objects == size_no_objects + 700), 1, "100 objects adds 700 bytes");
}

void test_deserialize_invalid_buffer(void)
{
    uint8_t buffer[10];
    ChunkStreamPacket packet;

    int ret = chunk_stream_packet_deserialize(buffer, 10, &packet);
    ASSERT_EQ(ret, -1, "Deserialize small buffer should fail");
}

void test_packet_with_objects(void)
{
    TerrainChunk chunk;
    memset(&chunk, 0, sizeof(chunk));
    chunk.version = 1;

    ChunkStreamPacket packet;
    chunk_stream_packet_create(&chunk, 0, 0, &packet);

    /* Add objects manually (simulating spawner output) */
    packet.object_count = 3;
    packet.objects[0] = (ChunkStreamObjectEntry){.object_id = 1000,
                                                 .tile_x = 10,
                                                 .tile_z = 20,
                                                 .resource_type = 1}; /* RESOURCE_WOOD */
    packet.objects[1] = (ChunkStreamObjectEntry){.object_id = 1001,
                                                 .tile_x = 30,
                                                 .tile_z = 40,
                                                 .resource_type = 0}; /* RESOURCE_ORE */
    packet.objects[2] = (ChunkStreamObjectEntry){.object_id = 1002,
                                                 .tile_x = 50,
                                                 .tile_z = 60,
                                                 .resource_type = 2}; /* RESOURCE_STONE */

    /* Serialize */
    uint8_t buffer[16384];
    int serialized = chunk_stream_packet_serialize(&packet, buffer, sizeof(buffer));
    ASSERT_EQ((serialized > 0), 1, "Serialize with objects should succeed");

    /* Deserialize */
    ChunkStreamPacket packet2;
    int deserialized = chunk_stream_packet_deserialize(buffer, serialized, &packet2);
    ASSERT_EQ(deserialized, serialized, "Deserialize should match serialize size");

    ASSERT_EQ(packet2.object_count, 3, "Object count should be preserved");
    ASSERT_EQ(packet2.objects[0].object_id, 1000, "Object 0 ID should match");
    ASSERT_EQ(packet2.objects[1].tile_x, 30, "Object 1 tile_x should match");
    ASSERT_EQ(packet2.objects[2].resource_type, 2, "Object 2 resource type should match");
}

void test_multiple_chunks(void)
{
    /* Create packets for different chunks */
    TerrainChunk chunk1, chunk2;
    memset(&chunk1, 0, sizeof(chunk1));
    memset(&chunk2, 0, sizeof(chunk2));

    chunk1.version = 1;
    chunk1.heights[0][0] = 100;
    chunk2.version = 2;
    chunk2.heights[0][0] = 200;

    ChunkStreamPacket packet1, packet2;
    chunk_stream_packet_create(&chunk1, 0, 0, &packet1);
    chunk_stream_packet_create(&chunk2, 1, 1, &packet2);

    /* Serialize both */
    uint8_t buffer1[16384];
    uint8_t buffer2[16384];
    int size1 = chunk_stream_packet_serialize(&packet1, buffer1, sizeof(buffer1));
    int size2 = chunk_stream_packet_serialize(&packet2, buffer2, sizeof(buffer2));

    /* Deserialize */
    ChunkStreamPacket restored1, restored2;
    chunk_stream_packet_deserialize(buffer1, size1, &restored1);
    chunk_stream_packet_deserialize(buffer2, size2, &restored2);

    ASSERT_EQ(restored1.version, 1, "Chunk 1 version should be 1");
    ASSERT_EQ(restored2.version, 2, "Chunk 2 version should be 2");
    ASSERT_EQ(restored1.heights[0][0], 100, "Chunk 1 height should be 100");
    ASSERT_EQ(restored2.heights[0][0], 200, "Chunk 2 height should be 200");
}

void test_edge_case_max_objects(void)
{
    ChunkStreamPacket packet;
    memset(&packet, 0, sizeof(packet));

    /* Fill to max objects */
    packet.object_count = CHUNK_STREAM_MAX_OBJECTS;
    for (int i = 0; i < CHUNK_STREAM_MAX_OBJECTS; i++)
    {
        packet.objects[i].object_id = 1000 + i;
        packet.objects[i].tile_x = (uint8_t)(i % 64);
        packet.objects[i].tile_z = (uint8_t)(i / 64);
        packet.objects[i].resource_type = (uint8_t)(i % 3);
    }

    size_t estimated = chunk_stream_packet_estimate_size(CHUNK_STREAM_MAX_OBJECTS);
    uint8_t *buffer = (uint8_t *)malloc(estimated + 100); /* Extra for safety */

    int serialized = chunk_stream_packet_serialize(&packet, buffer, estimated + 100);
    ASSERT_EQ((serialized > 0), 1, "Serialize max objects should succeed");

    ChunkStreamPacket packet2;
    int deserialized = chunk_stream_packet_deserialize(buffer, serialized, &packet2);
    ASSERT_EQ(deserialized, serialized, "Deserialize max objects should work");
    ASSERT_EQ(packet2.object_count, CHUNK_STREAM_MAX_OBJECTS, "Object count should be max");

    free(buffer);
}

int main(void)
{
    printf("=== Chunk Stream Packet Tests ===\n\n");

    test_create_packet();
    test_serialize_deserialize();
    test_buffer_too_small();
    test_packet_size_estimation();
    test_deserialize_invalid_buffer();
    test_packet_with_objects();
    test_multiple_chunks();
    test_edge_case_max_objects();

    printf("\n=== Results ===\n");
    printf("Passed: %d / %d\n", tests_passed, tests_run);

    return (tests_passed == tests_run) ? 0 : 1;
}
