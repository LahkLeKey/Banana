#ifndef BANANA_DAL_SUPPLY_CHAIN_H
#define BANANA_DAL_SUPPLY_CHAIN_H

#include "../../domain/banana_supply_chain.h"

int banana_supply_chain_db_save_batch(const BananaBatch* batch);
int banana_supply_chain_db_get_batch(const char* batch_id, BananaBatch* batch);
int banana_supply_chain_db_clear_batches(void);

int banana_supply_chain_db_save_shipment(const BananaShipment* shipment);
int banana_supply_chain_db_get_shipment(const char* shipment_id, BananaShipment* shipment);
int banana_supply_chain_db_clear_shipments(void);

#endif