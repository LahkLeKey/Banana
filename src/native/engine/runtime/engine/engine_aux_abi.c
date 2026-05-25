#include "engine_aux_abi.h"

#include "../controller/attach/controller_attach.h"
#include "../controller/runtime/controller_runtime.h"
#include "../controller/sync/controller_sync.h"
#include "../world/world_metrics.h"
#include "../world/world_telemetry.h"

#include <string.h>

static int runtime_launch_gate_mode_from_label(const char *mode_label,
                                               banana_launch_gate_mode *out_mode)
{
    if (!mode_label || !out_mode)
        return -1;

    if (strcmp(mode_label, "development") == 0)
    {
        *out_mode = BANANA_LAUNCH_GATE_MODE_DEVELOPMENT;
        return 0;
    }

    if (strcmp(mode_label, "staging-uat") == 0)
    {
        *out_mode = BANANA_LAUNCH_GATE_MODE_STAGING_UAT;
        return 0;
    }

    if (strcmp(mode_label, "production-steam-package") == 0)
    {
        *out_mode = BANANA_LAUNCH_GATE_MODE_PRODUCTION_STEAM_PACKAGE;
        return 0;
    }

    return -1;
}

RuntimeEngineAuxContext runtime_engine_aux_context(World *world,
                                                   ControllerInstance **controllers,
                                                   int controller_count)
{
    RuntimeEngineAuxContext context;
    context.world = world;
    context.controllers = controllers;
    context.controller_count = controller_count;
    return context;
}

uint32_t runtime_engine_aux_controller_create(ControllerInstance **controllers,
                                              int max_active_controllers,
                                              int *inout_controller_count,
                                              const char *type,
                                              float x,
                                              float y,
                                              float z)
{
    return runtime_controller_create_and_register(controllers,
                                                  max_active_controllers,
                                                  inout_controller_count,
                                                  type,
                                                  x,
                                                  y,
                                                  z);
}

uint32_t runtime_engine_aux_controller_attach(RuntimeEngineAuxContext context,
                                              int max_active_controllers,
                                              int *inout_controller_count,
                                              uint32_t entity_id,
                                              const char *type)
{
    return runtime_controller_attach_to_entity(context.world,
                                               context.controllers,
                                               max_active_controllers,
                                               inout_controller_count,
                                               entity_id,
                                               type);
}

int runtime_engine_aux_controller_update(RuntimeEngineAuxContext context,
                                         uint32_t controller_id,
                                         float dt)
{
    return runtime_controller_update_by_id(context.controllers,
                                           context.controller_count,
                                           controller_id,
                                           dt);
}

int runtime_engine_aux_controller_signal(RuntimeEngineAuxContext context,
                                         uint32_t controller_id,
                                         const char *signal_name)
{
    return runtime_controller_signal_by_id(context.controllers,
                                           context.controller_count,
                                           controller_id,
                                           signal_name,
                                           NULL);
}

int runtime_engine_aux_entity_count(World *world)
{
    return runtime_world_entity_count(world);
}

float runtime_engine_aux_entity_x(World *world, int index)
{
    return runtime_world_entity_x(world, index);
}

float runtime_engine_aux_entity_z(World *world, int index)
{
    return runtime_world_entity_z(world, index);
}

int runtime_engine_aux_entity_state(RuntimeEngineAuxContext context, int index)
{
    return runtime_world_entity_state(context.world,
                                      index,
                                      context.controllers,
                                      context.controller_count);
}

int runtime_engine_aux_active_player_count(World *world)
{
    return runtime_world_active_player_count(world);
}

int runtime_engine_aux_parallel_sync_available(void)
{
    return runtime_controller_sync_parallel_available();
}

int runtime_engine_aux_launch_gate_policy_resolve(const char *mode_label,
                                                  banana_launch_gate_policy *out_policy)
{
    banana_launch_gate_mode mode = BANANA_LAUNCH_GATE_MODE_PRODUCTION_STEAM_PACKAGE;

    if (!out_policy)
        return -1;

    if (runtime_launch_gate_mode_from_label(mode_label, &mode) != 0)
        return -1;

    out_policy->mode = mode;
    out_policy->allow_non_steam_startup = 0;
    out_policy->allow_cached_verification = 0;
    out_policy->allow_override_context = 0;

    if (mode == BANANA_LAUNCH_GATE_MODE_DEVELOPMENT)
    {
        out_policy->allow_non_steam_startup = 1;
        out_policy->allow_cached_verification = 1;
        out_policy->allow_override_context = 1;
        return 0;
    }

    if (mode == BANANA_LAUNCH_GATE_MODE_STAGING_UAT)
    {
        out_policy->allow_cached_verification = 1;
        out_policy->allow_override_context = 1;
        return 0;
    }

    if (mode == BANANA_LAUNCH_GATE_MODE_PRODUCTION_STEAM_PACKAGE)
    {
        return 0;
    }

    return -1;
}

int runtime_engine_aux_launch_gate_decide(const banana_launch_gate_policy *policy,
                                          int has_steam_identity,
                                          int account_linked,
                                          int account_in_good_standing,
                                          int verification_fresh,
                                          int verification_available,
                                          banana_launch_gate_reason_code *out_reason_code,
                                          int *out_allow)
{
    if (!policy || !out_reason_code || !out_allow)
        return -1;

    *out_allow = 0;
    *out_reason_code = BANANA_LAUNCH_GATE_REASON_UNKNOWN_MODE;

    if (!has_steam_identity && !policy->allow_non_steam_startup)
    {
        *out_reason_code = BANANA_LAUNCH_GATE_REASON_STEAM_UNAVAILABLE;
        return 0;
    }

    if (!verification_available)
    {
        *out_reason_code = BANANA_LAUNCH_GATE_REASON_OFFLINE_UNVERIFIABLE;
        return 0;
    }

    if (!verification_fresh && !policy->allow_cached_verification)
    {
        *out_reason_code = BANANA_LAUNCH_GATE_REASON_STALE_SESSION;
        return 0;
    }

    if (!account_linked && !policy->allow_non_steam_startup)
    {
        *out_reason_code = BANANA_LAUNCH_GATE_REASON_UNLINKED_ACCOUNT;
        return 0;
    }

    if (!account_in_good_standing)
    {
        *out_reason_code = BANANA_LAUNCH_GATE_REASON_SUSPENDED_OR_RESTRICTED;
        return 0;
    }

    *out_allow = 1;
    *out_reason_code = BANANA_LAUNCH_GATE_REASON_OK;
    return 0;
}

int runtime_engine_aux_launch_gate_reason_is_remediable(banana_launch_gate_reason_code reason_code)
{
    if (reason_code == BANANA_LAUNCH_GATE_REASON_UNLINKED_ACCOUNT)
        return 1;

    if (reason_code == BANANA_LAUNCH_GATE_REASON_STALE_SESSION)
        return 1;

    if (reason_code == BANANA_LAUNCH_GATE_REASON_STEAM_UNAVAILABLE)
        return 1;

    if (reason_code == BANANA_LAUNCH_GATE_REASON_API_UNAVAILABLE)
        return 1;

    if (reason_code == BANANA_LAUNCH_GATE_REASON_OFFLINE_UNVERIFIABLE)
        return 1;

    return 0;
}