#include "shader.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifdef BANANA_ENGINE_HAS_GLFW
#include <GL/gl.h>

struct Shader { unsigned int program; };

static unsigned int compile_stage(GLenum type, const char *src) {
    unsigned int id = glCreateShader(type);
    glShaderSource(id, 1, &src, NULL);
    glCompileShader(id);
    int ok; glGetShaderiv(id, GL_COMPILE_STATUS, &ok);
    if (!ok) {
        char log[512]; glGetShaderInfoLog(id, 512, NULL, log);
        fprintf(stderr, "[engine/shader] compile error: %s\n", log);
        glDeleteShader(id);
        return 0;
    }
    return id;
}

Shader *shader_create(const char *vert_src, const char *frag_src) {
    unsigned int vs = compile_stage(GL_VERTEX_SHADER,   vert_src);
    unsigned int fs = compile_stage(GL_FRAGMENT_SHADER, frag_src);
    if (!vs || !fs) return NULL;

    unsigned int prog = glCreateProgram();
    glAttachShader(prog, vs); glAttachShader(prog, fs);
    glLinkProgram(prog);
    glDeleteShader(vs); glDeleteShader(fs);

    int ok; glGetProgramiv(prog, GL_LINK_STATUS, &ok);
    if (!ok) {
        char log[512]; glGetProgramInfoLog(prog, 512, NULL, log);
        fprintf(stderr, "[engine/shader] link error: %s\n", log);
        glDeleteProgram(prog);
        return NULL;
    }
    Shader *s = malloc(sizeof(Shader));
    s->program = prog;
    return s;
}

void shader_bind(Shader *s)  { glUseProgram(s->program); }

void shader_set_float(Shader *s, const char *name, float v) {
    glUniform1f(glGetUniformLocation(s->program, name), v);
}
void shader_set_vec3(Shader *s, const char *name, float x, float y, float z) {
    glUniform3f(glGetUniformLocation(s->program, name), x, y, z);
}
void shader_set_mat4(Shader *s, const char *name, const float *m16) {
    glUniformMatrix4fv(glGetUniformLocation(s->program, name), 1, 0, m16);
}

void shader_destroy(Shader *s) { if (s) { glDeleteProgram(s->program); free(s); } }

#else  /* stubs */

struct Shader { int dummy; };
Shader *shader_create(const char *v, const char *f) { (void)v; (void)f; return malloc(sizeof(Shader)); }
void shader_bind(Shader *s) { (void)s; }
void shader_set_float(Shader *s, const char *n, float v) { (void)s; (void)n; (void)v; }
void shader_set_vec3(Shader *s, const char *n, float x, float y, float z) { (void)s; (void)n; (void)x; (void)y; (void)z; }
void shader_set_mat4(Shader *s, const char *n, const float *m) { (void)s; (void)n; (void)m; }
void shader_destroy(Shader *s) { free(s); }

#endif
