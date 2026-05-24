#include "resource_domain.h"

#include <string.h>

#define BANANA_COMPILE_ASSERT(name, expr) typedef char banana_compile_assert_##name[(expr) ? 1 : -1]

typedef struct RuntimeResourceMapping
{
    RuntimeResourceKey key;
    const char *name;
} RuntimeResourceMapping;

static const RuntimeResourceMapping k_resource_mappings[] = {
    {RUNTIME_RESOURCE_WOOD, "wood"},
    {RUNTIME_RESOURCE_ORE, "ore"},
    {RUNTIME_RESOURCE_GOLD, "gold"},
};

BANANA_COMPILE_ASSERT(resource_mapping_count,
                      (sizeof(k_resource_mappings) / sizeof(k_resource_mappings[0])) ==
                          RUNTIME_RESOURCE_COUNT);

int runtime_resource_parse_key(const char *resource_type, RuntimeResourceKey *out_key)
{
    size_t index = 0;

    if (!resource_type || !out_key)
        return -1;

    for (index = 0; index < (sizeof(k_resource_mappings) / sizeof(k_resource_mappings[0])); index++)
    {
        if (strcmp(resource_type, k_resource_mappings[index].name) == 0)
        {
            *out_key = k_resource_mappings[index].key;
            return 0;
        }
    }

    return -1;
}

const char *runtime_resource_key_name(RuntimeResourceKey key)
{
    size_t index = 0;

    for (index = 0; index < (sizeof(k_resource_mappings) / sizeof(k_resource_mappings[0])); index++)
    {
        if (k_resource_mappings[index].key == key)
            return k_resource_mappings[index].name;
    }

    return "";
}
