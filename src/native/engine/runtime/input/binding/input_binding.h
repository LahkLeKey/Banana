/**
 * @file input_binding.h
 * @brief Input event routing and controller interaction binding
 *
 * Maps user interactions (clicks, proximity, trade) to controller signals.
 * Handles spatial queries to determine which controller(s) should receive events.
 *
 * Design: Input events (from network or local input) are routed through this module
 * to find the target controller and dispatch the appropriate signal.
 */

#ifndef BANANA_ENGINE_INPUT_BINDING_H
#define BANANA_ENGINE_INPUT_BINDING_H

#ifdef __cplusplus
extern "C"
{
#endif

#include <stdint.h>

    /**
     * Input event types that controllers can respond to
     */
    typedef enum
    {
        INPUT_EVENT_CLICK = 1,           /* World click at position */
        INPUT_EVENT_PROXIMITY_ENTER = 2, /* Player entered NPC proximity */
        INPUT_EVENT_PROXIMITY_EXIT = 3,  /* Player left NPC proximity */
        INPUT_EVENT_TRADE_REQUEST = 4,   /* Player initiated trade with NPC */
    } InputEventType;

    /**
     * Click event data
     */
    typedef struct
    {
        float world_x, world_z;       /* Click position in world coordinates */
        const char *player_guid;      /* Who clicked */
        int object_id;                /* Target object ID (if any) */
    } ClickEventData;

    /**
     * Proximity event data
     */
    typedef struct
    {
        const char *player_guid;
        float player_x, player_z;
    } ProximityEventData;

    /**
     * Trade event data
     */
    typedef struct
    {
        const char *player_guid;
        int merchant_id;
        const char *trade_type; /* e.g., "buy", "sell" */
    } TradeEventData;

    /**
     * Initialize the input binding system.
     * Must be called once at engine startup.
     */
    int input_binding_init(void);

    /**
     * Register an input-to-controller binding callback.
     * This allows custom routing logic for specific input types.
     *
     * input_type: INPUT_EVENT_*
     * callback: function that receives the event data and returns target controller ID(s)
     */
    int input_binding_register_callback(InputEventType input_type,
                                        uint32_t (*callback)(const void *event_data,
                                                             uint32_t *controller_ids,
                                                             int max_controllers));

    /**
     * Dispatch a click event.
     * Queries interactive objects and controllers in the clicked region,
     * dispatches the appropriate signal to each affected controller.
     */
    int input_binding_dispatch_click(float world_x, float world_z, const char *player_guid);

    /**
     * Dispatch a proximity event (player entering/exiting NPC radius).
     */
    int input_binding_dispatch_proximity(const char *player_guid, float player_x, float player_z,
                                         int is_entering);

    /**
     * Dispatch a trade request event.
     */
    int input_binding_dispatch_trade_request(const char *player_guid, int merchant_id,
                                              const char *trade_type);

    /**
     * Cleanup the input binding system.
     * Called at engine shutdown.
     */
    void input_binding_cleanup(void);

#ifdef __cplusplus
}
#endif

#endif /* BANANA_ENGINE_INPUT_BINDING_H */
