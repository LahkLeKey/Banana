#include "player_merchant_adapter.h"

#include <stddef.h>

#include "../merchant/trade/merchant_trade_abi.h"
#include "gateway/player_gateway_abi.h"
#include "resource/player_resource_abi.h"

#define PLAYER_MERCHANT_ADAPTER_ASSERT(name, expr) \
    typedef char player_merchant_adapter_assert_##name[(expr) ? 1 : -1]

typedef struct RuntimePlayerMerchantAdapterContext
{
    const char *player_guid;
} RuntimePlayerMerchantAdapterContext;

static int adapter_get_resource_by_key(void *context, RuntimeResourceKey resource_key);
static int adapter_set_resource_total_by_key(void *context,
                                             RuntimeResourceKey resource_key,
                                             int target_amount);
static int adapter_add_resource_by_key(void *context,
                                       RuntimeResourceKey resource_key,
                                       int amount);

typedef int (*RuntimeMerchantAdapterGetResourceByKeyFn)(void *context,
                                                        RuntimeResourceKey resource_key);
typedef int (*RuntimeMerchantAdapterSetResourceTotalByKeyFn)(void *context,
                                                             RuntimeResourceKey resource_key,
                                                             int target_amount);
typedef int (*RuntimeMerchantAdapterAddResourceByKeyFn)(void *context,
                                                        RuntimeResourceKey resource_key,
                                                        int amount);

PLAYER_MERCHANT_ADAPTER_ASSERT(context_player_guid_first_field,
                               offsetof(RuntimePlayerMerchantAdapterContext, player_guid) == 0);
PLAYER_MERCHANT_ADAPTER_ASSERT(context_layout_single_pointer,
                               sizeof(RuntimePlayerMerchantAdapterContext) == sizeof(const char *));
PLAYER_MERCHANT_ADAPTER_ASSERT(gateway_layout_context_first,
                               offsetof(RuntimeMerchantResourceGateway, context) == 0);
PLAYER_MERCHANT_ADAPTER_ASSERT(gateway_layout_callbacks_follow_context,
                               offsetof(RuntimeMerchantResourceGateway, get) > offsetof(RuntimeMerchantResourceGateway, context));
PLAYER_MERCHANT_ADAPTER_ASSERT(adapter_get_signature,
                               sizeof(&adapter_get_resource_by_key) == sizeof(RuntimeMerchantAdapterGetResourceByKeyFn));
PLAYER_MERCHANT_ADAPTER_ASSERT(adapter_set_total_signature,
                               sizeof(&adapter_set_resource_total_by_key) == sizeof(RuntimeMerchantAdapterSetResourceTotalByKeyFn));
PLAYER_MERCHANT_ADAPTER_ASSERT(adapter_add_signature,
                               sizeof(&adapter_add_resource_by_key) == sizeof(RuntimeMerchantAdapterAddResourceByKeyFn));

static int adapter_get_resource_by_key(void *context, RuntimeResourceKey resource_key)
{
    RuntimePlayerMerchantAdapterContext *adapter_context =
        (RuntimePlayerMerchantAdapterContext *)context;
    return runtime_player_resource_abi_get_by_key(adapter_context ? adapter_context->player_guid : NULL,
                                                  resource_key);
}

static int adapter_set_resource_total_by_key(void *context,
                                             RuntimeResourceKey resource_key,
                                             int target_amount)
{
    RuntimePlayerMerchantAdapterContext *adapter_context =
        (RuntimePlayerMerchantAdapterContext *)context;
    return runtime_player_resource_abi_set_total_by_key(adapter_context ? adapter_context->player_guid : NULL,
                                                        resource_key,
                                                        target_amount);
}

static int adapter_add_resource_by_key(void *context,
                                       RuntimeResourceKey resource_key,
                                       int amount)
{
    RuntimePlayerMerchantAdapterContext *adapter_context =
        (RuntimePlayerMerchantAdapterContext *)context;
    return runtime_player_resource_abi_add_by_key(adapter_context ? adapter_context->player_guid : NULL,
                                                  resource_key,
                                                  amount);
}

static RuntimeMerchantResourceGateway adapter_gateway(RuntimePlayerMerchantAdapterContext *context)
{
    RuntimeMerchantResourceGateway gateway;

    gateway = runtime_player_gateway_resource_gateway(adapter_get_resource_by_key,
                                                      adapter_set_resource_total_by_key,
                                                      adapter_add_resource_by_key,
                                                      context);
    return gateway;
}

int runtime_player_merchant_adapter_get_resource(const char *resource_type,
                                                 EntityId active_player_id)
{
    const char *player_guid = runtime_player_gateway_active_guid(active_player_id);
    return runtime_player_resource_abi_get(player_guid, resource_type);
}

int runtime_player_merchant_adapter_add_resource(const char *resource_type,
                                                 int amount,
                                                 EntityId active_player_id)
{
    const char *player_guid = runtime_player_gateway_active_guid(active_player_id);
    return runtime_player_resource_abi_add(player_guid, resource_type, amount);
}

int runtime_player_merchant_adapter_trade_buy(const RuntimeApplicationServicePorts *ports,
                                              int world_ready,
                                              int npc_id,
                                              const char *item_type,
                                              int quantity,
                                              int *inout_seeded,
                                              EntityId active_player_id)
{
    RuntimePlayerMerchantAdapterContext context;
    RuntimeMerchantResourceGateway gateway;

    context.player_guid = runtime_player_gateway_active_guid(active_player_id);
    gateway = adapter_gateway(&context);

    int status = runtime_merchant_trade_abi_buy(ports,
                                                world_ready,
                                                npc_id,
                                                item_type,
                                                quantity,
                                                inout_seeded,
                                                gateway);
    return status;
}

int runtime_player_merchant_adapter_trade_sell(const RuntimeApplicationServicePorts *ports,
                                               int world_ready,
                                               int npc_id,
                                               const char *item_type,
                                               int quantity,
                                               int *inout_seeded,
                                               EntityId active_player_id)
{
    RuntimePlayerMerchantAdapterContext context;
    RuntimeMerchantResourceGateway gateway;

    context.player_guid = runtime_player_gateway_active_guid(active_player_id);
    gateway = adapter_gateway(&context);

    int status = runtime_merchant_trade_abi_sell(ports,
                                                 world_ready,
                                                 npc_id,
                                                 item_type,
                                                 quantity,
                                                 inout_seeded,
                                                 gateway);
    return status;
}