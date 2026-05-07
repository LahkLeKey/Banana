#ifndef BANANA_ENGINE_STATE_MACHINE_H
#define BANANA_ENGINE_STATE_MACHINE_H

#ifdef __cplusplus
extern "C" {
#endif

#define BANANA_FSM_MAX_STATES      16
#define BANANA_FSM_MAX_TRANSITIONS 32
#define BANANA_FSM_NAME_LEN        32

typedef struct FSMState {
    char name[BANANA_FSM_NAME_LEN];
    void (*on_enter)(void *ctx);
    void (*on_update)(void *ctx, float dt);
    void (*on_exit)(void *ctx);
} FSMState;

typedef struct FSMTransition {
    int from_state; /* index */
    int to_state;   /* index */
    char trigger[BANANA_FSM_NAME_LEN];
} FSMTransition;

typedef struct StateMachine {
    FSMState      states[BANANA_FSM_MAX_STATES];
    FSMTransition transitions[BANANA_FSM_MAX_TRANSITIONS];
    int state_count;
    int transition_count;
    int current_state; /* index */
    float time_in_state;
    void *context; /* passed to on_enter/on_update/on_exit */
} StateMachine;

void fsm_init(StateMachine *fsm, void *context);
int  fsm_add_state(StateMachine *fsm, const char *name,
                    void (*on_enter)(void*),
                    void (*on_update)(void*, float),
                    void (*on_exit)(void*));
void fsm_add_transition(StateMachine *fsm, int from, int to, const char *trigger);

/* Update current state (calls on_update). */
void fsm_update(StateMachine *fsm, float dt);

/* Fire a named trigger — finds matching transition, calls on_exit/on_enter. */
void fsm_trigger(StateMachine *fsm, const char *trigger);

const char *fsm_current_state_name(const StateMachine *fsm);

#ifdef __cplusplus
}
#endif

#endif /* BANANA_ENGINE_STATE_MACHINE_H */
