#include "k3h4_dialogue_memory_delta_store.h"

#include <stddef.h>
#include <string.h>

static int clamp_int(int value, int min_value, int max_value)
{
    if (value < min_value)
        return min_value;
    if (value > max_value)
        return max_value;
    return value;
}

static int feature_is_valid(BananaK3h4DialogueMemoryFeature feature)
{
    return feature >= BANANA_K3H4_DIALOGUE_MEMORY_FEATURE_TRUST_DELTA &&
           feature <= BANANA_K3H4_DIALOGUE_MEMORY_FEATURE_HELPFULNESS_DELTA;
}

static int entry_is_active(const BananaK3h4DialogueMemoryEntry *entry, uint64_t current_tick)
{
    return entry->occupied != 0 && current_tick < entry->expires_tick;
}

static int find_existing_index(const BananaK3h4DialogueMemoryStore *store,
                               uint32_t npc_id,
                               BananaK3h4DialogueMemoryFeature feature)
{
    size_t index;

    for (index = 0; index < sizeof(store->entries) / sizeof(store->entries[0]); ++index)
    {
        const BananaK3h4DialogueMemoryEntry *entry = &store->entries[index];

        if (entry->occupied != 0 && entry->npc_id == npc_id && entry->feature == feature)
            return (int)index;
    }

    return -1;
}

static int find_free_or_expired_index(BananaK3h4DialogueMemoryStore *store, uint64_t current_tick)
{
    size_t index;

    for (index = 0; index < sizeof(store->entries) / sizeof(store->entries[0]); ++index)
    {
        BananaK3h4DialogueMemoryEntry *entry = &store->entries[index];

        if (entry->occupied == 0 || current_tick >= entry->expires_tick)
            return (int)index;
    }

    return -1;
}

static int find_evict_index(const BananaK3h4DialogueMemoryStore *store)
{
    size_t index;
    int selected = -1;

    for (index = 0; index < sizeof(store->entries) / sizeof(store->entries[0]); ++index)
    {
        const BananaK3h4DialogueMemoryEntry *entry = &store->entries[index];

        if (entry->occupied == 0)
            continue;

        if (selected < 0)
        {
            selected = (int)index;
            continue;
        }

        if (entry->expires_tick < store->entries[selected].expires_tick)
        {
            selected = (int)index;
            continue;
        }

        if (entry->expires_tick == store->entries[selected].expires_tick &&
            entry->sequence < store->entries[selected].sequence)
        {
            selected = (int)index;
        }
    }

    return selected;
}

void banana_native_k3h4_dialogue_memory_init(BananaK3h4DialogueMemoryStore *store,
                                              BananaK3h4DialogueMemoryPolicy policy)
{
    if (store == NULL)
        return;

    memset(store, 0, sizeof(*store));

    if (policy.ttl_ticks <= 0)
        policy.ttl_ticks = 1;

    if (policy.delta_min > policy.delta_max)
    {
        int temp = policy.delta_min;
        policy.delta_min = policy.delta_max;
        policy.delta_max = temp;
    }

    store->policy = policy;
    store->next_sequence = 1;
}

void banana_native_k3h4_dialogue_memory_prune_expired(BananaK3h4DialogueMemoryStore *store,
                                                       uint64_t current_tick)
{
    size_t index;

    if (store == NULL)
        return;

    for (index = 0; index < sizeof(store->entries) / sizeof(store->entries[0]); ++index)
    {
        BananaK3h4DialogueMemoryEntry *entry = &store->entries[index];

        if (entry->occupied != 0 && current_tick >= entry->expires_tick)
            memset(entry, 0, sizeof(*entry));
    }
}

int banana_native_k3h4_dialogue_memory_upsert_delta(BananaK3h4DialogueMemoryStore *store,
                                                     uint32_t npc_id,
                                                     BananaK3h4DialogueMemoryFeature feature,
                                                     int delta,
                                                     uint64_t current_tick)
{
    int index;
    BananaK3h4DialogueMemoryEntry *entry;

    if (store == NULL || !feature_is_valid(feature) || npc_id == 0)
        return -1;

    banana_native_k3h4_dialogue_memory_prune_expired(store, current_tick);

    index = find_existing_index(store, npc_id, feature);
    if (index < 0)
        index = find_free_or_expired_index(store, current_tick);
    if (index < 0)
        index = find_evict_index(store);
    if (index < 0)
        return -2;

    entry = &store->entries[index];
    entry->occupied = 1;
    entry->npc_id = npc_id;
    entry->feature = feature;
    entry->delta = clamp_int(delta, store->policy.delta_min, store->policy.delta_max);
    entry->updated_tick = current_tick;
    entry->expires_tick = current_tick + (uint64_t)store->policy.ttl_ticks;
    entry->sequence = store->next_sequence++;

    return 0;
}

int banana_native_k3h4_dialogue_memory_get_delta(const BananaK3h4DialogueMemoryStore *store,
                                                  uint32_t npc_id,
                                                  BananaK3h4DialogueMemoryFeature feature,
                                                  uint64_t current_tick,
                                                  int *out_delta)
{
    int index;

    if (store == NULL || out_delta == NULL || !feature_is_valid(feature) || npc_id == 0)
        return -1;

    index = find_existing_index(store, npc_id, feature);
    if (index < 0 || !entry_is_active(&store->entries[index], current_tick))
        return -2;

    *out_delta = store->entries[index].delta;
    return 0;
}

int banana_native_k3h4_dialogue_memory_count_active(const BananaK3h4DialogueMemoryStore *store,
                                                     uint64_t current_tick)
{
    size_t index;
    int count = 0;

    if (store == NULL)
        return 0;

    for (index = 0; index < sizeof(store->entries) / sizeof(store->entries[0]); ++index)
    {
        if (entry_is_active(&store->entries[index], current_tick))
            count += 1;
    }

    return count;
}
