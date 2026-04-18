#ifndef BANANA_REPOSITORIES_H
#define BANANA_REPOSITORIES_H

#include "banana_inventory.h"
#include "banana_processing.h"
#include "banana_supply_chain.h"

#ifdef __cplusplus
extern "C" {
#endif

#define BANANA_MAX_PLANT_REPOSITORY 64
#define BANANA_MAX_BUNCH_REPOSITORY 64
#define BANANA_MAX_SHIPMENT_REPOSITORY 64
#define BANANA_MAX_INVENTORY_REPOSITORY 64

void banana_plant_repository_clear(void);
BananaStatus banana_plant_repository_save(const BananaPlant* plant);
BananaStatus banana_plant_repository_get(const char* plant_id, BananaPlant* plant);

void banana_bunch_repository_clear(void);
BananaStatus banana_bunch_repository_save(const BananaBunchRecord* bunch);
BananaStatus banana_bunch_repository_get(const char* bunch_id, BananaBunchRecord* bunch);

void banana_shipment_repository_clear(void);
BananaStatus banana_shipment_repository_save(const BananaShipment* shipment);
BananaStatus banana_shipment_repository_get(const char* shipment_id, BananaShipment* shipment);

void banana_inventory_repository_clear(void);
BananaStatus banana_inventory_repository_save(const BananaInventoryItem* item);
BananaStatus banana_inventory_repository_get(const char* inventory_id, BananaInventoryItem* item);

#ifdef __cplusplus
}
#endif

#endif