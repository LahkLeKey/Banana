#ifndef BANANA_ENGINE_RUNTIME_CAMERA_BASIS_H
#define BANANA_ENGINE_RUNTIME_CAMERA_BASIS_H

#ifdef __cplusplus
extern "C"
{
#endif

    int runtime_camera_compute_ground_basis(const float *camera_eye,
                                            const float *camera_target,
                                            int camera_valid,
                                            float *forward,
                                            float *right);

#ifdef __cplusplus
}
#endif

#endif
