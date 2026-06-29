#include "ui.h"

/* Forward declarations */
extern int engine_player_get_resource(const char *resource_type);
extern int engine_npc_merchant_get_price(int npc_id, const char *item_type);

/* Merchant widget - stub for now, will render via callbacks */
int ui_merchant_dialog_impl(UIContext *ctx, float x, float y, int npc_id) {
    if (!ctx) return 0;
    
    /* Merchant dialog rendering will be added after core UI system is validated */
    return 0;
}
