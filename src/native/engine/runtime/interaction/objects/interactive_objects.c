/**
 * @file interactive_objects.c
 * @brief Interactive object system implementation
 *
 * Manages interactive objects (trees, ore deposits, etc.) that players can click to
 * trigger actions. State is owned by C and synchronized across multiplayer clients.
 */

#include "interactive_objects.h"

#include <string.h>

/* Maximum interactive objects in a world */
#define MAX_INTERACTIVE_OBJECTS 512

/* Interactive object registry */
static struct {
  int count;
  InteractiveObject objects[MAX_INTERACTIVE_OBJECTS];
} g_interactive_objects_registry = {0};

/* Next object ID counter */
static int g_next_object_id = 1;

int interactive_objects_init(void) {
  g_interactive_objects_registry.count = 0;
  g_next_object_id = 1;
  return 0;
}

int interactive_objects_register(const InteractiveObject *object) {
  if (!object) {
    return -1;
  }

  if (g_interactive_objects_registry.count >= MAX_INTERACTIVE_OBJECTS) {
    return -1; /* Registry full */
  }

  InteractiveObject obj = *object;
  obj.id = g_next_object_id++;
  obj.state_changed_at = 0;
  if (obj.state == 0) {
    obj.state = INTERACTIVE_STATE_ACTIVE;
  }

  g_interactive_objects_registry.objects[g_interactive_objects_registry.count++] = obj;
  return obj.id;
}

int interactive_objects_click_at(float world_x, float world_z, float click_radius,
                                  int *out_object_id) {
  if (!out_object_id) {
    return -1;
  }

  *out_object_id = -1;

  /* Find closest object within click radius */
  float closest_dist = click_radius;
  int closest_index = -1;

  for (int i = 0; i < g_interactive_objects_registry.count; i++) {
    const InteractiveObject *obj = &g_interactive_objects_registry.objects[i];

    /* Skip non-active objects */
    if (obj->state != INTERACTIVE_STATE_ACTIVE) {
      continue;
    }

    /* Calculate distance from click to object center */
    float dx = world_x - obj->x;
    float dz = world_z - obj->z;
    float dist = dx * dx + dz * dz; /* Use squared distance for efficiency */
    float obj_radius_sq = obj->click_radius * obj->click_radius;

    if (dist <= obj_radius_sq && dist < closest_dist * closest_dist) {
      closest_dist = dist;
      closest_index = i;
    }
  }

  if (closest_index < 0) {
    return 0; /* No object clicked */
  }

  InteractiveObject *obj = &g_interactive_objects_registry.objects[closest_index];
  *out_object_id = obj->id;

  /* Transition to depleted state on click */
  obj->state = INTERACTIVE_STATE_DEPLETED;

  return obj->resource_yield;
}

int interactive_objects_get_state(int object_id, InteractiveObjectState *out_state) {
  if (!out_state) {
    return -1;
  }

  for (int i = 0; i < g_interactive_objects_registry.count; i++) {
    if (g_interactive_objects_registry.objects[i].id == object_id) {
      *out_state = g_interactive_objects_registry.objects[i].state;
      return 0;
    }
  }

  return -1; /* Object not found */
}

int interactive_objects_set_state(int object_id, InteractiveObjectState state,
                                   int64_t current_time_ms) {
  for (int i = 0; i < g_interactive_objects_registry.count; i++) {
    InteractiveObject *obj = &g_interactive_objects_registry.objects[i];
    if (obj->id == object_id) {
      obj->state = state;
      obj->state_changed_at = current_time_ms;
      return 0;
    }
  }

  return -1; /* Object not found */
}

void interactive_objects_tick(int64_t current_time_ms, int64_t cooldown_duration_ms) {
  for (int i = 0; i < g_interactive_objects_registry.count; i++) {
    InteractiveObject *obj = &g_interactive_objects_registry.objects[i];

    /* Check if cooldown has expired */
    if (obj->state == INTERACTIVE_STATE_COOLDOWN) {
      int64_t elapsed = current_time_ms - obj->state_changed_at;
      if (elapsed >= cooldown_duration_ms) {
        obj->state = INTERACTIVE_STATE_ACTIVE;
      }
    }
  }
}

int interactive_objects_count(void) {
  return g_interactive_objects_registry.count;
}

int interactive_objects_get(int object_id, InteractiveObject *out_object) {
  if (!out_object) {
    return -1;
  }

  for (int i = 0; i < g_interactive_objects_registry.count; i++) {
    if (g_interactive_objects_registry.objects[i].id == object_id) {
      *out_object = g_interactive_objects_registry.objects[i];
      return 0;
    }
  }

  return -1; /* Object not found */
}

int interactive_objects_snapshot(int max_count, InteractiveObject *out_objects) {
  if (!out_objects || max_count <= 0) {
    return 0;
  }

  int count = 0;
  for (int i = 0; i < g_interactive_objects_registry.count && count < max_count; i++) {
    out_objects[count++] = g_interactive_objects_registry.objects[i];
  }

  return count;
}

void interactive_objects_clear(void) {
  g_interactive_objects_registry.count = 0;
  g_next_object_id = 1;
}

void interactive_objects_shutdown(void) {
  interactive_objects_clear();
}
