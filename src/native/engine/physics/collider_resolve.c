#include "collider.h"

void collider_resolve(const Collision *c, PhysicsBody *a, PhysicsBody *b)
{
    if (a->is_static && b->is_static)
        return;

    float inv_mass_a = a->is_static ? 0.f : 1.f / a->mass;
    float inv_mass_b = b->is_static ? 0.f : 1.f / b->mass;
    float inv_sum = inv_mass_a + inv_mass_b;
    if (inv_sum == 0.f)
        return;

    float rel_vx = b->velocity[0] - a->velocity[0];
    float rel_vy = b->velocity[1] - a->velocity[1];
    float rel_vz = b->velocity[2] - a->velocity[2];
    float vn = rel_vx * c->normal[0] + rel_vy * c->normal[1] + rel_vz * c->normal[2];
    if (vn > 0)
        return;

    float e = (a->restitution + b->restitution) * 0.5f;
    float j = -(1.f + e) * vn / inv_sum;

    float jx = j * c->normal[0];
    float jy = j * c->normal[1];
    float jz = j * c->normal[2];

    if (!a->is_static)
    {
        a->velocity[0] -= jx * inv_mass_a;
        a->velocity[1] -= jy * inv_mass_a;
        a->velocity[2] -= jz * inv_mass_a;
    }
    if (!b->is_static)
    {
        b->velocity[0] += jx * inv_mass_b;
        b->velocity[1] += jy * inv_mass_b;
        b->velocity[2] += jz * inv_mass_b;
    }

    float correction = (c->penetration / inv_sum) * 0.8f;
    if (!a->is_static)
    {
        a->position[0] -= c->normal[0] * correction * inv_mass_a;
        a->position[1] -= c->normal[1] * correction * inv_mass_a;
        a->position[2] -= c->normal[2] * correction * inv_mass_a;
    }
    if (!b->is_static)
    {
        b->position[0] += c->normal[0] * correction * inv_mass_b;
        b->position[1] += c->normal[1] * correction * inv_mass_b;
        b->position[2] += c->normal[2] * correction * inv_mass_b;
    }
}
