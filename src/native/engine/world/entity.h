#ifndef BANANA_ENGINE_ENTITY_H
#define BANANA_ENGINE_ENTITY_H

#ifdef __cplusplus
extern "C"
{
#endif

#include <stdint.h>

    typedef uint32_t EntityId;

    typedef enum
    {
        ENTITY_TYPE_STATIC,  /* immovable world object */
        ENTITY_TYPE_DYNAMIC, /* physics-driven object */
        ENTITY_TYPE_NPC,     /* has a controller instance */
        ENTITY_TYPE_PLAYER,  /* reserved */
        ENTITY_TYPE_TRIGGER  /* invisible trigger volume */
    } EntityType;

    typedef struct Entity
    {
        EntityId id;
        EntityType type;
        char player_guid[40];
        char controller_kind[16];
        char gameplay_model_id[96];
        float position[3];
        float rotation[4]; /* quaternion x,y,z,w */
        float scale[3];
        uint32_t physics_body_id; /* 0 = no physics body */
        uint32_t controller_id;   /* 0 = no AI controller */
        int active;
    } Entity;

    Entity *entity_create(EntityId id, EntityType type, float x, float y, float z);
    void entity_destroy(Entity *e);

#ifdef __cplusplus
}
#endif

#endif /* BANANA_ENGINE_ENTITY_H */
