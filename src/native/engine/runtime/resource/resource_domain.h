#ifndef BANANA_ENGINE_RUNTIME_RESOURCE_DOMAIN_H
#define BANANA_ENGINE_RUNTIME_RESOURCE_DOMAIN_H

#ifdef __cplusplus
extern "C"
{
#endif

#define BANANA_COMPILE_ASSERT(name, expr) typedef char banana_compile_assert_##name[(expr) ? 1 : -1]

    typedef enum RuntimeResourceKey
    {
        RUNTIME_RESOURCE_WOOD = 0,
        RUNTIME_RESOURCE_ORE = 1,
        RUNTIME_RESOURCE_GOLD = 2,
        RUNTIME_RESOURCE_COUNT = 3
    } RuntimeResourceKey;

    BANANA_COMPILE_ASSERT(runtime_resource_key_wood_index, RUNTIME_RESOURCE_WOOD == 0);
    BANANA_COMPILE_ASSERT(runtime_resource_key_ore_index, RUNTIME_RESOURCE_ORE == 1);
    BANANA_COMPILE_ASSERT(runtime_resource_key_gold_index, RUNTIME_RESOURCE_GOLD == 2);
    BANANA_COMPILE_ASSERT(runtime_resource_key_count, RUNTIME_RESOURCE_COUNT == 3);

    int runtime_resource_parse_key(const char *resource_type, RuntimeResourceKey *out_key);
    const char *runtime_resource_key_name(RuntimeResourceKey key);

#ifdef __cplusplus
}
#endif

#endif
