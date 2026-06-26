#ifndef BANANA_NATIVE_K3H4_DIALOGUE_MEMORY_DELTA_STORE_H
#define BANANA_NATIVE_K3H4_DIALOGUE_MEMORY_DELTA_STORE_H

#include <stdint.h>

#ifdef __cplusplus
extern "C"
{
#endif

    typedef enum BananaK3h4DialogueMemoryFeature
    {
        BANANA_K3H4_DIALOGUE_MEMORY_FEATURE_TRUST_DELTA = 0,
        BANANA_K3H4_DIALOGUE_MEMORY_FEATURE_HOSTILITY_DELTA = 1,
        BANANA_K3H4_DIALOGUE_MEMORY_FEATURE_HELPFULNESS_DELTA = 2,
    } BananaK3h4DialogueMemoryFeature;

    enum
    {
        BANANA_K3H4_DIALOGUE_MEMORY_NPC_CAP = 8,
        BANANA_K3H4_DIALOGUE_MEMORY_FEATURE_CAP = 3,
    };

    typedef struct BananaK3h4DialogueMemoryPolicy
    {
        int ttl_ticks;
        int delta_min;
        int delta_max;
    } BananaK3h4DialogueMemoryPolicy;

    typedef struct BananaK3h4DialogueMemoryEntry
    {
        uint32_t npc_id;
        BananaK3h4DialogueMemoryFeature feature;
        int delta;
        uint64_t updated_tick;
        uint64_t expires_tick;
        uint64_t sequence;
        int occupied;
    } BananaK3h4DialogueMemoryEntry;

    typedef struct BananaK3h4DialogueMemoryStore
    {
        BananaK3h4DialogueMemoryPolicy policy;
        BananaK3h4DialogueMemoryEntry entries[BANANA_K3H4_DIALOGUE_MEMORY_NPC_CAP * BANANA_K3H4_DIALOGUE_MEMORY_FEATURE_CAP];
        uint64_t next_sequence;
    } BananaK3h4DialogueMemoryStore;

    void banana_native_k3h4_dialogue_memory_init(BananaK3h4DialogueMemoryStore *store,
                                                  BananaK3h4DialogueMemoryPolicy policy);

    void banana_native_k3h4_dialogue_memory_prune_expired(BananaK3h4DialogueMemoryStore *store,
                                                           uint64_t current_tick);

    int banana_native_k3h4_dialogue_memory_upsert_delta(BananaK3h4DialogueMemoryStore *store,
                                                         uint32_t npc_id,
                                                         BananaK3h4DialogueMemoryFeature feature,
                                                         int delta,
                                                         uint64_t current_tick);

    int banana_native_k3h4_dialogue_memory_get_delta(const BananaK3h4DialogueMemoryStore *store,
                                                      uint32_t npc_id,
                                                      BananaK3h4DialogueMemoryFeature feature,
                                                      uint64_t current_tick,
                                                      int *out_delta);

    int banana_native_k3h4_dialogue_memory_count_active(const BananaK3h4DialogueMemoryStore *store,
                                                         uint64_t current_tick);

#ifdef __cplusplus
}
#endif

#endif
