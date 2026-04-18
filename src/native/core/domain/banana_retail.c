#include "banana_retail.h"

#include <string.h>

static int stage_distance(BananaRipenessStage left, BananaRipenessStage right) {
    return (left > right) ? (left - right) : (right - left);
}

BananaStatus banana_price_set(
    int cents,
    const char* currency,
    BananaPrice* price
) {
    size_t length = 0U;

    if (price == 0 || cents < 0 || currency == 0 || currency[0] == '\0') {
        return BANANA_ERROR_INVALID_INPUT;
    }

    length = strlen(currency);
    if (length != 3U) {
        return BANANA_ERROR_INVALID_INPUT;
    }

    memset(price, 0, sizeof(*price));
    price->cents = cents;
    memcpy(price->currency, currency, length);
    return BANANA_OK;
}

BananaStatus banana_barcode_set(
    const char* value,
    BananaBarcode* barcode
) {
    size_t length = 0U;

    if (barcode == 0 || value == 0 || value[0] == '\0') {
        return BANANA_ERROR_INVALID_INPUT;
    }

    length = strlen(value);
    if (length >= BANANA_BARCODE_CAPACITY) {
        return BANANA_ERROR_INVALID_INPUT;
    }

    memset(barcode, 0, sizeof(*barcode));
    memcpy(barcode->value, value, length);
    return BANANA_OK;
}

BananaStatus banana_store_section_register(
    const char* section_id,
    const char* store_id,
    BananaBarcode barcode,
    BananaPrice unit_price,
    BananaRipenessStage preferred_stage,
    int facing_capacity,
    BananaStoreSection* section
) {
    BananaStatus status = BANANA_OK;

    if (section == 0 || facing_capacity <= 0 || barcode.value[0] == '\0' || unit_price.currency[0] == '\0') {
        return BANANA_ERROR_INVALID_INPUT;
    }

    memset(section, 0, sizeof(*section));
    status = banana_identifier_copy(&section->section_id, section_id);
    if (status != BANANA_OK) {
        return status;
    }

    status = banana_identifier_copy(&section->store_id, store_id);
    if (status != BANANA_OK) {
        return status;
    }

    section->barcode = barcode;
    section->unit_price = unit_price;
    section->preferred_stage = preferred_stage;
    section->facing_capacity = facing_capacity;
    return BANANA_OK;
}

BananaStatus banana_retail_order_create(
    const char* order_id,
    const char* store_id,
    const char* section_id,
    int ordered_quantity,
    BananaPrice unit_price,
    BananaRetailOrder* order
) {
    BananaStatus status = BANANA_OK;

    if (order == 0 || ordered_quantity <= 0 || unit_price.currency[0] == '\0') {
        return BANANA_ERROR_INVALID_INPUT;
    }

    memset(order, 0, sizeof(*order));
    status = banana_identifier_copy(&order->order_id, order_id);
    if (status != BANANA_OK) {
        return status;
    }

    status = banana_identifier_copy(&order->store_id, store_id);
    if (status != BANANA_OK) {
        return status;
    }

    status = banana_identifier_copy(&order->section_id, section_id);
    if (status != BANANA_OK) {
        return status;
    }

    order->ordered_quantity = ordered_quantity;
    order->unit_price = unit_price;
    order->status = BANANA_ORDER_CREATED;
    return BANANA_OK;
}

BananaStatus banana_retail_order_fulfill(
    BananaRetailOrder* order,
    BananaInventoryItem* item,
    int quantity,
    int event_day_ordinal,
    BananaDomainEvent* event
) {
    BananaStatus status = BANANA_OK;

    if (order == 0
        || item == 0
        || quantity <= 0
        || order->fulfilled_quantity + quantity > order->ordered_quantity) {
        return BANANA_ERROR_INVALID_INPUT;
    }

    status = banana_inventory_sell(item, quantity, event_day_ordinal, event);
    if (status != BANANA_OK) {
        return status;
    }

    order->fulfilled_quantity += quantity;
    if (order->fulfilled_quantity == order->ordered_quantity) {
        order->status = BANANA_ORDER_FULFILLED;
    } else if (item->quantity_on_hand == 0) {
        order->status = BANANA_ORDER_BACKORDERED;
    } else {
        order->status = BANANA_ORDER_PARTIALLY_FULFILLED;
    }

    return BANANA_OK;
}

int banana_store_section_can_face_inventory(
    const BananaStoreSection* section,
    const BananaInventoryItem* item
) {
    if (section == 0 || item == 0) {
        return 0;
    }

    return item->quantity_on_hand <= section->facing_capacity
        && stage_distance(section->preferred_stage, item->ripeness_stage) <= 1;
}