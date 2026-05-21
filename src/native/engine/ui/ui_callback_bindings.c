#include "../engine.h"
#include "ui.h"
#include <stdint.h>

/* ─────────────────────────────────────────────────────────────────────────
   UI Callback Bindings
   
   Wires UI widget interactions (button clicks, menu selections) directly
   to engine functions. These callbacks are passed to UI elements and
   invoked when the user interacts with them.
   ───────────────────────────────────────────────────────────────────────── */

/* Callback: Buy wood from merchant */
static int ui_callback_buy_wood(void *user_data) {
    int npc_id = (int)(intptr_t)user_data;
    return engine_npc_merchant_trade_buy(npc_id, "wood", 1);
}

/* Callback: Buy ore from merchant */
static int ui_callback_buy_ore(void *user_data) {
    int npc_id = (int)(intptr_t)user_data;
    return engine_npc_merchant_trade_buy(npc_id, "ore", 1);
}

/* Callback: Sell wood to merchant */
static int ui_callback_sell_wood(void *user_data) {
    int npc_id = (int)(intptr_t)user_data;
    return engine_npc_merchant_trade_sell(npc_id, "wood", 1);
}

/* Callback: Sell ore to merchant */
static int ui_callback_sell_ore(void *user_data) {
    int npc_id = (int)(intptr_t)user_data;
    return engine_npc_merchant_trade_sell(npc_id, "ore", 1);
}

/* Forward declaration: engine.c manages global UI context */
extern UIContext *g_ui_context;

/* Get UI callback for a specific trade action */
UICallback ui_get_trade_callback(int trade_action, int npc_id) {
    switch (trade_action) {
        case 0: /* BUY_WOOD */
            return ui_callback_buy_wood;
        case 1: /* BUY_ORE */
            return ui_callback_buy_ore;
        case 2: /* SELL_WOOD */
            return ui_callback_sell_wood;
        case 3: /* SELL_ORE */
            return ui_callback_sell_ore;
        default:
            return NULL;
    }
}

/* Get user data for trade callback (NPC ID) */
void *ui_get_trade_callback_data(int npc_id) {
    return (void *)(intptr_t)npc_id;
}
