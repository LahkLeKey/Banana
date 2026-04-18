#include "banana_db.h"

#include "domain/banana_cultivation.h"
#include "domain/banana_inventory.h"
#include "domain/banana_processing.h"
#include "domain/banana_projection_legacy.h"
#include "domain/banana_supply_chain.h"

int banana_db_query(int purchases, int multiplier, char** out_payload, int* out_row_count) {
    return banana_projection_legacy_db_query(purchases, multiplier, out_payload, out_row_count);
}

int banana_db_save_plant(const BananaPlant* plant) {
    return banana_cultivation_db_save_plant(plant);
}

int banana_db_get_plant(const char* plant_id, BananaPlant* plant) {
    return banana_cultivation_db_get_plant(plant_id, plant);
}

int banana_db_clear_plants(void) {
    return banana_cultivation_db_clear_plants();
}

int banana_db_save_bunch(const BananaBunchRecord* bunch) {
    return banana_processing_db_save_bunch(bunch);
}

int banana_db_get_bunch(const char* bunch_id, BananaBunchRecord* bunch) {
    return banana_processing_db_get_bunch(bunch_id, bunch);
}

int banana_db_clear_bunches(void) {
    return banana_processing_db_clear_bunches();
}

int banana_db_save_batch(const BananaBatch* batch) {
    return banana_supply_chain_db_save_batch(batch);
}

int banana_db_get_batch(const char* batch_id, BananaBatch* batch) {
    return banana_supply_chain_db_get_batch(batch_id, batch);
}

int banana_db_clear_batches(void) {
    return banana_supply_chain_db_clear_batches();
}

int banana_db_save_shipment(const BananaShipment* shipment) {
    return banana_supply_chain_db_save_shipment(shipment);
}

int banana_db_get_shipment(const char* shipment_id, BananaShipment* shipment) {
    return banana_supply_chain_db_get_shipment(shipment_id, shipment);
}

int banana_db_clear_shipments(void) {
    return banana_supply_chain_db_clear_shipments();
}

int banana_db_save_inventory_item(const BananaInventoryItem* item) {
    return banana_inventory_db_save_item(item);
}

int banana_db_get_inventory_item(const char* inventory_id, BananaInventoryItem* item) {
    return banana_inventory_db_get_item(inventory_id, item);
}

int banana_db_clear_inventory_items(void) {
    return banana_inventory_db_clear_items();
}
