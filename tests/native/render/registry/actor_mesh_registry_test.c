/**
 * @file actor_mesh_registry_test.c
 * @brief Unit tests for actor mesh registry
 *
 * Tests registration, lookup, LOD selection, and cleanup of actor meshes.
 */

#include "actor_mesh_registry.h"
#include <stdio.h>
#include <string.h>

/* Test counters */
static int tests_run = 0;
static int tests_passed = 0;

/* Simple assertion macro */
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
            printf("FAIL: %s (expected %d, got %d)\n", msg, expected, actual);                \
        }                                                                                      \
    } while (0)

#define ASSERT_FLOAT_EQ(actual, expected, tolerance, msg)                                      \
    do                                                                                         \
    {                                                                                          \
        tests_run++;                                                                           \
        float _actual = (float)(actual);                                                      \
        float _expected = (float)(expected);                                                  \
        float _delta = _actual - _expected;                                                   \
        if (_delta < 0.0f)                                                                     \
            _delta = -_delta;                                                                  \
        if (_delta <= (tolerance))                                                             \
        {                                                                                      \
            tests_passed++;                                                                    \
        }                                                                                      \
        else                                                                                   \
        {                                                                                      \
            printf("FAIL: %s (expected %.3f, got %.3f)\n", msg, (double)_expected, (double)_actual); \
        }                                                                                      \
    } while (0)

#define ASSERT_NULL(ptr, msg)                                                                  \
    do                                                                                         \
    {                                                                                          \
        tests_run++;                                                                           \
        if ((ptr) == NULL)                                                                     \
        {                                                                                      \
            tests_passed++;                                                                    \
        }                                                                                      \
        else                                                                                   \
        {                                                                                      \
            printf("FAIL: %s (expected NULL, got non-NULL)\n", msg);                          \
        }                                                                                      \
    } while (0)

#define ASSERT_NOT_NULL(ptr, msg)                                                              \
    do                                                                                         \
    {                                                                                          \
        tests_run++;                                                                           \
        if ((ptr) != NULL)                                                                     \
        {                                                                                      \
            tests_passed++;                                                                    \
        }                                                                                      \
        else                                                                                   \
        {                                                                                      \
            printf("FAIL: %s (expected non-NULL)\n", msg);                                    \
        }                                                                                      \
    } while (0)

/**
 * Test: Initialize registry
 */
void test_init(void)
{
    int ret = actor_mesh_registry_init();
    ASSERT_EQ(ret, 0, "actor_mesh_registry_init should return 0");
    ASSERT_EQ(actor_mesh_registry_count(), 0, "Registry should be empty after init");
}

/**
 * Test: Register actor type with single LOD
 */
void test_register_single_lod(void)
{
    actor_mesh_registry_init();

    /* Create a minimal LOD entry with NULL mesh (for testing registry logic, not rendering) */
    ActorMeshLOD lod = {.mesh = NULL, .click_radius = 1.5f, .vertex_count = 100};
    lod.aabb_min[0] = -1.0f;
    lod.aabb_min[1] = 0.0f;
    lod.aabb_min[2] = -1.0f;
    lod.aabb_max[0] = 1.0f;
    lod.aabb_max[1] = 2.0f;
    lod.aabb_max[2] = 1.0f;

    int ret =
        actor_mesh_registry_register("tree", &lod, 1, 1.5f, 1.0f, ACTOR_FLAG_INTERACTIVE);
    ASSERT_EQ(ret, 0, "Register tree should succeed");
    ASSERT_EQ(actor_mesh_registry_count(), 1, "Registry count should be 1");

    const ActorMeshEntry *entry = actor_mesh_registry_lookup("tree");
    ASSERT_NOT_NULL(entry, "Lookup tree should find entry");
    if (entry)
    {
        ASSERT_EQ(strcmp(entry->actor_type, "tree"), 0, "Actor type should match");
        ASSERT_EQ(entry->lod_count, 1, "LOD count should be 1");
        ASSERT_EQ(entry->flags & ACTOR_FLAG_INTERACTIVE, ACTOR_FLAG_INTERACTIVE,
                  "Should have INTERACTIVE flag");
    }

    actor_mesh_registry_cleanup();
}

/**
 * Test: Register multiple LOD levels
 */
void test_register_multiple_lods(void)
{
    actor_mesh_registry_init();

    ActorMeshLOD lods[3] = {
        {.mesh = NULL, .click_radius = 1.5f, .vertex_count = 1000},
        {.mesh = NULL, .click_radius = 1.5f, .vertex_count = 500},
        {.mesh = NULL, .click_radius = 1.5f, .vertex_count = 100},
    };

    int ret = actor_mesh_registry_register("merchant", lods, 3, 2.0f, 1.2f, ACTOR_FLAG_MOBILE);
    ASSERT_EQ(ret, 0, "Register merchant should succeed");

    const ActorMeshEntry *entry = actor_mesh_registry_lookup("merchant");
    ASSERT_NOT_NULL(entry, "Lookup merchant should find entry");
    if (entry)
    {
        ASSERT_EQ(entry->lod_count, 3, "LOD count should be 3");
        ASSERT_FLOAT_EQ(entry->scale, 1.2f, 0.001f, "Scale should be 1.2");
        ASSERT_EQ(entry->flags & ACTOR_FLAG_MOBILE, ACTOR_FLAG_MOBILE,
                  "Should have MOBILE flag");
    }

    actor_mesh_registry_cleanup();
}

/**
 * Test: LOD selection based on distance
 */
void test_lod_selection(void)
{
    actor_mesh_registry_init();

    ActorMeshLOD lods[4] = {
        {.mesh = NULL, .click_radius = 1.0f, .vertex_count = 1000},
        {.mesh = NULL, .click_radius = 1.0f, .vertex_count = 500},
        {.mesh = NULL, .click_radius = 1.0f, .vertex_count = 200},
        {.mesh = NULL, .click_radius = 1.0f, .vertex_count = 50},
    };

    actor_mesh_registry_register("barrel", lods, 4, 1.0f, 0.8f, ACTOR_FLAG_STATIC);
    const ActorMeshEntry *entry = actor_mesh_registry_lookup("barrel");

    /* Test LOD selection at various distances */
    ASSERT_EQ(actor_mesh_registry_select_lod(entry, 5.0f), 0, "Distance 5 should select LOD0");
    ASSERT_EQ(actor_mesh_registry_select_lod(entry, 25.0f), 1, "Distance 25 should select LOD1");
    ASSERT_EQ(actor_mesh_registry_select_lod(entry, 100.0f), 2, "Distance 100 should select LOD2");
    ASSERT_EQ(actor_mesh_registry_select_lod(entry, 200.0f), 3, "Distance 200 should select LOD3");

    actor_mesh_registry_cleanup();
}

/**
 * Test: Lookup non-existent actor
 */
void test_lookup_nonexistent(void)
{
    actor_mesh_registry_init();

    const ActorMeshEntry *entry = actor_mesh_registry_lookup("nonexistent");
    ASSERT_NULL(entry, "Lookup nonexistent should return NULL");

    actor_mesh_registry_cleanup();
}

/**
 * Test: Reject duplicate registration
 */
void test_reject_duplicate(void)
{
    actor_mesh_registry_init();

    ActorMeshLOD lod = {.mesh = NULL, .click_radius = 1.0f, .vertex_count = 100};

    int ret1 = actor_mesh_registry_register("oak", &lod, 1, 1.0f, 1.0f, 0);
    ASSERT_EQ(ret1, 0, "First registration should succeed");

    int ret2 = actor_mesh_registry_register("oak", &lod, 1, 1.0f, 1.0f, 0);
    ASSERT_EQ(ret2, -1, "Duplicate registration should fail");
    ASSERT_EQ(actor_mesh_registry_count(), 1, "Registry should still have 1 entry");

    actor_mesh_registry_cleanup();
}

/**
 * Test: Foreach iteration
 */
int count_callback(const ActorMeshEntry *entry, void *userdata)
{
    if (userdata)
    {
        int *count = (int *)userdata;
        (*count)++;
    }
    return 0; /* Continue iteration */
}

void test_foreach(void)
{
    actor_mesh_registry_init();

    ActorMeshLOD lod = {.mesh = NULL, .click_radius = 1.0f, .vertex_count = 100};

    actor_mesh_registry_register("pine", &lod, 1, 1.0f, 1.0f, 0);
    actor_mesh_registry_register("birch", &lod, 1, 1.0f, 1.0f, 0);
    actor_mesh_registry_register("maple", &lod, 1, 1.0f, 1.0f, 0);

    int count = 0;
    int ret = actor_mesh_registry_foreach(count_callback, &count);
    ASSERT_EQ(ret, 3, "Foreach should process 3 entries");
    ASSERT_EQ(count, 3, "Callback should be called 3 times");

    actor_mesh_registry_cleanup();
}

/**
 * Main test runner
 */
int main(void)
{
    printf("=== Actor Mesh Registry Tests ===\n\n");

    test_init();
    test_register_single_lod();
    test_register_multiple_lods();
    test_lod_selection();
    test_lookup_nonexistent();
    test_reject_duplicate();
    test_foreach();

    printf("\n=== Results ===\n");
    printf("Passed: %d / %d\n", tests_passed, tests_run);

    return (tests_passed == tests_run) ? 0 : 1;
}
