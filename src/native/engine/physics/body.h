#ifndef BANANA_ENGINE_BODY_H
#define BANANA_ENGINE_BODY_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

typedef uint32_t PhysicsBodyId;

typedef enum {
    SHAPE_BOX,
    SHAPE_SPHERE,
    SHAPE_CAPSULE
} ShapeType;

typedef struct {
    float half_extents[3]; /* width/2, height/2, depth/2 */
} BoxShape;

typedef struct {
    float radius;
} SphereShape;

typedef struct {
    float radius;
    float half_height;
} CapsuleShape;

typedef struct PhysicsBody {
    PhysicsBodyId id;
    float position[3];
    float velocity[3];
    float force_accum[3]; /* reset each step */
    float mass;           /* 0.0 = static (infinite mass) */
    float restitution;    /* bounciness 0.0–1.0 */
    float friction;
    ShapeType shape_type;
    union {
        BoxShape    box;
        SphereShape sphere;
        CapsuleShape capsule;
    } shape;
    int is_static;        /* 1 = immovable */
} PhysicsBody;

PhysicsBody *physics_body_create(PhysicsBodyId id, float mass,
                                  float x, float y, float z,
                                  ShapeType shape_type);
void physics_body_apply_force(PhysicsBody *b, float fx, float fy, float fz);
void physics_body_apply_impulse(PhysicsBody *b, float ix, float iy, float iz);
void physics_body_set_static(PhysicsBody *b, int is_static);
void physics_body_destroy(PhysicsBody *b);

#ifdef __cplusplus
}
#endif

#endif /* BANANA_ENGINE_BODY_H */
