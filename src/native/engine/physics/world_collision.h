#ifndef BANANA_ENGINE_PHYSICS_WORLD_COLLISION_H
#define BANANA_ENGINE_PHYSICS_WORLD_COLLISION_H

#include "collider.h"
#include "world.h"

typedef void (*PhysicsWorldCollisionResolveHook)(const Collision *c, PhysicsBody *a, PhysicsBody *b);

void physics_world_set_collision_resolve_hook(PhysicsWorldCollisionResolveHook hook);
void physics_world_reset_collision_resolve_hook(void);
void physics_world_resolve_collision_pairs(PhysicsWorld *w, const CollisionList *cols);

#endif
