#include "collider.h"
#include <math.h>
#include <string.h>

static float fabsf_local(float x)
{
    return x < 0 ? -x : x;
}

int collider_box_vs_box(const PhysicsBody *a, const PhysicsBody *b, Collision *out)
{
    float ax = a->shape.box.half_extents[0];
    float ay = a->shape.box.half_extents[1];
    float az = a->shape.box.half_extents[2];
    float bx = b->shape.box.half_extents[0];
    float by = b->shape.box.half_extents[1];
    float bz = b->shape.box.half_extents[2];

    float dx = b->position[0] - a->position[0];
    float dy = b->position[1] - a->position[1];
    float dz = b->position[2] - a->position[2];

    float ox = (ax + bx) - fabsf_local(dx);
    float oy = (ay + by) - fabsf_local(dy);
    float oz = (az + bz) - fabsf_local(dz);

    if (ox <= 0 || oy <= 0 || oz <= 0)
        return 0;

    out->body_a = a->id;
    out->body_b = b->id;
    out->contact_point[0] = (a->position[0] + b->position[0]) * 0.5f;
    out->contact_point[1] = (a->position[1] + b->position[1]) * 0.5f;
    out->contact_point[2] = (a->position[2] + b->position[2]) * 0.5f;

    /* Minimum penetration axis */
    if (ox < oy && ox < oz)
    {
        out->penetration = ox;
        out->normal[0] = dx < 0 ? -1.f : 1.f;
        out->normal[1] = 0;
        out->normal[2] = 0;
    }
    else if (oy < oz)
    {
        out->penetration = oy;
        out->normal[0] = 0;
        out->normal[1] = dy < 0 ? -1.f : 1.f;
        out->normal[2] = 0;
    }
    else
    {
        out->penetration = oz;
        out->normal[0] = 0;
        out->normal[1] = 0;
        out->normal[2] = dz < 0 ? -1.f : 1.f;
    }
    return 1;
}

int collider_sphere_vs_sphere(const PhysicsBody *a, const PhysicsBody *b, Collision *out)
{
    float dx = b->position[0] - a->position[0];
    float dy = b->position[1] - a->position[1];
    float dz = b->position[2] - a->position[2];
    float dist2 = dx * dx + dy * dy + dz * dz;
    float radsum = a->shape.sphere.radius + b->shape.sphere.radius;
    if (dist2 >= radsum * radsum)
        return 0;

    float dist = sqrtf(dist2);
    out->body_a = a->id;
    out->body_b = b->id;
    out->penetration = radsum - dist;
    if (dist > 1e-6f)
    {
        out->normal[0] = dx / dist;
        out->normal[1] = dy / dist;
        out->normal[2] = dz / dist;
    }
    else
    {
        out->normal[0] = 0;
        out->normal[1] = 1;
        out->normal[2] = 0;
    }
    out->contact_point[0] = a->position[0] + out->normal[0] * a->shape.sphere.radius;
    out->contact_point[1] = a->position[1] + out->normal[1] * a->shape.sphere.radius;
    out->contact_point[2] = a->position[2] + out->normal[2] * a->shape.sphere.radius;
    return 1;
}

int collider_box_vs_sphere(const PhysicsBody *a, const PhysicsBody *b, Collision *out)
{
    /* Clamp sphere centre to box AABB, then test distance vs radius. */
    float cx = b->position[0], cy = b->position[1], cz = b->position[2];
    float clx = a->position[0] - a->shape.box.half_extents[0];
    float chx = a->position[0] + a->shape.box.half_extents[0];
    float cly = a->position[1] - a->shape.box.half_extents[1];
    float chy = a->position[1] + a->shape.box.half_extents[1];
    float clz = a->position[2] - a->shape.box.half_extents[2];
    float chz = a->position[2] + a->shape.box.half_extents[2];

    float closest_x = cx < clx ? clx : (cx > chx ? chx : cx);
    float closest_y = cy < cly ? cly : (cy > chy ? chy : cy);
    float closest_z = cz < clz ? clz : (cz > chz ? chz : cz);

    float dx = cx - closest_x, dy = cy - closest_y, dz = cz - closest_z;
    float dist2 = dx * dx + dy * dy + dz * dz;
    float r = b->shape.sphere.radius;
    if (dist2 >= r * r)
        return 0;

    float dist = sqrtf(dist2);
    out->body_a = a->id;
    out->body_b = b->id;
    out->penetration = r - dist;
    if (dist > 1e-6f)
    {
        out->normal[0] = dx / dist;
        out->normal[1] = dy / dist;
        out->normal[2] = dz / dist;
    }
    else
    {
        out->normal[0] = 0;
        out->normal[1] = 1;
        out->normal[2] = 0;
    }
    out->contact_point[0] = closest_x;
    out->contact_point[1] = closest_y;
    out->contact_point[2] = closest_z;
    return 1;
}

CollisionList collider_broad_phase(PhysicsBody **bodies, int count)
{
    CollisionList list;
    list.count = 0;
    for (int i = 0; i < count && list.count < BANANA_MAX_COLLISIONS; i++)
    {
        for (int j = i + 1; j < count && list.count < BANANA_MAX_COLLISIONS; j++)
        {
            Collision c;
            int hit = 0;
            PhysicsBody *a = bodies[i], *b = bodies[j];
            if (a->shape_type == SHAPE_BOX && b->shape_type == SHAPE_BOX)
                hit = collider_box_vs_box(a, b, &c);
            else if (a->shape_type == SHAPE_SPHERE && b->shape_type == SHAPE_SPHERE)
                hit = collider_sphere_vs_sphere(a, b, &c);
            else
                hit = collider_box_vs_sphere(a, b, &c);
            if (hit)
                list.items[list.count++] = c;
        }
    }
    return list;
}

void collider_resolve(const Collision *c, PhysicsBody *a, PhysicsBody *b)
{
    if (a->is_static && b->is_static)
        return;

    float inv_mass_a = a->is_static ? 0.f : 1.f / a->mass;
    float inv_mass_b = b->is_static ? 0.f : 1.f / b->mass;
    float inv_sum = inv_mass_a + inv_mass_b;
    if (inv_sum == 0.f)
        return;

    /* Relative velocity along normal */
    float rel_vx = b->velocity[0] - a->velocity[0];
    float rel_vy = b->velocity[1] - a->velocity[1];
    float rel_vz = b->velocity[2] - a->velocity[2];
    float vn = rel_vx * c->normal[0] + rel_vy * c->normal[1] + rel_vz * c->normal[2];
    if (vn > 0)
        return; /* separating */

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

    /* Positional correction to prevent sinking */
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
