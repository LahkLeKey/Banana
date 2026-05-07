#include "body.h"
#include <stdlib.h>
#include <string.h>

PhysicsBody *physics_body_create(PhysicsBodyId id, float mass,
                                   float x, float y, float z,
                                   ShapeType shape_type) {
    PhysicsBody *b = calloc(1, sizeof(PhysicsBody));
    b->id          = id;
    b->mass        = mass;
    b->position[0] = x; b->position[1] = y; b->position[2] = z;
    b->restitution = 0.3f;
    b->friction    = 0.5f;
    b->shape_type  = shape_type;
    b->is_static   = (mass == 0.f) ? 1 : 0;

    /* Default shape dimensions */
    if (shape_type == SHAPE_BOX) {
        b->shape.box.half_extents[0] = 0.5f;
        b->shape.box.half_extents[1] = 0.5f;
        b->shape.box.half_extents[2] = 0.5f;
    } else if (shape_type == SHAPE_SPHERE) {
        b->shape.sphere.radius = 0.5f;
    } else if (shape_type == SHAPE_CAPSULE) {
        b->shape.capsule.radius      = 0.3f;
        b->shape.capsule.half_height = 0.9f;
    }
    return b;
}

void physics_body_apply_force(PhysicsBody *b, float fx, float fy, float fz) {
    if (b->is_static) return;
    b->force_accum[0] += fx;
    b->force_accum[1] += fy;
    b->force_accum[2] += fz;
}

void physics_body_apply_impulse(PhysicsBody *b, float ix, float iy, float iz) {
    if (b->is_static || b->mass <= 0.f) return;
    float inv_mass = 1.f / b->mass;
    b->velocity[0] += ix * inv_mass;
    b->velocity[1] += iy * inv_mass;
    b->velocity[2] += iz * inv_mass;
}

void physics_body_set_static(PhysicsBody *b, int is_static) {
    b->is_static = is_static;
    if (is_static) { b->mass = 0.f; }
}

void physics_body_destroy(PhysicsBody *b) { free(b); }
