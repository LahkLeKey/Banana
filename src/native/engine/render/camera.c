#ifndef _USE_MATH_DEFINES
#define _USE_MATH_DEFINES
#endif
#include "camera.h"
#include <math.h>
#include <string.h>

/* M_PI fallback for platforms that don't define it */
#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

Camera camera_create(float fov, float aspect, float near_plane, float far_plane) {
    Camera c;
    c.position[0] = 0; c.position[1] = 5; c.position[2] = 10;
    c.target[0]   = 0; c.target[1]   = 0; c.target[2]   = 0;
    c.up[0]       = 0; c.up[1]       = 1; c.up[2]       = 0;
    c.fov_degrees = fov;
    c.aspect      = aspect;
    c.near_plane  = near_plane;
    c.far_plane   = far_plane;
    return c;
}

void camera_look_at(Camera *c,
                     float px, float py, float pz,
                     float tx, float ty, float tz) {
    c->position[0] = px; c->position[1] = py; c->position[2] = pz;
    c->target[0]   = tx; c->target[1]   = ty; c->target[2]   = tz;
}

/* Helper: normalise a vec3 in place. */
static void norm3(float *v) {
    float len = sqrtf(v[0]*v[0] + v[1]*v[1] + v[2]*v[2]);
    if (len > 1e-6f) { v[0]/=len; v[1]/=len; v[2]/=len; }
}

static void cross3(const float *a, const float *b, float *out) {
    out[0] = a[1]*b[2] - a[2]*b[1];
    out[1] = a[2]*b[0] - a[0]*b[2];
    out[2] = a[0]*b[1] - a[1]*b[0];
}

static float dot3(const float *a, const float *b) {
    return a[0]*b[0] + a[1]*b[1] + a[2]*b[2];
}

void camera_get_view(const Camera *c, float *m16) {
    float f[3] = { c->target[0]-c->position[0],
                   c->target[1]-c->position[1],
                   c->target[2]-c->position[2] };
    norm3(f);
    float up[3] = { c->up[0], c->up[1], c->up[2] };
    float s[3]; cross3(f, up, s); norm3(s);
    float u[3]; cross3(s, f, u);

    memset(m16, 0, 16 * sizeof(float));
    m16[0]=s[0]; m16[4]=s[1]; m16[8] =s[2]; m16[12]=-dot3(s, c->position);
    m16[1]=u[0]; m16[5]=u[1]; m16[9] =u[2]; m16[13]=-dot3(u, c->position);
    m16[2]=-f[0];m16[6]=-f[1];m16[10]=-f[2];m16[14]= dot3(f, c->position);
    m16[15]=1.f;
}

void camera_get_projection(const Camera *c, float *m16) {
    float rad = c->fov_degrees * ((float)M_PI / 180.f);
    float t = 1.f / tanf(rad * 0.5f);
    float fn = c->far_plane - c->near_plane;

    memset(m16, 0, 16 * sizeof(float));
    m16[0]  = t / c->aspect;
    m16[5]  = t;
    m16[10] = -(c->far_plane + c->near_plane) / fn;
    m16[11] = -1.f;
    m16[14] = -(2.f * c->far_plane * c->near_plane) / fn;
}

void camera_get_view_projection(const Camera *c, float *m16) {
    float view[16], proj[16];
    camera_get_view(c, view);
    camera_get_projection(c, proj);
    /* proj * view (column-major) */
    for (int row = 0; row < 4; row++) {
        for (int col = 0; col < 4; col++) {
            float sum = 0;
            for (int k = 0; k < 4; k++)
                sum += proj[k*4+row] * view[col*4+k];
            m16[col*4+row] = sum;
        }
    }
}
