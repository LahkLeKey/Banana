#include "banana_application.h"

BananaStatus banana_application_harvest_bunch(
    const char* plant_id,
    const BananaHarvestCommand* command,
    BananaBunchRecord* bunch,
    BananaDomainEvent* bloom_event,
    BananaDomainEvent* harvest_event
) {
    BananaPlant plant;
    BananaStatus status = BANANA_OK;

    if (command == 0 || bunch == 0) {
        return BANANA_ERROR_INVALID_INPUT;
    }

    status = banana_plant_repository_get(plant_id, &plant);
    if (status != BANANA_OK) {
        return status;
    }

    status = banana_plant_record_bloom(&plant, command->bunch_id, command->bloom_day_ordinal, bloom_event);
    if (status != BANANA_OK) {
        return status;
    }

    status = banana_bunch_factory_create(
        &plant,
        command->bunch_id,
        &command->bunch_spec,
        command->cultivar,
        command->fruit_specs,
        command->fruit_count,
        bunch,
        harvest_event);
    if (status != BANANA_OK) {
        return status;
    }

    return banana_bunch_repository_save(bunch);
}

BananaStatus banana_application_ripen_bunch(
    const BananaRipenCommand* command,
    BananaBunchRecord* bunch,
    BananaRipenessPrediction* prediction,
    BananaDomainEvent* event
) {
    BananaStatus status = BANANA_OK;

    if (command == 0 || bunch == 0) {
        return BANANA_ERROR_INVALID_INPUT;
    }

    status = banana_bunch_repository_get(command->bunch_id, bunch);
    if (status != BANANA_OK) {
        return status;
    }

    status = banana_ripening_service_apply(
        bunch,
        &command->input,
        command->event_day_ordinal,
        prediction,
        event);
    if (status != BANANA_OK) {
        return status;
    }

    return banana_bunch_repository_save(bunch);
}

BananaStatus banana_application_ship_bunch(
    const BananaShipCommand* command,
    BananaBatch* batch,
    BananaShipment* shipment,
    BananaDomainEvent* event
) {
    BananaBunchRecord bunch;
    BananaStatus status = BANANA_OK;

    if (command == 0 || batch == 0 || shipment == 0) {
        return BANANA_ERROR_INVALID_INPUT;
    }

    status = banana_bunch_repository_get(command->bunch_id, &bunch);
    if (status != BANANA_OK) {
        return status;
    }

    status = banana_batch_register(
        command->batch_id,
        command->origin_farm,
        command->storage_temp_c,
        command->ethylene_exposure,
        command->estimated_shelf_life_days,
        batch);
    if (status != BANANA_OK) {
        return status;
    }

    status = banana_batch_add_bunch(batch, bunch.aggregate.bunch.bunch_id.value);
    if (status != BANANA_OK) {
        return status;
    }

    status = banana_shipment_create(
        command->shipment_id,
        command->origin_node_id,
        command->destination_node_id,
        shipment);
    if (status != BANANA_OK) {
        return status;
    }

    status = banana_shipment_add_batch(shipment, batch);
    if (status != BANANA_OK) {
        return status;
    }

    status = banana_shipment_dispatch(shipment, batch, command->departure_day_ordinal, event);
    if (status != BANANA_OK) {
        return status;
    }

    return banana_shipment_repository_save(shipment);
}

BananaStatus banana_application_receive_inventory(
    const BananaInventoryReceiveCommand* command,
    BananaInventoryItem* item,
    BananaDomainEvent* event
) {
    BananaStatus status = BANANA_OK;

    if (command == 0 || item == 0) {
        return BANANA_ERROR_INVALID_INPUT;
    }

    status = banana_inventory_receive(
        command->inventory_id,
        command->location_id,
        command->source_batch_id,
        command->ripeness_stage,
        command->quantity_on_hand,
        command->reorder_threshold,
        command->total_weight_kg,
        command->event_day_ordinal,
        item,
        event);
    if (status != BANANA_OK) {
        return status;
    }

    return banana_inventory_repository_save(item);
}

BananaStatus banana_application_sell_bananas(
    const BananaSellCommand* command,
    BananaInventoryItem* item,
    BananaDomainEvent* event
) {
    BananaStatus status = BANANA_OK;

    if (command == 0 || item == 0) {
        return BANANA_ERROR_INVALID_INPUT;
    }

    status = banana_inventory_repository_get(command->inventory_id, item);
    if (status != BANANA_OK) {
        return status;
    }

    status = banana_inventory_sell(item, command->quantity, command->event_day_ordinal, event);
    if (status != BANANA_OK) {
        return status;
    }

    return banana_inventory_repository_save(item);
}

BananaStatus banana_application_discard_spoiled(
    const BananaDiscardSpoiledCommand* command,
    BananaInventoryItem* item,
    BananaDomainEvent* event
) {
    BananaStatus status = BANANA_OK;

    if (command == 0 || item == 0) {
        return BANANA_ERROR_INVALID_INPUT;
    }

    status = banana_inventory_repository_get(command->inventory_id, item);
    if (status != BANANA_OK) {
        return status;
    }

    status = banana_inventory_discard_spoiled(item, command->quantity, command->event_day_ordinal, event);
    if (status != BANANA_OK) {
        return status;
    }

    return banana_inventory_repository_save(item);
}