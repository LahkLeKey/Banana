/* wildlife_controller.c
 *
 * NPC wildlife AI controller — 4-state FSM:
 *   IDLE → PATROL → INVESTIGATE → RETURN → IDLE
 *
 * Registers itself with the controller registry under the name "wildlife".
 * Spawn via: engine_controller_attach(entity_id, "wildlife")
 */

#include "wildlife_controller.h"
#include "controller.h"
#include "navigation.h"
#include "state_machine.h"
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* ── Internal state ────────────────────────────────────────────────────────── */

typedef struct
{
    float position[3];            /* current world position (synced to entity) */
    float patrol_origin[3];       /* spawn-point / patrol base                 */
    float patrol_waypoints[4][3]; /* square patrol loop around origin          */
    int patrol_wp_idx;            /* next waypoint index (0-3)                 */
    float idle_timer;             /* seconds remaining in idle                 */
    float investigate_target[3];  /* target world position when investigating  */
    float investigate_timer;      /* seconds remaining before returning        */
    float combat_target[3];       /* hostile target world position             */
    float combat_timer;           /* seconds remaining in combat pursuit       */
    NavGrid grid;                 /* embedded nav grid (no heap alloc needed)  */
    NavPath current_path;         /* current A* path                           */
    StateMachine fsm;             /* embedded FSM                              */
} WildlifeState;

/* ── FSM state callbacks ─────────────────────────────────────────────────── */

static void idle_enter(void *ctx)
{
    WildlifeState *w = (WildlifeState *)ctx;
    /* idle 3 to 7 seconds using the entity id as a cheap pseudo-random seed */
    float jitter = (float)((int)(w->position[0] * 7 + w->position[2] * 13) & 0xF);
    w->idle_timer = 3.0f + jitter * 0.25f;
}

static void idle_update(void *ctx, float dt)
{
    WildlifeState *w = (WildlifeState *)ctx;
    w->idle_timer -= dt;
    if (w->idle_timer <= 0.0f)
        fsm_trigger(&w->fsm, "start_patrol");
}

static void idle_exit(void *ctx)
{
    (void)ctx;
}

/* ── patrol ── */
static void patrol_enter(void *ctx)
{
    WildlifeState *w = (WildlifeState *)ctx;
    float *wp = w->patrol_waypoints[w->patrol_wp_idx];
    nav_find_path(&w->grid, w->position[0], w->position[2], wp[0], wp[2], &w->current_path);
}

static void patrol_update(void *ctx, float dt)
{
    WildlifeState *w = (WildlifeState *)ctx;
    int arrived = nav_move_along_path(&w->current_path, w->position, 3.5f, dt);
    if (arrived)
    {
        w->patrol_wp_idx = (w->patrol_wp_idx + 1) % 4;
        if (w->patrol_wp_idx == 0)
        {
            /* completed a full loop — brief idle */
            fsm_trigger(&w->fsm, "idle");
        }
        else
        {
            float *wp = w->patrol_waypoints[w->patrol_wp_idx];
            nav_find_path(&w->grid, w->position[0], w->position[2], wp[0], wp[2], &w->current_path);
        }
    }
}

static void patrol_exit(void *ctx)
{
    (void)ctx;
}

/* ── investigate ── */
static void investigate_enter(void *ctx)
{
    WildlifeState *w = (WildlifeState *)ctx;
    w->investigate_timer = 8.0f;
    nav_find_path(&w->grid, w->position[0], w->position[2], w->investigate_target[0],
                  w->investigate_target[2], &w->current_path);
}

static void investigate_update(void *ctx, float dt)
{
    WildlifeState *w = (WildlifeState *)ctx;
    w->investigate_timer -= dt;
    int arrived = nav_move_along_path(&w->current_path, w->position, 5.0f, dt);
    if (arrived || w->investigate_timer <= 0.0f)
        fsm_trigger(&w->fsm, "return");
}

static void investigate_exit(void *ctx)
{
    (void)ctx;
}

/* ── combat ── */
static void combat_enter(void *ctx)
{
    WildlifeState *w = (WildlifeState *)ctx;
    w->combat_timer = 6.0f;
    nav_find_path(&w->grid,
                  w->position[0],
                  w->position[2],
                  w->combat_target[0],
                  w->combat_target[2],
                  &w->current_path);
}

static void combat_update(void *ctx, float dt)
{
    WildlifeState *w = (WildlifeState *)ctx;
    int arrived = 0;

    w->combat_timer -= dt;
    arrived = nav_move_along_path(&w->current_path, w->position, 7.0f, dt);

    if (arrived || w->combat_timer <= 0.0f)
        fsm_trigger(&w->fsm, "return");
}

static void combat_exit(void *ctx)
{
    (void)ctx;
}

/* ── return ── */
static void return_enter(void *ctx)
{
    WildlifeState *w = (WildlifeState *)ctx;
    nav_find_path(&w->grid, w->position[0], w->position[2], w->patrol_origin[0],
                  w->patrol_origin[2], &w->current_path);
}

static void return_update(void *ctx, float dt)
{
    WildlifeState *w = (WildlifeState *)ctx;
    int arrived = nav_move_along_path(&w->current_path, w->position, 4.0f, dt);
    if (arrived)
        fsm_trigger(&w->fsm, "idle");
}

static void return_exit(void *ctx)
{
    (void)ctx;
}

/* ── ControllerInstance callbacks ─────────────────────────────────────────── */

static void wildlife_update(ControllerInstance *self, float dt)
{
    WildlifeState *w = (WildlifeState *)self->state;
    fsm_update(&w->fsm, dt);
    /* mirror position back to the controller (engine syncs → entity) */
    self->position[0] = w->position[0];
    self->position[1] = w->position[1];
    self->position[2] = w->position[2];
}

static void wildlife_signal(ControllerInstance *self, const char *signal, const void *data)
{
    WildlifeState *w = (WildlifeState *)self->state;
    if (strcmp(signal, "player_nearby") == 0 && data)
    {
        const float *p = (const float *)data;
        w->investigate_target[0] = p[0];
        w->investigate_target[1] = 0.0f;
        w->investigate_target[2] = p[2];
        fsm_trigger(&w->fsm, "investigate");
    }
    else if (strcmp(signal, "battle_engage") == 0 && data)
    {
        const float *p = (const float *)data;
        w->combat_target[0] = p[0];
        w->combat_target[1] = 0.0f;
        w->combat_target[2] = p[2];
        fsm_trigger(&w->fsm, "combat");
    }
    else if (strcmp(signal, "death") == 0)
    {
        fsm_trigger(&w->fsm, "idle");
    }
}

static void wildlife_destroy(ControllerInstance *self)
{
    if (self->state)
    {
        free(self->state);
        self->state = NULL;
    }
}

/* ── Factory ───────────────────────────────────────────────────────────────── */

static ControllerInstance *wildlife_factory(float x, float y, float z)
{
    ControllerInstance *inst = (ControllerInstance *)calloc(1, sizeof(ControllerInstance));
    WildlifeState *w = (WildlifeState *)calloc(1, sizeof(WildlifeState));

    w->position[0] = x;
    w->position[1] = y;
    w->position[2] = z;
    w->patrol_origin[0] = x;
    w->patrol_origin[1] = y;
    w->patrol_origin[2] = z;

    /* Patrol waypoints: clockwise square, 6 units per side */
    float offsets[4][2] = {{6, 0}, {6, 6}, {0, 6}, {0, 0}};
    for (int i = 0; i < 4; i++)
    {
        w->patrol_waypoints[i][0] = x + offsets[i][0];
        w->patrol_waypoints[i][1] = 0.0f;
        w->patrol_waypoints[i][2] = z + offsets[i][1];
    }
    w->patrol_wp_idx = 0;

    /* Inline nav grid (everything passable; obstacles added by world setup) */
    w->grid.cell_size = 1.0f;
    w->grid.origin[0] = 0.0f;
    w->grid.origin[1] = 0.0f;
    memset(w->grid.passable, 1, sizeof(w->grid.passable));

    /* FSM wiring */
    fsm_init(&w->fsm, w);
    int idle_idx = fsm_add_state(&w->fsm, "idle", idle_enter, idle_update, idle_exit);
    int pat_idx = fsm_add_state(&w->fsm, "patrol", patrol_enter, patrol_update, patrol_exit);
    int inv_idx = fsm_add_state(&w->fsm, "investigate", investigate_enter, investigate_update,
                                investigate_exit);
    int combat_idx = fsm_add_state(&w->fsm, "combat", combat_enter, combat_update, combat_exit);
    int ret_idx = fsm_add_state(&w->fsm, "return", return_enter, return_update, return_exit);

    fsm_add_transition(&w->fsm, idle_idx, pat_idx, "start_patrol");
    fsm_add_transition(&w->fsm, pat_idx, idle_idx, "idle");
    fsm_add_transition(&w->fsm, pat_idx, inv_idx, "investigate");
    fsm_add_transition(&w->fsm, idle_idx, inv_idx, "investigate");
    fsm_add_transition(&w->fsm, idle_idx, combat_idx, "combat");
    fsm_add_transition(&w->fsm, pat_idx, combat_idx, "combat");
    fsm_add_transition(&w->fsm, inv_idx, combat_idx, "combat");
    fsm_add_transition(&w->fsm, ret_idx, combat_idx, "combat");
    fsm_add_transition(&w->fsm, combat_idx, ret_idx, "return");
    fsm_add_transition(&w->fsm, inv_idx, ret_idx, "return");
    fsm_add_transition(&w->fsm, ret_idx, idle_idx, "idle");

    inst->state = w;
    inst->update = wildlife_update;
    inst->on_signal = wildlife_signal;
    inst->destroy = wildlife_destroy;
    strncpy(inst->type_name, "wildlife", sizeof(inst->type_name) - 1);
    inst->position[0] = x;
    inst->position[1] = y;
    inst->position[2] = z;

    return inst;
}

void wildlife_controller_register(void)
{
    controller_register("wildlife", wildlife_factory);
}

const char *wildlife_controller_debug_state_name(const ControllerInstance *controller)
{
    const WildlifeState *state = NULL;

    if (!controller)
        return NULL;

    if (strncmp(controller->type_name, "wildlife", sizeof(controller->type_name)) != 0)
        return NULL;

    if (!controller->state)
        return NULL;

    state = (const WildlifeState *)controller->state;
    return fsm_current_state_name(&state->fsm);
}
