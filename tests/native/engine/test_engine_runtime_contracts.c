#include <stdio.h>
#include <stdlib.h>

#include "../../../src/native/engine/engine.h"
#include "../../../src/native/engine/render/mesh.h"
#include "../../../src/native/engine/render/renderer.h"

static int s_pass = 0;
static int s_fail = 0;

#define TEST(name) do { printf("  %-64s", name); } while (0)
#define PASS() do { printf("PASS\n"); s_pass++; } while (0)
#define FAIL(msg) do { printf("FAIL (%s)\n", msg); s_fail++; } while (0)

#define ASSERT_TRUE(expr, msg) \
  do { \
    if (!(expr)) { \
      FAIL(msg); \
      return; \
    } \
  } while (0)

static int validate_stub_frame_pattern(Renderer *renderer, int width, int height) {
  const unsigned char *frame = renderer_get_frame_buffer(renderer);
  if (!frame) return 0;

  int pixel_count = width * height;
  int first = 0;
  int last = pixel_count - 1;

  if (frame[first * 4 + 0] != (unsigned char)(first % 256)) return 0;
  if (frame[first * 4 + 1] != 0x42) return 0;
  if (frame[first * 4 + 2] != 0x00) return 0;
  if (frame[first * 4 + 3] != 0xFF) return 0;

  if (frame[last * 4 + 0] != (unsigned char)(last % 256)) return 0;
  if (frame[last * 4 + 1] != 0x42) return 0;
  if (frame[last * 4 + 2] != 0x00) return 0;
  if (frame[last * 4 + 3] != 0xFF) return 0;

  return 1;
}

static void test_mesh_banana_variants_create(void) {
  TEST("mesh: banana-derived primitives construct successfully");

  Mesh *base = mesh_create_banana();
  Mesh *lod0 = mesh_create_banana_lod(0);
  Mesh *lod1 = mesh_create_banana_lod(1);
  Mesh *lod2 = mesh_create_banana_lod(2);
  Mesh *lod3 = mesh_create_banana_lod(3);
  Mesh *vectorized = mesh_create_banana_vector(1.2f, 0.9f, 0.8f, 0.2f, 2);
  Mesh *block_like = mesh_create_banana_block_like(1);
  Mesh *orb_like = mesh_create_banana_orb_like(1);

  ASSERT_TRUE(base != NULL, "base banana mesh failed");
  ASSERT_TRUE(lod0 != NULL && lod1 != NULL && lod2 != NULL && lod3 != NULL, "banana lod mesh failed");
  ASSERT_TRUE(vectorized != NULL, "banana vector mesh failed");
  ASSERT_TRUE(block_like != NULL, "banana block-like mesh failed");
  ASSERT_TRUE(orb_like != NULL, "banana orb-like mesh failed");

  mesh_destroy(base);
  mesh_destroy(lod0);
  mesh_destroy(lod1);
  mesh_destroy(lod2);
  mesh_destroy(lod3);
  mesh_destroy(vectorized);
  mesh_destroy(block_like);
  mesh_destroy(orb_like);
  PASS();
}

static void test_terrain_heightfield_mesh_contract(void) {
  TEST("mesh: deterministic terrain heightfield mesh contract");

  unsigned char heights[6 * 6];
  for (int z = 0; z < 6; z++) {
    for (int x = 0; x < 6; x++) {
      heights[z * 6 + x] = (unsigned char)((x + z) % 4);
    }
  }

  Mesh *terrain = mesh_create_terrain_heightfield(heights, 6, 6, 1.0f, 0.7f);
  ASSERT_TRUE(terrain != NULL, "terrain mesh generation failed");

  Mesh *invalid = mesh_create_terrain_heightfield(heights, 1, 6, 1.0f, 0.7f);
  ASSERT_TRUE(invalid == NULL, "invalid terrain dimensions unexpectedly accepted");

  mesh_destroy(terrain);
  PASS();
}

static void test_renderer_resize_for_viewport_profiles(void) {
  TEST("renderer: desktop/tablet/mobile resize profiles stay valid");

  Renderer *renderer = renderer_create(1366, 900);
  ASSERT_TRUE(renderer != NULL, "renderer_create failed");

  struct {
    int w;
    int h;
  } profiles[] = {
      {1366, 900},   /* desktop */
      {1024, 768},   /* tablet */
      {390, 844},    /* mobile portrait */
      {844, 390},    /* mobile landscape */
  };

  for (size_t i = 0; i < sizeof(profiles) / sizeof(profiles[0]); i++) {
    renderer_resize(renderer, profiles[i].w, profiles[i].h);
    renderer_begin_frame(renderer);
    renderer_end_frame(renderer);
    ASSERT_TRUE(validate_stub_frame_pattern(renderer, profiles[i].w, profiles[i].h),
                "frame buffer invalid after viewport resize");
  }

  renderer_destroy(renderer);
  PASS();
}

static void test_engine_runtime_tick_and_terrain_api(void) {
  TEST("engine: tick loop + terrain dirty update API contract");

  ASSERT_TRUE(engine_init(800, 600) == 0, "engine_init failed");

  uint32_t eid = engine_world_spawn(ENTITY_TYPE_NPC, 2.0f, 0.0f, 2.0f);
  ASSERT_TRUE(eid != 0, "engine_world_spawn failed");
  ASSERT_TRUE(engine_controller_attach(eid, "wildlife") != 0, "engine_controller_attach failed");

  ASSERT_TRUE(engine_terrain_set_height(3, 3, 2) == 1, "engine_terrain_set_height valid update failed");
  ASSERT_TRUE(engine_terrain_set_height(-1, 3, 2) == 0, "engine_terrain_set_height invalid update accepted");
  engine_terrain_mark_region_dirty(0, 0, 5, 5);

  for (int i = 0; i < 10; i++) {
    ASSERT_TRUE(engine_tick(1.0f / 60.0f) == 0, "engine_tick failed");
  }

  {
    int entity_count = engine_get_entity_count();
    int has_active_state = 0;
    ASSERT_TRUE(entity_count >= 1, "entity count contract violated");
    for (int idx = 0; idx < entity_count; idx++) {
      if (engine_get_entity_state(idx) == 1) {
        has_active_state = 1;
        break;
      }
    }
    ASSERT_TRUE(has_active_state == 1, "entity state not active");
  }

  engine_shutdown();
  PASS();
}

static void test_engine_right_click_path_is_inert(void) {
  TEST("engine: right-click path remains inert for movement contract");

  ASSERT_TRUE(engine_init(800, 600) == 0, "engine_init failed");

  ASSERT_TRUE(engine_get_click_count() == 0, "unexpected click count baseline");
  ASSERT_TRUE(engine_get_target_reached_count() == 0, "unexpected reached count baseline");
  ASSERT_TRUE(engine_get_has_move_target() == 0, "unexpected move target baseline");

  ASSERT_TRUE(engine_handle_right_click(200.0f, 150.0f) == 0, "right-click handler should remain inert");
  ASSERT_TRUE(engine_handle_right_click_normalized(0.5f, 0.5f) == 0,
              "normalized right-click handler should remain inert");

  ASSERT_TRUE(engine_get_click_count() == 0, "right-click unexpectedly mutated click count");
  ASSERT_TRUE(engine_get_target_reached_count() == 0,
              "right-click unexpectedly mutated target reached count");
  ASSERT_TRUE(engine_get_has_move_target() == 0,
              "right-click unexpectedly created a move target");

  engine_shutdown();
  PASS();
}

static void test_engine_mouse_bridge_is_inert_during_keyboard_input(void) {
  TEST("engine: mouse bridge stays inert while keyboard movement is active");

  ASSERT_TRUE(engine_init(800, 600) == 0, "engine_init failed");

  engine_set_move_input(1.0f, 0.0f);
  ASSERT_TRUE(engine_tick(1.0f / 60.0f) == 0, "engine_tick failed with keyboard input");

  ASSERT_TRUE(engine_handle_right_click_normalized(0.2f, 0.8f) == 0,
              "mouse bridge should remain inert while keyboard input is active");
  ASSERT_TRUE(engine_handle_right_click_normalized(-5.0f, 3.0f) == 0,
              "out-of-range normalized mouse bridge should remain inert");

  ASSERT_TRUE(engine_get_click_count() == 0, "mouse bridge unexpectedly mutated click count");
  ASSERT_TRUE(engine_get_target_reached_count() == 0,
              "mouse bridge unexpectedly mutated target reached count");
  ASSERT_TRUE(engine_get_has_move_target() == 0,
              "mouse bridge unexpectedly created a move target");

  engine_shutdown();
  PASS();
}

int main(void) {
  printf("\n=== Banana Engine Runtime Contract Tests ===\n\n");

  test_mesh_banana_variants_create();
  test_terrain_heightfield_mesh_contract();
  test_renderer_resize_for_viewport_profiles();
  test_engine_runtime_tick_and_terrain_api();
  test_engine_right_click_path_is_inert();
  test_engine_mouse_bridge_is_inert_during_keyboard_input();

  printf("\nResults: %d passed, %d failed\n", s_pass, s_fail);
  return (s_fail == 0) ? 0 : 1;
}
