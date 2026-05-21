#include "ui.h"
#include <stdio.h>

/* Forward declare to avoid circular dependency */
extern int engine_player_get_resource(const char *resource_type);

/* Inventory widget - stub for now, will render via callbacks */
int ui_inventory_panel_impl(UIContext *ctx, float x, float y) {
    if (!ctx) return 0;
    
    /* Inventory rendering will be added after core UI system is validated */
    return 0;
}
