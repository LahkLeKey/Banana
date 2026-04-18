#ifndef BANANA_RETAIL_H
#define BANANA_RETAIL_H

#include "banana_inventory.h"

#ifdef __cplusplus
extern "C" {
#endif

#define BANANA_BARCODE_CAPACITY 32

typedef enum BananaOrderStatus {
    BANANA_ORDER_CREATED = 0,
    BANANA_ORDER_PARTIALLY_FULFILLED = 1,
    BANANA_ORDER_FULFILLED = 2,
    BANANA_ORDER_BACKORDERED = 3
} BananaOrderStatus;

typedef struct BananaPrice {
    int cents;
    char currency[4];
} BananaPrice;

typedef struct BananaBarcode {
    char value[BANANA_BARCODE_CAPACITY];
} BananaBarcode;

typedef struct BananaStoreSection {
    BananaIdentifier section_id;
    BananaIdentifier store_id;
    BananaBarcode barcode;
    BananaPrice unit_price;
    BananaRipenessStage preferred_stage;
    int facing_capacity;
} BananaStoreSection;

typedef struct BananaRetailOrder {
    BananaIdentifier order_id;
    BananaIdentifier store_id;
    BananaIdentifier section_id;
    int ordered_quantity;
    int fulfilled_quantity;
    BananaPrice unit_price;
    BananaOrderStatus status;
} BananaRetailOrder;

BananaStatus banana_price_set(
    int cents,
    const char* currency,
    BananaPrice* price
);

BananaStatus banana_barcode_set(
    const char* value,
    BananaBarcode* barcode
);

BananaStatus banana_store_section_register(
    const char* section_id,
    const char* store_id,
    BananaBarcode barcode,
    BananaPrice unit_price,
    BananaRipenessStage preferred_stage,
    int facing_capacity,
    BananaStoreSection* section
);

BananaStatus banana_retail_order_create(
    const char* order_id,
    const char* store_id,
    const char* section_id,
    int ordered_quantity,
    BananaPrice unit_price,
    BananaRetailOrder* order
);

BananaStatus banana_retail_order_fulfill(
    BananaRetailOrder* order,
    BananaInventoryItem* item,
    int quantity,
    int event_day_ordinal,
    BananaDomainEvent* event
);

int banana_store_section_can_face_inventory(
    const BananaStoreSection* section,
    const BananaInventoryItem* item
);

#ifdef __cplusplus
}
#endif

#endif