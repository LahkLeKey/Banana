#include "render/actor_mesh_registry.h"

#include "runtime/support/test_support.h"

#include <string.h>

/* mesh_destroy stub since no real GL context is available */
void mesh_destroy(Mesh *m) { (void)m; }

static int g_foreach_calls = 0;
static int stub_foreach_cb(const ActorMeshEntry *entry, void *userdata)
{
    (void)entry;
    (void)userdata;
    g_foreach_calls += 1;
    return 0;
}

static int stub_foreach_stop_at_1(const ActorMeshEntry *entry, void *userdata)
{
    (void)entry;
    (void)userdata;
    g_foreach_calls += 1;
    return 1;   /* stop after first */
}

static void test_registry_init_register_lookup(void **state)
{
    (void)state;
    BANANA_TEST_ASSERT_INT_EQ(actor_mesh_registry_init(),
                              0,
                              "init must succeed");
    BANANA_TEST_ASSERT_INT_EQ(actor_mesh_registry_count(),
                              0,
                              "count must be zero after init");
    BANANA_TEST_ASSERT_TRUE(actor_mesh_registry_lookup("tree") == NULL,
                            "lookup on empty registry must return null");
    BANANA_TEST_ASSERT_TRUE(actor_mesh_registry_lookup(NULL) == NULL,
                            "lookup with null key must return null");

    ActorMeshLOD lods[2];
    memset(lods, 0, sizeof(lods));

    /* validation guards */
    BANANA_TEST_ASSERT_INT_EQ(actor_mesh_registry_register(NULL, lods, 1, 2.f, 1.f, 0),
                              -1,
                              "null actor_type must be rejected");
    BANANA_TEST_ASSERT_INT_EQ(actor_mesh_registry_register("tree", NULL, 1, 2.f, 1.f, 0),
                              -1,
                              "null lod_meshes must be rejected");
    BANANA_TEST_ASSERT_INT_EQ(actor_mesh_registry_register("tree", lods, 0, 2.f, 1.f, 0),
                              -1,
                              "zero lod_count must be rejected");
    BANANA_TEST_ASSERT_INT_EQ(actor_mesh_registry_register("tree", lods, ACTOR_LOD_COUNT + 1, 2.f, 1.f, 0),
                              -1,
                              "excessive lod_count must be rejected");

    /* valid registration */
    BANANA_TEST_ASSERT_INT_EQ(actor_mesh_registry_register("tree", lods, 2, 2.f, 1.f, ACTOR_FLAG_INTERACTIVE),
                              0,
                              "valid registration must succeed");
    BANANA_TEST_ASSERT_INT_EQ(actor_mesh_registry_count(),
                              1,
                              "count must be 1 after one registration");

    /* duplicate */
    BANANA_TEST_ASSERT_INT_EQ(actor_mesh_registry_register("tree", lods, 1, 2.f, 1.f, 0),
                              -1,
                              "duplicate actor_type must be rejected");

    /* second type */
    BANANA_TEST_ASSERT_INT_EQ(actor_mesh_registry_register("barrel", lods, 1, 1.f, 0.5f, 0),
                              0,
                              "second registration must succeed");

    const ActorMeshEntry *entry = actor_mesh_registry_lookup("tree");
    BANANA_TEST_ASSERT_TRUE(entry != NULL, "lookup for registered type must succeed");
    BANANA_TEST_ASSERT_INT_EQ(entry->lod_count, 2, "registered lod_count must match");

    BANANA_TEST_ASSERT_TRUE(actor_mesh_registry_lookup("unknown") == NULL,
                            "lookup for unknown type must return null");
}

static void test_registry_select_lod(void **state)
{
    (void)state;
    actor_mesh_registry_init();
    ActorMeshLOD lods[4];
    memset(lods, 0, sizeof(lods));
    actor_mesh_registry_register("npc", lods, 4, 1.f, 1.f, 0);

    const ActorMeshEntry *e = actor_mesh_registry_lookup("npc");

    BANANA_TEST_ASSERT_INT_EQ(actor_mesh_registry_select_lod(NULL, 5.f),
                              -1,
                              "null entry must return -1");
    BANANA_TEST_ASSERT_INT_EQ(actor_mesh_registry_select_lod(e, 5.f),
                              0,
                              "distance < 10 must select LOD 0 (ULTRA)");
    BANANA_TEST_ASSERT_INT_EQ(actor_mesh_registry_select_lod(e, 30.f),
                              1,
                              "distance 30 must select LOD 1 (HIGH)");
    BANANA_TEST_ASSERT_INT_EQ(actor_mesh_registry_select_lod(e, 80.f),
                              2,
                              "distance 80 must select LOD 2 (MED)");
    BANANA_TEST_ASSERT_INT_EQ(actor_mesh_registry_select_lod(e, 200.f),
                              3,
                              "distance 200 must select LOD 3 (LOW)");
}

static void test_registry_foreach_and_cleanup(void **state)
{
    (void)state;
    actor_mesh_registry_init();
    ActorMeshLOD lods[1];
    memset(lods, 0, sizeof(lods));
    actor_mesh_registry_register("a", lods, 1, 1.f, 1.f, 0);
    actor_mesh_registry_register("b", lods, 1, 1.f, 1.f, 0);
    actor_mesh_registry_register("c", lods, 1, 1.f, 1.f, 0);

    g_foreach_calls = 0;
    int n = actor_mesh_registry_foreach(stub_foreach_cb, NULL);
    BANANA_TEST_ASSERT_INT_EQ(n, 3,
                              "foreach must process all three entries");
    BANANA_TEST_ASSERT_INT_EQ(g_foreach_calls, 3,
                              "callback must be invoked for each entry");

    g_foreach_calls = 0;
    n = actor_mesh_registry_foreach(stub_foreach_stop_at_1, NULL);
    BANANA_TEST_ASSERT_INT_EQ(n, 1,
                              "foreach must stop when callback returns non-zero");

    BANANA_TEST_ASSERT_INT_EQ(actor_mesh_registry_foreach(NULL, NULL),
                              0,
                              "null callback must return 0");

    actor_mesh_registry_cleanup();
    BANANA_TEST_ASSERT_INT_EQ(actor_mesh_registry_count(),
                              0,
                              "count must be zero after cleanup");
}

BANANA_TEST_MAIN(
    BANANA_TEST_CASE(test_registry_init_register_lookup),
    BANANA_TEST_CASE(test_registry_select_lod),
    BANANA_TEST_CASE(test_registry_foreach_and_cleanup)
)
