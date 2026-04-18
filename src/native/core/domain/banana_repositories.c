#include "banana_repositories.h"

#include "../dal/banana_db.h"

#include <string.h>

static BananaPlant plant_repository[BANANA_MAX_PLANT_REPOSITORY];
static int plant_repository_count = 0;

static BananaBunchRecord bunch_repository[BANANA_MAX_BUNCH_REPOSITORY];
static int bunch_repository_count = 0;

static BananaShipment shipment_repository[BANANA_MAX_SHIPMENT_REPOSITORY];
static int shipment_repository_count = 0;

static BananaInventoryItem inventory_repository[BANANA_MAX_INVENTORY_REPOSITORY];
static int inventory_repository_count = 0;

static int find_identifier_index(const BananaIdentifier* identifiers, int count, const char* value) {
    int index = 0;

    if (identifiers == 0 || value == 0) {
        return -1;
    }

    for (index = 0; index < count; index++) {
        if (strcmp(identifiers[index].value, value) == 0) {
            return index;
        }
    }

    return -1;
}

static BananaStatus cache_plant(const BananaPlant* plant) {
    BananaIdentifier identifiers[BANANA_MAX_PLANT_REPOSITORY];
    int index = 0;

    if (plant == 0) {
        return BANANA_ERROR_INVALID_INPUT;
    }

    for (index = 0; index < plant_repository_count; index++) {
        identifiers[index] = plant_repository[index].plant_id;
    }

    index = find_identifier_index(identifiers, plant_repository_count, plant->plant_id.value);
    if (index >= 0) {
        plant_repository[index] = *plant;
        return BANANA_OK;
    }

    if (plant_repository_count >= BANANA_MAX_PLANT_REPOSITORY) {
        return BANANA_ERROR_OVERFLOW;
    }

    plant_repository[plant_repository_count] = *plant;
    plant_repository_count++;
    return BANANA_OK;
}

static BananaStatus cache_bunch(const BananaBunchRecord* bunch) {
    BananaIdentifier identifiers[BANANA_MAX_BUNCH_REPOSITORY];
    int index = 0;

    if (bunch == 0) {
        return BANANA_ERROR_INVALID_INPUT;
    }

    for (index = 0; index < bunch_repository_count; index++) {
        identifiers[index] = bunch_repository[index].aggregate.bunch.bunch_id;
    }

    index = find_identifier_index(identifiers, bunch_repository_count, bunch->aggregate.bunch.bunch_id.value);
    if (index >= 0) {
        bunch_repository[index] = *bunch;
        return BANANA_OK;
    }

    if (bunch_repository_count >= BANANA_MAX_BUNCH_REPOSITORY) {
        return BANANA_ERROR_OVERFLOW;
    }

    bunch_repository[bunch_repository_count] = *bunch;
    bunch_repository_count++;
    return BANANA_OK;
}

static BananaStatus cache_shipment(const BananaShipment* shipment) {
    BananaIdentifier identifiers[BANANA_MAX_SHIPMENT_REPOSITORY];
    int index = 0;

    if (shipment == 0) {
        return BANANA_ERROR_INVALID_INPUT;
    }

    for (index = 0; index < shipment_repository_count; index++) {
        identifiers[index] = shipment_repository[index].shipment_id;
    }

    index = find_identifier_index(identifiers, shipment_repository_count, shipment->shipment_id.value);
    if (index >= 0) {
        shipment_repository[index] = *shipment;
        return BANANA_OK;
    }

    if (shipment_repository_count >= BANANA_MAX_SHIPMENT_REPOSITORY) {
        return BANANA_ERROR_OVERFLOW;
    }

    shipment_repository[shipment_repository_count] = *shipment;
    shipment_repository_count++;
    return BANANA_OK;
}

static BananaStatus cache_inventory(const BananaInventoryItem* item) {
    BananaIdentifier identifiers[BANANA_MAX_INVENTORY_REPOSITORY];
    int index = 0;

    if (item == 0) {
        return BANANA_ERROR_INVALID_INPUT;
    }

    for (index = 0; index < inventory_repository_count; index++) {
        identifiers[index] = inventory_repository[index].inventory_id;
    }

    index = find_identifier_index(identifiers, inventory_repository_count, item->inventory_id.value);
    if (index >= 0) {
        inventory_repository[index] = *item;
        return BANANA_OK;
    }

    if (inventory_repository_count >= BANANA_MAX_INVENTORY_REPOSITORY) {
        return BANANA_ERROR_OVERFLOW;
    }

    inventory_repository[inventory_repository_count] = *item;
    inventory_repository_count++;
    return BANANA_OK;
}

void banana_plant_repository_clear(void) {
    memset(plant_repository, 0, sizeof(plant_repository));
    plant_repository_count = 0;
    (void)banana_db_clear_plants();
}

BananaStatus banana_plant_repository_save(const BananaPlant* plant) {
    BananaStatus status = BANANA_OK;

    if (plant == 0) {
        return BANANA_ERROR_INVALID_INPUT;
    }

    status = cache_plant(plant);
    if (status != BANANA_OK) {
        return status;
    }

    (void)banana_db_save_plant(plant);
    return BANANA_OK;
}

BananaStatus banana_plant_repository_get(const char* plant_id, BananaPlant* plant) {
    BananaIdentifier identifiers[BANANA_MAX_PLANT_REPOSITORY];
    int index = 0;

    if (plant == 0) {
        return BANANA_ERROR_INVALID_INPUT;
    }

    if (plant_id != 0 && plant_id[0] != '\0') {
        int db_status = banana_db_get_plant(plant_id, plant);
        if (db_status == BANANA_DB_OK) {
            (void)cache_plant(plant);
            return BANANA_OK;
        }
    }

    for (index = 0; index < plant_repository_count; index++) {
        identifiers[index] = plant_repository[index].plant_id;
    }

    index = find_identifier_index(identifiers, plant_repository_count, plant_id);
    if (index < 0) {
        return BANANA_ERROR_NOT_FOUND;
    }

    *plant = plant_repository[index];
    return BANANA_OK;
}

void banana_bunch_repository_clear(void) {
    memset(bunch_repository, 0, sizeof(bunch_repository));
    bunch_repository_count = 0;
    (void)banana_db_clear_bunches();
}

BananaStatus banana_bunch_repository_save(const BananaBunchRecord* bunch) {
    BananaStatus status = BANANA_OK;

    if (bunch == 0) {
        return BANANA_ERROR_INVALID_INPUT;
    }

    status = cache_bunch(bunch);
    if (status != BANANA_OK) {
        return status;
    }

    (void)banana_db_save_bunch(bunch);
    return BANANA_OK;
}

BananaStatus banana_bunch_repository_get(const char* bunch_id, BananaBunchRecord* bunch) {
    BananaIdentifier identifiers[BANANA_MAX_BUNCH_REPOSITORY];
    int index = 0;

    if (bunch == 0) {
        return BANANA_ERROR_INVALID_INPUT;
    }

    if (bunch_id != 0 && bunch_id[0] != '\0') {
        int db_status = banana_db_get_bunch(bunch_id, bunch);
        if (db_status == BANANA_DB_OK) {
            (void)cache_bunch(bunch);
            return BANANA_OK;
        }
    }

    for (index = 0; index < bunch_repository_count; index++) {
        identifiers[index] = bunch_repository[index].aggregate.bunch.bunch_id;
    }

    index = find_identifier_index(identifiers, bunch_repository_count, bunch_id);
    if (index < 0) {
        return BANANA_ERROR_NOT_FOUND;
    }

    *bunch = bunch_repository[index];
    return BANANA_OK;
}

void banana_shipment_repository_clear(void) {
    memset(shipment_repository, 0, sizeof(shipment_repository));
    shipment_repository_count = 0;
    (void)banana_db_clear_shipments();
}

BananaStatus banana_shipment_repository_save(const BananaShipment* shipment) {
    BananaStatus status = BANANA_OK;

    if (shipment == 0) {
        return BANANA_ERROR_INVALID_INPUT;
    }

    status = cache_shipment(shipment);
    if (status != BANANA_OK) {
        return status;
    }

    (void)banana_db_save_shipment(shipment);
    return BANANA_OK;
}

BananaStatus banana_shipment_repository_get(const char* shipment_id, BananaShipment* shipment) {
    BananaIdentifier identifiers[BANANA_MAX_SHIPMENT_REPOSITORY];
    int index = 0;

    if (shipment == 0) {
        return BANANA_ERROR_INVALID_INPUT;
    }

    if (shipment_id != 0 && shipment_id[0] != '\0') {
        int db_status = banana_db_get_shipment(shipment_id, shipment);
        if (db_status == BANANA_DB_OK) {
            (void)cache_shipment(shipment);
            return BANANA_OK;
        }
    }

    for (index = 0; index < shipment_repository_count; index++) {
        identifiers[index] = shipment_repository[index].shipment_id;
    }

    index = find_identifier_index(identifiers, shipment_repository_count, shipment_id);
    if (index < 0) {
        return BANANA_ERROR_NOT_FOUND;
    }

    *shipment = shipment_repository[index];
    return BANANA_OK;
}

void banana_inventory_repository_clear(void) {
    memset(inventory_repository, 0, sizeof(inventory_repository));
    inventory_repository_count = 0;
    (void)banana_db_clear_inventory_items();
}

BananaStatus banana_inventory_repository_save(const BananaInventoryItem* item) {
    BananaStatus status = BANANA_OK;

    if (item == 0) {
        return BANANA_ERROR_INVALID_INPUT;
    }

    status = cache_inventory(item);
    if (status != BANANA_OK) {
        return status;
    }

    (void)banana_db_save_inventory_item(item);
    return BANANA_OK;
}

BananaStatus banana_inventory_repository_get(const char* inventory_id, BananaInventoryItem* item) {
    BananaIdentifier identifiers[BANANA_MAX_INVENTORY_REPOSITORY];
    int index = 0;

    if (item == 0) {
        return BANANA_ERROR_INVALID_INPUT;
    }

    if (inventory_id != 0 && inventory_id[0] != '\0') {
        int db_status = banana_db_get_inventory_item(inventory_id, item);
        if (db_status == BANANA_DB_OK) {
            (void)cache_inventory(item);
            return BANANA_OK;
        }
    }

    for (index = 0; index < inventory_repository_count; index++) {
        identifiers[index] = inventory_repository[index].inventory_id;
    }

    index = find_identifier_index(identifiers, inventory_repository_count, inventory_id);
    if (index < 0) {
        return BANANA_ERROR_NOT_FOUND;
    }

    *item = inventory_repository[index];
    return BANANA_OK;
}