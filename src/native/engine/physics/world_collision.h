#ifndef BANANA_ENGINE_PHYSICS_WORLD_COLLISION_H
#define BANANA_ENGINE_PHYSICS_WORLD_COLLISION_H

#include "collider.h"
#include "world.h"

void physics_world_resolve_collision_pairs(PhysicsWorld *w, const CollisionList *cols);

#endif
