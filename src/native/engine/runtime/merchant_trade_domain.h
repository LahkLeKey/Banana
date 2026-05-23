#ifndef BANANA_ENGINE_RUNTIME_MERCHANT_TRADE_DOMAIN_H
#define BANANA_ENGINE_RUNTIME_MERCHANT_TRADE_DOMAIN_H

#include "../ai/npc_merchant.h"

#ifdef __cplusplus
extern "C"
{
#endif

    int runtime_merchant_map_buy_status(MerchantTradeResult result);
    int runtime_merchant_map_sell_status(MerchantTradeResult result);

#ifdef __cplusplus
}
#endif

#endif
