#ifndef BANANA_ENGINE_RENDER_BACKEND_DX12_PROJECTION_POLICY_H
#define BANANA_ENGINE_RENDER_BACKEND_DX12_PROJECTION_POLICY_H

#ifdef __cplusplus
extern "C"
{
#endif

    typedef struct BananaDx12ProjectionCamera
    {
        float eye[3];
        float target[3];
        float up[3];
        float fov_degrees;
        float aspect;
        int valid;
    } BananaDx12ProjectionCamera;

    void banana_dx12_projection_world_to_clip(const BananaDx12ProjectionCamera *camera,
                                              float wx,
                                              float wy,
                                              float wz,
                                              float *out_x,
                                              float *out_y,
                                              float *out_z,
                                              float *out_view_depth);

#ifdef __cplusplus
}
#endif

#endif
