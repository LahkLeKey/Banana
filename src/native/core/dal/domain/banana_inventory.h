#ifndef BANANA_DAL_INVENTORY_H
#define BANANA_DAL_INVENTORY_H

#include "../../domain/banana_inventory.h"

int banana_inventory_db_save_item(const BananaInventoryItem* item);
int banana_inventory_db_get_item(const char* inventory_id, BananaInventoryItem* item);
int banana_inventory_db_clear_items(void);

#endif