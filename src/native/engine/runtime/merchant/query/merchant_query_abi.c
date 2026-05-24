#include "merchant_query_abi.h"

int runtime_merchant_query_abi_get_price(const RuntimeApplicationServicePorts *ports,
                                         int npc_id,
                                         const char *item_type,
                                         int *inout_seeded)
{
    int price = 0;

    if (!ports || !ports->merchant.get_price || !item_type)
        return 0;

    if (ports->merchant.get_price(npc_id, item_type, inout_seeded, &price) != 0)
        return 0;

    return price;
}
