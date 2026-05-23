#ifndef BANANA_ENGINE_RUNTIME_MERCHANT_QUERY_ABI_H
#define BANANA_ENGINE_RUNTIME_MERCHANT_QUERY_ABI_H

#include "application_service_ports.h"

#ifdef __cplusplus
extern "C"
{
#endif

    int runtime_merchant_query_abi_get_price(const RuntimeApplicationServicePorts *ports,
                                             int npc_id,
                                             const char *item_type,
                                             int *inout_seeded);

#ifdef __cplusplus
}
#endif

#endif
