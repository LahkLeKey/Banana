/**
 * @file interactive_objects_test.c
 * @brief Unit tests for interactive object system
 */

#include "runtime/interaction/objects/interactive_objects.h"

#include <assert.h>
#include <stdio.h>
#include <string.h>

/* Test: Initialize system */
int test_interactive_objects_init(void) {
  int status = interactive_objects_init();
  assert(status == 0);
  assert(interactive_objects_count() == 0);
  printf("✓ Test: init\n");
  return 0;
}

/* Test: Register object */
int test_interactive_objects_register(void) {
  interactive_objects_init();

  InteractiveObject obj = {
      .type = INTERACTIVE_TYPE_TREE,
      .x = 5.0f,
      .z = 10.0f,
      .click_radius = 1.5f,
      .state = INTERACTIVE_STATE_ACTIVE,
      .resource_yield = 5,
      .resource_type = "wood",
  };

  int obj_id = interactive_objects_register(&obj);
  assert(obj_id > 0);
  assert(interactive_objects_count() == 1);

  InteractiveObject retrieved;
  int status = interactive_objects_get(obj_id, &retrieved);
  assert(status == 0);
  assert(retrieved.x == 5.0f);
  assert(retrieved.z == 10.0f);
  assert(retrieved.type == INTERACTIVE_TYPE_TREE);
  assert(retrieved.state == INTERACTIVE_STATE_ACTIVE);

  printf("✓ Test: register\n");
  return 0;
}

/* Test: Click detection - hit */
int test_interactive_objects_click_hit(void) {
  interactive_objects_init();

  InteractiveObject obj = {
      .type = INTERACTIVE_TYPE_TREE,
      .x = 0.0f,
      .z = 0.0f,
      .click_radius = 2.0f,
      .state = INTERACTIVE_STATE_ACTIVE,
      .resource_yield = 5,
      .resource_type = "wood",
  };

  int obj_id = interactive_objects_register(&obj);
  assert(obj_id > 0);

  /* Click near center of object */
  int clicked_id = -1;
  int yield = interactive_objects_click_at(0.5f, 0.5f, 2.0f, &clicked_id);

  assert(yield == 5);
  assert(clicked_id == obj_id);

  /* Verify object transitioned to depleted state */
  InteractiveObject retrieved;
  interactive_objects_get(obj_id, &retrieved);
  assert(retrieved.state == INTERACTIVE_STATE_DEPLETED);

  printf("✓ Test: click_hit\n");
  return 0;
}

/* Test: Click detection - miss (outside radius) */
int test_interactive_objects_click_miss(void) {
  interactive_objects_init();

  InteractiveObject obj = {
      .type = INTERACTIVE_TYPE_TREE,
      .x = 0.0f,
      .z = 0.0f,
      .click_radius = 2.0f,
      .state = INTERACTIVE_STATE_ACTIVE,
      .resource_yield = 5,
      .resource_type = "wood",
  };

  int obj_id = interactive_objects_register(&obj);
  assert(obj_id > 0);

  /* Click far outside object */
  int clicked_id = -1;
  int yield = interactive_objects_click_at(10.0f, 10.0f, 2.0f, &clicked_id);

  assert(yield == 0);
  assert(clicked_id == -1);

  /* Object should still be active */
  InteractiveObject retrieved;
  interactive_objects_get(obj_id, &retrieved);
  assert(retrieved.state == INTERACTIVE_STATE_ACTIVE);

  printf("✓ Test: click_miss\n");
  return 0;
}

/* Test: Click detection - inactive object ignored */
int test_interactive_objects_click_inactive(void) {
  interactive_objects_init();

  InteractiveObject obj = {
      .type = INTERACTIVE_TYPE_TREE,
      .x = 0.0f,
      .z = 0.0f,
      .click_radius = 2.0f,
      .state = INTERACTIVE_STATE_DEPLETED,
      .resource_yield = 5,
      .resource_type = "wood",
  };

  int obj_id = interactive_objects_register(&obj);
  assert(obj_id > 0);

  /* Click on inactive object - should be ignored */
  int clicked_id = -1;
  int yield = interactive_objects_click_at(0.0f, 0.0f, 2.0f, &clicked_id);

  assert(yield == 0);
  assert(clicked_id == -1);

  printf("✓ Test: click_inactive\n");
  return 0;
}

/* Test: Multiple objects - closest wins */
int test_interactive_objects_click_closest(void) {
  interactive_objects_init();

  InteractiveObject obj1 = {
      .type = INTERACTIVE_TYPE_TREE,
      .x = 0.0f,
      .z = 0.0f,
      .click_radius = 2.0f,
      .state = INTERACTIVE_STATE_ACTIVE,
      .resource_yield = 5,
      .resource_type = "wood",
  };

  InteractiveObject obj2 = {
      .type = INTERACTIVE_TYPE_ORE_DEPOSIT,
      .x = 5.0f,
      .z = 5.0f,
      .click_radius = 2.0f,
      .state = INTERACTIVE_STATE_ACTIVE,
      .resource_yield = 3,
      .resource_type = "ore",
  };

  int id1 = interactive_objects_register(&obj1);
  int id2 = interactive_objects_register(&obj2);

  /* Click closer to object1 */
  int clicked_id = -1;
  int yield = interactive_objects_click_at(0.5f, 0.5f, 10.0f, &clicked_id);

  assert(yield == 5);
  assert(clicked_id == id1);

  interactive_objects_init(); /* Reset */

  /* Re-register in different order */
  id2 = interactive_objects_register(&obj2);
  id1 = interactive_objects_register(&obj1);

  /* Click closer to object1 again - should still select closest */
  clicked_id = -1;
  yield = interactive_objects_click_at(0.5f, 0.5f, 10.0f, &clicked_id);

  assert(yield == 5);
  assert(clicked_id == id1);

  printf("✓ Test: click_closest\n");
  return 0;
}

/* Test: State management */
int test_interactive_objects_state_management(void) {
  interactive_objects_init();

  InteractiveObject obj = {
      .type = INTERACTIVE_TYPE_BARREL,
      .x = 1.0f,
      .z = 1.0f,
      .click_radius = 1.0f,
      .state = INTERACTIVE_STATE_ACTIVE,
      .resource_yield = 1,
      .resource_type = "water",
  };

  int obj_id = interactive_objects_register(&obj);

  /* Check initial state */
  InteractiveObjectState state;
  int status = interactive_objects_get_state(obj_id, &state);
  assert(status == 0);
  assert(state == INTERACTIVE_STATE_ACTIVE);

  /* Set to cooldown */
  status = interactive_objects_set_state(obj_id, INTERACTIVE_STATE_COOLDOWN, 0);
  assert(status == 0);

  status = interactive_objects_get_state(obj_id, &state);
  assert(status == 0);
  assert(state == INTERACTIVE_STATE_COOLDOWN);

  /* Set to depleted */
  status = interactive_objects_set_state(obj_id, INTERACTIVE_STATE_DEPLETED, 100);
  assert(status == 0);

  status = interactive_objects_get_state(obj_id, &state);
  assert(status == 0);
  assert(state == INTERACTIVE_STATE_DEPLETED);

  printf("✓ Test: state_management\n");
  return 0;
}

/* Test: Cooldown tick */
int test_interactive_objects_cooldown_tick(void) {
  interactive_objects_init();

  InteractiveObject obj = {
      .type = INTERACTIVE_TYPE_TREE,
      .x = 0.0f,
      .z = 0.0f,
      .click_radius = 2.0f,
      .state = INTERACTIVE_STATE_ACTIVE,
      .resource_yield = 5,
      .resource_type = "wood",
  };

  int obj_id = interactive_objects_register(&obj);

  /* Transition to cooldown at time 1000 */
  interactive_objects_set_state(obj_id, INTERACTIVE_STATE_COOLDOWN, 1000);

  /* Tick at time 2000 (before cooldown expires) */
  interactive_objects_tick(2000, 5000); /* 5 second cooldown */

  InteractiveObjectState state;
  interactive_objects_get_state(obj_id, &state);
  assert(state == INTERACTIVE_STATE_COOLDOWN); /* Still in cooldown */

  /* Tick at time 6100 (after cooldown expires) */
  interactive_objects_tick(6100, 5000); /* 5 second cooldown */

  interactive_objects_get_state(obj_id, &state);
  assert(state == INTERACTIVE_STATE_ACTIVE); /* Cooldown expired */

  printf("✓ Test: cooldown_tick\n");
  return 0;
}

/* Test: Snapshot */
int test_interactive_objects_snapshot(void) {
  interactive_objects_init();

  for (int i = 0; i < 5; i++) {
    InteractiveObject obj = {
        .type = INTERACTIVE_TYPE_TREE,
        .x = (float)i,
        .z = (float)i,
        .click_radius = 1.0f,
        .state = INTERACTIVE_STATE_ACTIVE,
        .resource_yield = 5,
        .resource_type = "wood",
    };
    interactive_objects_register(&obj);
  }

  assert(interactive_objects_count() == 5);

  InteractiveObject snapshot[10];
  int count = interactive_objects_snapshot(10, snapshot);

  assert(count == 5);
  for (int i = 0; i < 5; i++) {
    assert(snapshot[i].x == (float)i);
    assert(snapshot[i].z == (float)i);
  }

  printf("✓ Test: snapshot\n");
  return 0;
}

/* Test: Clear */
int test_interactive_objects_clear(void) {
  interactive_objects_init();

  for (int i = 0; i < 5; i++) {
    InteractiveObject obj = {
        .type = INTERACTIVE_TYPE_TREE,
        .x = (float)i,
        .z = (float)i,
        .click_radius = 1.0f,
        .state = INTERACTIVE_STATE_ACTIVE,
        .resource_yield = 5,
        .resource_type = "wood",
    };
    interactive_objects_register(&obj);
  }

  assert(interactive_objects_count() == 5);

  interactive_objects_clear();
  assert(interactive_objects_count() == 0);

  printf("✓ Test: clear\n");
  return 0;
}

int main(void) {
  printf("Running interactive objects tests...\n\n");

  int failed = 0;

  failed += test_interactive_objects_init();
  failed += test_interactive_objects_register();
  failed += test_interactive_objects_click_hit();
  failed += test_interactive_objects_click_miss();
  failed += test_interactive_objects_click_inactive();
  failed += test_interactive_objects_click_closest();
  failed += test_interactive_objects_state_management();
  failed += test_interactive_objects_cooldown_tick();
  failed += test_interactive_objects_snapshot();
  failed += test_interactive_objects_clear();

  printf("\n%s\n", failed == 0 ? "All tests passed! ✓" : "Some tests failed! ✗");
  return failed;
}
