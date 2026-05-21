/**
 * @file npc_merchant.h
 * @brief Merchant NPC controller for trading gameplay
 *
 * Merchant NPCs are stationary traders in the world that buy and sell resources.
 * They track their own inventory and respond to player proximity signals.
 */

#ifndef BANANA_ENGINE_NPC_MERCHANT_H
#define BANANA_ENGINE_NPC_MERCHANT_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Merchant trade request result codes
 */
typedef enum {
  MERCHANT_TRADE_OK = 0,           // Trade succeeded
  MERCHANT_TRADE_INSUFFICIENT_GOLD = 1,  // Player lacks gold
  MERCHANT_TRADE_INSUFFICIENT_STOCK = 2, // Merchant out of stock
  MERCHANT_TRADE_INVALID_ITEM = 3,       // Unknown item type
  MERCHANT_TRADE_INVALID_MERCHANT = 4,   // Merchant not found
} MerchantTradeResult;

/**
 * Merchant inventory item
 */
typedef struct {
  const char *item_name;  // e.g., "wood", "ore", "potion"
  int quantity;           // Current stock
  int price_gold;         // Price per unit in gold
  int max_stock;          // Maximum inventory capacity
} MerchantInventoryItem;

/**
 * Merchant NPC state
 */
typedef struct {
  int controller_id;           // Linked to controller registry
  float market_x, market_z;    // Stationary position in world
  int gold_balance;            // Merchant's working capital
  MerchantInventoryItem *inventory;
  int inventory_count;
  int64_t last_activity_at;    // Timestamp of last player interaction
} MerchantNPC;

/**
 * Initialize merchant NPC system
 * @return Status code (0 = success)
 */
int npc_merchant_init(void);

/**
 * Register a new merchant in the world
 * @param controller_id ID from controller registry
 * @param market_x X position where merchant stands
 * @param market_z Z position where merchant stands
 * @param initial_gold Starting gold for trades
 * @return Merchant ID on success, -1 on error
 */
int npc_merchant_register(int controller_id, float market_x, float market_z,
                          int initial_gold);

/**
 * Add inventory item to merchant
 * @param merchant_id Merchant ID
 * @param item_name Item identifier
 * @param quantity Initial quantity
 * @param max_stock Maximum stock limit
 * @param price_gold Price per unit
 * @return 0 on success, -1 on error
 */
int npc_merchant_add_inventory_item(int merchant_id, const char *item_name,
                                     int quantity, int max_stock, int price_gold);

/**
 * Process a merchant trade request (buying from merchant)
 * Player gold → Merchant item
 * @param merchant_id Merchant to buy from
 * @param item_name Item to purchase
 * @param quantity Number of units
 * @param player_gold [in/out] Player's current gold; decremented on success
 * @return Trade result code (see MerchantTradeResult)
 */
MerchantTradeResult npc_merchant_trade_buy(int merchant_id, const char *item_name,
                                            int quantity, int *player_gold);

/**
 * Process a merchant trade request (selling to merchant)
 * Player item → Merchant gold
 * @param merchant_id Merchant to sell to
 * @param item_name Item to sell
 * @param quantity Number of units
 * @param player_gold [in/out] Player's current gold; incremented on success
 * @return Trade result code
 */
MerchantTradeResult npc_merchant_trade_sell(int merchant_id, const char *item_name,
                                             int quantity, int *player_gold);

/**
 * Get merchant's current inventory status
 * @param merchant_id Merchant ID
 * @param item_name Item to query
 * @param out_quantity [out] Current quantity in stock
 * @param out_price [out] Price per unit
 * @return 0 on success, -1 if item not found
 */
int npc_merchant_get_item_price(int merchant_id, const char *item_name,
                                 int *out_quantity, int *out_price);

/**
 * Tick merchant NPCs (update activity timers, generate signals)
 * @param current_time_ms Current time in milliseconds
 */
void npc_merchant_tick(int64_t current_time_ms);

/**
 * Get count of active merchants
 * @return Number of merchant NPCs
 */
int npc_merchant_count(void);

/**
 * Get merchant by ID
 * @param merchant_id Merchant ID
 * @param out_merchant [out] Merchant state
 * @return 0 on success, -1 if not found
 */
int npc_merchant_get(int merchant_id, MerchantNPC *out_merchant);

/**
 * Clear all merchant NPCs (typically on engine reset)
 */
void npc_merchant_clear(void);

/**
 * Shutdown merchant NPC system
 */
void npc_merchant_shutdown(void);

#ifdef __cplusplus
}
#endif

#endif // BANANA_ENGINE_NPC_MERCHANT_H
