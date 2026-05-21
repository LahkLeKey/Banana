#ifndef BANANA_ENGINE_PLAYER_CONTROLLER_H
#define BANANA_ENGINE_PLAYER_CONTROLLER_H

#ifdef __cplusplus
extern "C"
{
#endif

#include "controller.h"

    void player_controller_register(void);
    void player_controller_set_move_input(ControllerInstance *self, float move_x, float move_z);
    int player_controller_get_move_input(const ControllerInstance *self, float *move_x,
                                         float *move_z);

#ifdef __cplusplus
}
#endif

#endif /* BANANA_ENGINE_PLAYER_CONTROLLER_H */