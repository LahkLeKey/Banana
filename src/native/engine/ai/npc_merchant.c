/**
 * @file npc_merchant.c
 * @brief Merchant NPC controller implementation
 *
 * Manages merchant NPCs that trade with players.
 */

#include "npc_merchant.h"

#include <stdlib.h>
#include <string.h>

/* Maximum merchants in a world */
#define MAX_MERCHANTS 32

/* Maximum inventory items per merchant */
#define MAX_INVENTORY_ITEMS 16

/* Merchant registry */
static struct {
  int count;
  MerchantNPC merchants[MAX_MERCHANTS];
} g_merchant_registry = {0};

int npc_merchant_init(void) {
  g_merchant_registry.count = 0;
  return 0;
}

int npc_merchant_register(int controller_id, float market_x, float market_z,
                          int initial_gold) {
  if (g_merchant_registry.count >= MAX_MERCHANTS) {
    return -1; /* Registry full */
  }

  MerchantNPC *merchant = &g_merchant_registry.merchants[g_merchant_registry.count++];
  merchant->controller_id = controller_id;
  merchant->market_x = market_x;
  merchant->market_z = market_z;
  merchant->gold_balance = initial_gold;
  merchant->inventory_count = 0;
  merchant->inventory =
      (MerchantInventoryItem *)malloc(MAX_INVENTORY_ITEMS * sizeof(MerchantInventoryItem));

  if (!merchant->inventory) {
    g_merchant_registry.count--;
    return -1;
  }

  merchant->last_activity_at = 0;
  return g_merchant_registry.count - 1; /* Return merchant ID */
}

int npc_merchant_add_inventory_item(int merchant_id, const char *item_name,
                                     int quantity, int max_stock, int price_gold) {
  if (merchant_id < 0 || merchant_id >= g_merchant_registry.count) {
    return -1;
  }

  MerchantNPC *merchant = &g_merchant_registry.merchants[merchant_id];

  if (merchant->inventory_count >= MAX_INVENTORY_ITEMS) {
    return -1; /* Inventory full */
  }

  MerchantInventoryItem *item = &merchant->inventory[merchant->inventory_count++];
  item->item_name = item_name;
  item->quantity = quantity;
  item->max_stock = max_stock;
  item->price_gold = price_gold;

  return 0;
}

static MerchantInventoryItem *npc_merchant_find_item(int merchant_id,
                                                      const char *item_name) {
  if (merchant_id < 0 || merchant_id >= g_merchant_registry.count) {
    return NULL;
  }

  MerchantNPC *merchant = &g_merchant_registry.merchants[merchant_id];

  for (int i = 0; i < merchant->inventory_count; i++) {
    if (strcmp(merchant->inventory[i].item_name, item_name) == 0) {
      return &merchant->inventory[i];
    }
  }

  return NULL;
}

MerchantTradeResult npc_merchant_trade_buy(int merchant_id, const char *item_name,
                                            int quantity, int *player_gold) {
  if (!player_gold) {
    return MERCHANT_TRADE_INVALID_MERCHANT;
  }

  MerchantInventoryItem *item = npc_merchant_find_item(merchant_id, item_name);
  if (!item) {
    return MERCHANT_TRADE_INVALID_ITEM;
  }

  int total_cost = item->price_gold * quantity;

  if (*player_gold < total_cost) {
    return MERCHANT_TRADE_INSUFFICIENT_GOLD;
  }

  if (item->quantity < quantity) {
    return MERCHANT_TRADE_INSUFFICIENT_STOCK;
  }

  /* Execute trade */
  item->quantity -= quantity;
  *player_gold -= total_cost;

  if (merchant_id >= 0 && merchant_id < g_merchant_registry.count) {
    g_merchant_registry.merchants[merchant_id].gold_balance += total_cost;
    g_merchant_registry.merchants[merchant_id].last_activity_at = 0; /* Update timestamp
                                                                         if needed */
  }

  return MERCHANT_TRADE_OK;
}

MerchantTradeResult npc_merchant_trade_sell(int merchant_id, const char *item_name,
                                             int quantity, int *player_gold) {
  if (!player_gold) {
    return MERCHANT_TRADE_INVALID_MERCHANT;
  }

  MerchantInventoryItem *item = npc_merchant_find_item(merchant_id, item_name);
  if (!item) {
    return MERCHANT_TRADE_INVALID_ITEM;
  }

  int total_revenue = item->price_gold * quantity;

  if (item->quantity + quantity > item->max_stock) {
    return MERCHANT_TRADE_INSUFFICIENT_STOCK; /* Merchant inventory full */
  }

  /* Execute trade */
  item->quantity += quantity;
  *player_gold += total_revenue;

  if (merchant_id >= 0 && merchant_id < g_merchant_registry.count) {
    MerchantNPC *merchant = &g_merchant_registry.merchants[merchant_id];
    merchant->gold_balance -= total_revenue;
    merchant->last_activity_at = 0; /* Update timestamp if needed */

    /* Prevent merchant from going negative */
    if (merchant->gold_balance < 0) {
      merchant->gold_balance = 0;
    }
  }

  return MERCHANT_TRADE_OK;
}

int npc_merchant_get_item_price(int merchant_id, const char *item_name,
                                 int *out_quantity, int *out_price) {
  if (!out_quantity || !out_price) {
    return -1;
  }

  MerchantInventoryItem *item = npc_merchant_find_item(merchant_id, item_name);
  if (!item) {
    return -1;
  }

  *out_quantity = item->quantity;
  *out_price = item->price_gold;
  return 0;
}

void npc_merchant_tick(int64_t current_time_ms) {
  /* Placeholder for future merchant AI behavior:
   * - Restock items based on time
   * - Adjust prices dynamically
   * - Generate trade signals for nearby players
   */
  (void)current_time_ms; /* Suppress unused parameter warning */
}

int npc_merchant_count(void) {
  return g_merchant_registry.count;
}

int npc_merchant_get(int merchant_id, MerchantNPC *out_merchant) {
  if (!out_merchant) {
    return -1;
  }

  if (merchant_id < 0 || merchant_id >= g_merchant_registry.count) {
    return -1;
  }

  *out_merchant = g_merchant_registry.merchants[merchant_id];
  return 0;
}

void npc_merchant_clear(void) {
  for (int i = 0; i < g_merchant_registry.count; i++) {
    if (g_merchant_registry.merchants[i].inventory) {
      free(g_merchant_registry.merchants[i].inventory);
    }
  }
  g_merchant_registry.count = 0;
}

void npc_merchant_shutdown(void) {
  npc_merchant_clear();
}
