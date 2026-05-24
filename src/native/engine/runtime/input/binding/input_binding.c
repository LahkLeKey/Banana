/**
 * @file input_binding.c
 * @brief Input event routing and controller interaction binding implementation
 *
 * Routes user interactions to appropriate controllers based on spatial queries.
 */

#include "input_binding.h"
#include "../../../ai/controller.h"

#include <stdlib.h>
#include <string.h>

/* Maximum input callbacks per event type */
#define MAX_INPUT_CALLBACKS 8

/* Callback registry */
typedef struct
{
    uint32_t (*callback)(const void *event_data, uint32_t *controller_ids, int max_controllers);
} InputCallback;

static struct
{
    InputCallback callbacks[INPUT_EVENT_TRADE_REQUEST + 1][MAX_INPUT_CALLBACKS];
    int callback_counts[INPUT_EVENT_TRADE_REQUEST + 1];
} g_input_binding = {0};

int input_binding_init(void)
{
    memset(&g_input_binding, 0, sizeof(g_input_binding));
    return 0;
}

int input_binding_register_callback(InputEventType input_type,
                                    uint32_t (*callback)(const void *event_data,
                                                         uint32_t *controller_ids,
                                                         int max_controllers))
{
    if (input_type < INPUT_EVENT_CLICK || input_type > INPUT_EVENT_TRADE_REQUEST)
    {
        return -1;
    }

    if (!callback)
    {
        return -1;
    }

    int idx = g_input_binding.callback_counts[input_type];
    if (idx >= MAX_INPUT_CALLBACKS)
    {
        return -1; /* Too many callbacks */
    }

    g_input_binding.callbacks[input_type][idx].callback = callback;
    g_input_binding.callback_counts[input_type]++;
    return 0;
}

int input_binding_dispatch_click(float world_x, float world_z, const char *player_guid)
{
    if (!player_guid)
    {
        return -1;
    }

    ClickEventData event = {
        .world_x = world_x,
        .world_z = world_z,
        .player_guid = player_guid,
        .object_id = 0,
    };

    uint32_t controller_ids[32];
    int dispatched = 0;

    /* Call registered callbacks for click events */
    int callback_count = g_input_binding.callback_counts[INPUT_EVENT_CLICK];
    for (int i = 0; i < callback_count; i++)
    {
        InputCallback *cb = &g_input_binding.callbacks[INPUT_EVENT_CLICK][i];
        uint32_t count = cb->callback(&event, controller_ids, 32);

        /* Signal each affected controller */
        for (uint32_t j = 0; j < count && j < 32; j++)
        {
            /* Controller signal dispatch would happen here with engine world context */
            dispatched++;
        }
    }

    return dispatched;
}

int input_binding_dispatch_proximity(const char *player_guid, float player_x, float player_z,
                                     int is_entering)
{
    if (!player_guid)
    {
        return -1;
    }

    ProximityEventData event = {
        .player_guid = player_guid,
        .player_x = player_x,
        .player_z = player_z,
    };

    InputEventType event_type = is_entering ? INPUT_EVENT_PROXIMITY_ENTER : INPUT_EVENT_PROXIMITY_EXIT;

    uint32_t controller_ids[32];
    int dispatched = 0;

    int callback_count = g_input_binding.callback_counts[event_type];
    for (int i = 0; i < callback_count; i++)
    {
        InputCallback *cb = &g_input_binding.callbacks[event_type][i];
        uint32_t count = cb->callback(&event, controller_ids, 32);

        for (uint32_t j = 0; j < count && j < 32; j++)
        {
            dispatched++;
        }
    }

    return dispatched;
}

int input_binding_dispatch_trade_request(const char *player_guid, int merchant_id,
                                          const char *trade_type)
{
    if (!player_guid || !trade_type)
    {
        return -1;
    }

    TradeEventData event = {
        .player_guid = player_guid,
        .merchant_id = merchant_id,
        .trade_type = trade_type,
    };

    uint32_t controller_ids[32];
    int dispatched = 0;

    int callback_count = g_input_binding.callback_counts[INPUT_EVENT_TRADE_REQUEST];
    for (int i = 0; i < callback_count; i++)
    {
        InputCallback *cb = &g_input_binding.callbacks[INPUT_EVENT_TRADE_REQUEST][i];
        uint32_t count = cb->callback(&event, controller_ids, 32);

        for (uint32_t j = 0; j < count && j < 32; j++)
        {
            dispatched++;
        }
    }

    return dispatched;
}

void input_binding_cleanup(void)
{
    memset(&g_input_binding, 0, sizeof(g_input_binding));
}
