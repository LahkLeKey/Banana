#include "state_machine.h"
#include <string.h>
#include <stdio.h>

void fsm_init(StateMachine *fsm, void *context) {
    memset(fsm, 0, sizeof(StateMachine));
    fsm->current_state  = -1;
    fsm->time_in_state  = 0.f;
    fsm->context        = context;
}

int fsm_add_state(StateMachine *fsm, const char *name,
                   void (*on_enter)(void*),
                   void (*on_update)(void*, float),
                   void (*on_exit)(void*)) {
    if (fsm->state_count >= BANANA_FSM_MAX_STATES) {
        fprintf(stderr, "[engine/fsm] max states reached\n");
        return -1;
    }
    int idx = fsm->state_count++;
    strncpy(fsm->states[idx].name, name, BANANA_FSM_NAME_LEN - 1);
    fsm->states[idx].on_enter  = on_enter;
    fsm->states[idx].on_update = on_update;
    fsm->states[idx].on_exit   = on_exit;
    /* First state added becomes initial state */
    if (fsm->current_state == -1) {
        fsm->current_state = idx;
        if (on_enter) on_enter(fsm->context);
    }
    return idx;
}

void fsm_add_transition(StateMachine *fsm, int from, int to, const char *trigger) {
    if (fsm->transition_count >= BANANA_FSM_MAX_TRANSITIONS) return;
    int idx = fsm->transition_count++;
    fsm->transitions[idx].from_state = from;
    fsm->transitions[idx].to_state   = to;
    strncpy(fsm->transitions[idx].trigger, trigger, BANANA_FSM_NAME_LEN - 1);
}

void fsm_update(StateMachine *fsm, float dt) {
    fsm->time_in_state += dt;
    int s = fsm->current_state;
    if (s >= 0 && fsm->states[s].on_update)
        fsm->states[s].on_update(fsm->context, dt);
}

void fsm_trigger(StateMachine *fsm, const char *trigger) {
    for (int i = 0; i < fsm->transition_count; i++) {
        FSMTransition *t = &fsm->transitions[i];
        if (t->from_state == fsm->current_state &&
            strncmp(t->trigger, trigger, BANANA_FSM_NAME_LEN) == 0) {
            int from = fsm->current_state;
            int to   = t->to_state;
            if (fsm->states[from].on_exit)   fsm->states[from].on_exit(fsm->context);
            fsm->current_state = to;
            fsm->time_in_state = 0.f;
            if (fsm->states[to].on_enter) fsm->states[to].on_enter(fsm->context);
            return;
        }
    }
}

const char *fsm_current_state_name(const StateMachine *fsm) {
    if (fsm->current_state < 0) return "none";
    return fsm->states[fsm->current_state].name;
}
