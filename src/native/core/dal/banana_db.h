#ifndef BANANA_DB_H
#define BANANA_DB_H

#include <stddef.h>

#include "../domain/banana_inventory.h"
#include "../domain/banana_lifecycle.h"
#include "../domain/banana_processing.h"
#include "../domain/banana_supply_chain.h"

typedef enum BananaDbResult {
	BANANA_DB_OK = 0,
	BANANA_DB_INVALID_ARGUMENT = 1,
	BANANA_DB_NOT_CONFIGURED = 2,
	BANANA_DB_ERROR = 3,
	BANANA_DB_NOT_FOUND = 4
} BananaDbResult;

int banana_db_query_profile_projection(int purchases, int multiplier, char** out_payload, int* out_row_count);

int banana_db_save_plant(const BananaPlant* plant);
int banana_db_get_plant(const char* plant_id, BananaPlant* plant);
int banana_db_clear_plants(void);

int banana_db_save_bunch(const BananaBunchRecord* bunch);
int banana_db_get_bunch(const char* bunch_id, BananaBunchRecord* bunch);
int banana_db_clear_bunches(void);

int banana_db_save_batch(const BananaBatch* batch);
int banana_db_get_batch(const char* batch_id, BananaBatch* batch);
int banana_db_clear_batches(void);

int banana_db_save_shipment(const BananaShipment* shipment);
int banana_db_get_shipment(const char* shipment_id, BananaShipment* shipment);
int banana_db_clear_shipments(void);

int banana_db_save_inventory_item(const BananaInventoryItem* item);
int banana_db_get_inventory_item(const char* inventory_id, BananaInventoryItem* item);
int banana_db_clear_inventory_items(void);

#endif
