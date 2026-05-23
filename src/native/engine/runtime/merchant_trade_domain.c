#include "merchant_trade_domain.h"

#define BANANA_COMPILE_ASSERT(name, expr) typedef char banana_compile_assert_##name[(expr) ? 1 : -1]

BANANA_COMPILE_ASSERT(merchant_trade_ok_index, MERCHANT_TRADE_OK == 0);
BANANA_COMPILE_ASSERT(merchant_trade_insufficient_gold_index,
                      MERCHANT_TRADE_INSUFFICIENT_GOLD == 1);
BANANA_COMPILE_ASSERT(merchant_trade_insufficient_stock_index,
                      MERCHANT_TRADE_INSUFFICIENT_STOCK == 2);
BANANA_COMPILE_ASSERT(merchant_trade_invalid_item_index, MERCHANT_TRADE_INVALID_ITEM == 3);
BANANA_COMPILE_ASSERT(merchant_trade_invalid_merchant_index,
                      MERCHANT_TRADE_INVALID_MERCHANT == 4);

int runtime_merchant_map_buy_status(MerchantTradeResult result)
{
    switch (result)
    {
    case MERCHANT_TRADE_OK:
        return 0;
    case MERCHANT_TRADE_INSUFFICIENT_GOLD:
        return -2;
    case MERCHANT_TRADE_INSUFFICIENT_STOCK:
        return -3;
    default:
        return -1;
    }
}

int runtime_merchant_map_sell_status(MerchantTradeResult result)
{
    switch (result)
    {
    case MERCHANT_TRADE_OK:
        return 0;
    case MERCHANT_TRADE_INSUFFICIENT_STOCK:
        return -3;
    case MERCHANT_TRADE_INSUFFICIENT_GOLD:
        return -4;
    default:
        return -1;
    }
}
