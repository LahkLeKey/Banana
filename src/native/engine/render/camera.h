#ifndef BANANA_ENGINE_CAMERA_H
#define BANANA_ENGINE_CAMERA_H

#ifdef __cplusplus
extern "C" {
#endif

typedef struct Camera {
    float position[3];
    float target[3];
    float up[3];
    float fov_degrees;
    float aspect;
    float near_plane;
    float far_plane;
} Camera;

Camera camera_create(float fov_degrees, float aspect, float near_plane, float far_plane);

/* Set camera position + look-at target. */
void camera_look_at(Camera *c, float px, float py, float pz,
                                float tx, float ty, float tz);

/* Write view matrix (column-major float[16]). */
void camera_get_view(const Camera *c, float *m16_out);

/* Write projection matrix (column-major float[16]). */
void camera_get_projection(const Camera *c, float *m16_out);

/* Multiply view × projection into out (column-major float[16]). */
void camera_get_view_projection(const Camera *c, float *m16_out);

#ifdef __cplusplus
}
#endif

#endif /* BANANA_ENGINE_CAMERA_H */
