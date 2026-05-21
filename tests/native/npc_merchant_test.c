/**
 * @file npc_merchant_test.c
 * @brief Unit tests for merchant NPC system
 */

#include "ai/npc_merchant.h"

#include <assert.h>
#include <stdio.h>

/* Test: Initialize merchant system */
int test_npc_merchant_init(void) {
  int status = npc_merchant_init();
  assert(status == 0);
  assert(npc_merchant_count() == 0);
  printf("✓ Test: init\n");
  return 0;
}

/* Test: Register merchant */
int test_npc_merchant_register(void) {
  npc_merchant_init();

  int merchant_id = npc_merchant_register(0, 10.0f, 10.0f, 100);
  assert(merchant_id == 0);
  assert(npc_merchant_count() == 1);

  MerchantNPC merchant;
  int status = npc_merchant_get(merchant_id, &merchant);
  assert(status == 0);
  assert(merchant.market_x == 10.0f);
  assert(merchant.market_z == 10.0f);
  assert(merchant.gold_balance == 100);

  printf("✓ Test: register\n");
  return 0;
}

/* Test: Add inventory items */
int test_npc_merchant_add_inventory(void) {
  npc_merchant_init();

  int merchant_id = npc_merchant_register(0, 10.0f, 10.0f, 500);

  int status = npc_merchant_add_inventory_item(merchant_id, "wood", 50, 100, 10);
  assert(status == 0);

  status = npc_merchant_add_inventory_item(merchant_id, "ore", 20, 50, 25);
  assert(status == 0);

  MerchantNPC merchant;
  npc_merchant_get(merchant_id, &merchant);
  assert(merchant.inventory_count == 2);

  printf("✓ Test: add_inventory\n");
  return 0;
}

/* Test: Get item price */
int test_npc_merchant_get_item_price(void) {
  npc_merchant_init();

  int merchant_id = npc_merchant_register(0, 10.0f, 10.0f, 500);
  npc_merchant_add_inventory_item(merchant_id, "wood", 50, 100, 10);

  int quantity, price;
  int status = npc_merchant_get_item_price(merchant_id, "wood", &quantity, &price);

  assert(status == 0);
  assert(quantity == 50);
  assert(price == 10);

  printf("✓ Test: get_item_price\n");
  return 0;
}

/* Test: Trade buy - success */
int test_npc_merchant_trade_buy_success(void) {
  npc_merchant_init();

  int merchant_id = npc_merchant_register(0, 10.0f, 10.0f, 500);
  npc_merchant_add_inventory_item(merchant_id, "wood", 50, 100, 10);

  int player_gold = 100;
  MerchantTradeResult result = npc_merchant_trade_buy(merchant_id, "wood", 5, &player_gold);

  assert(result == MERCHANT_TRADE_OK);
  assert(player_gold == 50); /* 100 - (5 * 10) */

  MerchantNPC merchant;
  npc_merchant_get(merchant_id, &merchant);
  assert(merchant.gold_balance == 550); /* 500 + (5 * 10) */

  int quantity, price;
  npc_merchant_get_item_price(merchant_id, "wood", &quantity, &price);
  assert(quantity == 45); /* 50 - 5 */

  printf("✓ Test: trade_buy_success\n");
  return 0;
}

/* Test: Trade buy - insufficient gold */
int test_npc_merchant_trade_buy_insufficient_gold(void) {
  npc_merchant_init();

  int merchant_id = npc_merchant_register(0, 10.0f, 10.0f, 500);
  npc_merchant_add_inventory_item(merchant_id, "wood", 50, 100, 10);

  int player_gold = 30;
  MerchantTradeResult result = npc_merchant_trade_buy(merchant_id, "wood", 5, &player_gold);

  assert(result == MERCHANT_TRADE_INSUFFICIENT_GOLD);
  assert(player_gold == 30); /* Unchanged */

  int quantity;
  int price;
  npc_merchant_get_item_price(merchant_id, "wood", &quantity, &price);
  assert(quantity == 50); /* Unchanged */

  printf("✓ Test: trade_buy_insufficient_gold\n");
  return 0;
}

/* Test: Trade buy - insufficient stock */
int test_npc_merchant_trade_buy_insufficient_stock(void) {
  npc_merchant_init();

  int merchant_id = npc_merchant_register(0, 10.0f, 10.0f, 500);
  npc_merchant_add_inventory_item(merchant_id, "wood", 5, 100, 10);

  int player_gold = 1000;
  MerchantTradeResult result = npc_merchant_trade_buy(merchant_id, "wood", 10, &player_gold);

  assert(result == MERCHANT_TRADE_INSUFFICIENT_STOCK);
  assert(player_gold == 1000); /* Unchanged */

  printf("✓ Test: trade_buy_insufficient_stock\n");
  return 0;
}

/* Test: Trade buy - invalid item */
int test_npc_merchant_trade_buy_invalid_item(void) {
  npc_merchant_init();

  int merchant_id = npc_merchant_register(0, 10.0f, 10.0f, 500);
  npc_merchant_add_inventory_item(merchant_id, "wood", 50, 100, 10);

  int player_gold = 1000;
  MerchantTradeResult result =
      npc_merchant_trade_buy(merchant_id, "iron", 5, &player_gold);

  assert(result == MERCHANT_TRADE_INVALID_ITEM);
  assert(player_gold == 1000); /* Unchanged */

  printf("✓ Test: trade_buy_invalid_item\n");
  return 0;
}

/* Test: Trade sell - success */
int test_npc_merchant_trade_sell_success(void) {
  npc_merchant_init();

  int merchant_id = npc_merchant_register(0, 10.0f, 10.0f, 500);
  npc_merchant_add_inventory_item(merchant_id, "wood", 50, 100, 10);

  int player_gold = 0;
  MerchantTradeResult result = npc_merchant_trade_sell(merchant_id, "wood", 5, &player_gold);

  assert(result == MERCHANT_TRADE_OK);
  assert(player_gold == 50); /* 0 + (5 * 10) */

  MerchantNPC merchant;
  npc_merchant_get(merchant_id, &merchant);
  assert(merchant.gold_balance == 450); /* 500 - (5 * 10) */

  int quantity, price;
  npc_merchant_get_item_price(merchant_id, "wood", &quantity, &price);
  assert(quantity == 55); /* 50 + 5 */

  printf("✓ Test: trade_sell_success\n");
  return 0;
}

/* Test: Trade sell - merchant inventory full */
int test_npc_merchant_trade_sell_merchant_full(void) {
  npc_merchant_init();

  int merchant_id = npc_merchant_register(0, 10.0f, 10.0f, 500);
  npc_merchant_add_inventory_item(merchant_id, "wood", 95, 100, 10); /* Max is 100 */

  int player_gold = 0;
  MerchantTradeResult result = npc_merchant_trade_sell(merchant_id, "wood", 10, &player_gold);

  assert(result == MERCHANT_TRADE_INSUFFICIENT_STOCK); /* Merchant inventory full */
  assert(player_gold == 0);                             /* Unchanged */

  printf("✓ Test: trade_sell_merchant_full\n");
  return 0;
}

/* Test: Trade sell - merchant cannot afford payout */
int test_npc_merchant_trade_sell_insufficient_merchant_gold(void) {
  npc_merchant_init();

  int merchant_id = npc_merchant_register(0, 10.0f, 10.0f, 20);
  npc_merchant_add_inventory_item(merchant_id, "wood", 50, 100, 10);

  int player_gold = 0;
  MerchantTradeResult result = npc_merchant_trade_sell(merchant_id, "wood", 3, &player_gold);

  assert(result == MERCHANT_TRADE_INSUFFICIENT_GOLD);
  assert(player_gold == 0); /* Unchanged */

  MerchantNPC merchant;
  npc_merchant_get(merchant_id, &merchant);
  assert(merchant.gold_balance == 20); /* Unchanged */

  int quantity, price;
  npc_merchant_get_item_price(merchant_id, "wood", &quantity, &price);
  assert(quantity == 50); /* Unchanged */

  printf("✓ Test: trade_sell_insufficient_merchant_gold\n");
  return 0;
}

/* Test: Trade sell - invalid quantity */
int test_npc_merchant_trade_sell_invalid_quantity(void) {
  npc_merchant_init();

  int merchant_id = npc_merchant_register(0, 10.0f, 10.0f, 500);
  npc_merchant_add_inventory_item(merchant_id, "wood", 50, 100, 10);

  int player_gold = 0;
  MerchantTradeResult result = npc_merchant_trade_sell(merchant_id, "wood", 0, &player_gold);

  assert(result == MERCHANT_TRADE_INVALID_ITEM);
  assert(player_gold == 0);

  printf("✓ Test: trade_sell_invalid_quantity\n");
  return 0;
}

/* Test: Multiple merchants */
int test_npc_merchant_multiple(void) {
  npc_merchant_init();

  int merchant1 = npc_merchant_register(0, 10.0f, 10.0f, 100);
  int merchant2 = npc_merchant_register(1, 20.0f, 20.0f, 200);

  assert(npc_merchant_count() == 2);

  npc_merchant_add_inventory_item(merchant1, "wood", 50, 100, 10);
  npc_merchant_add_inventory_item(merchant2, "ore", 30, 50, 25);

  MerchantNPC m1, m2;
  npc_merchant_get(merchant1, &m1);
  npc_merchant_get(merchant2, &m2);

  assert(m1.gold_balance == 100);
  assert(m2.gold_balance == 200);
  assert(m1.inventory_count == 1);
  assert(m2.inventory_count == 1);

  printf("✓ Test: multiple_merchants\n");
  return 0;
}

/* Test: Clear system */
int test_npc_merchant_clear(void) {
  npc_merchant_init();

  for (int i = 0; i < 5; i++) {
    npc_merchant_register(i, 10.0f + i, 10.0f + i, 100 + i * 50);
  }

  assert(npc_merchant_count() == 5);

  npc_merchant_clear();
  assert(npc_merchant_count() == 0);

  printf("✓ Test: clear\n");
  return 0;
}

int main(void) {
  printf("Running merchant NPC tests...\n\n");

  int failed = 0;

  failed += test_npc_merchant_init();
  failed += test_npc_merchant_register();
  failed += test_npc_merchant_add_inventory();
  failed += test_npc_merchant_get_item_price();
  failed += test_npc_merchant_trade_buy_success();
  failed += test_npc_merchant_trade_buy_insufficient_gold();
  failed += test_npc_merchant_trade_buy_insufficient_stock();
  failed += test_npc_merchant_trade_buy_invalid_item();
  failed += test_npc_merchant_trade_sell_success();
  failed += test_npc_merchant_trade_sell_merchant_full();
  failed += test_npc_merchant_trade_sell_insufficient_merchant_gold();
  failed += test_npc_merchant_trade_sell_invalid_quantity();
  failed += test_npc_merchant_multiple();
  failed += test_npc_merchant_clear();

  printf("\n%s\n", failed == 0 ? "All tests passed! ✓" : "Some tests failed! ✗");
  return failed;
}
