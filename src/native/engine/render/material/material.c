#include "material.h"
#include "shader.h"

Material material_solid(float r, float g, float b, float a)
{
    Material m;
    m.color[0] = r;
    m.color[1] = g;
    m.color[2] = b;
    m.color[3] = a;
    m.roughness = 0.5f;
    m.metallic = 0.0f;
    m.use_texture = 0.0f;
    m.uv_scale = 1.0f;
    return m;
}

void material_apply(const Material *m, struct Shader *s)
{
    shader_set_vec3(s, "u_color", m->color[0], m->color[1], m->color[2]);
    shader_set_float(s, "u_roughness", m->roughness);
    shader_set_float(s, "u_metallic", m->metallic);
    shader_set_float(s, "u_use_texture", m->use_texture);
    shader_set_float(s, "u_uv_scale", m->uv_scale);
}
