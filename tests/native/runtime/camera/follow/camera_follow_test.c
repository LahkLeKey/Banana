#include "runtime/camera/follow/camera_follow.h"
#include "runtime/camera/follow/camera_follow_policy.h"

#include "../../support/test_support.h"

#include <string.h>

static int g_policy_compute_calls = 0;
static int g_policy_compute_result = 1;
static int g_set_camera_calls = 0;
static Camera g_last_camera = {0};

Entity *world_get_entity(World *world, EntityId id)
{
    int i = 0;

    if (!world || id == 0)
        return NULL;

    for (i = 0; i < world->entity_count; i++)
    {
        Entity *entity = world->entities[i];
        if (entity && entity->id == id)
            return entity;
    }

    return NULL;
}

int runtime_camera_follow_policy_compute(const float *player_position,
                                         int viewport_width,
                                         int viewport_height,
                                         RuntimeCameraFollowPose *out_pose)
{
    (void)player_position;
    (void)viewport_width;
    (void)viewport_height;

    g_policy_compute_calls += 1;
    if (!out_pose || !g_policy_compute_result)
        return 0;

    memset(out_pose, 0, sizeof(*out_pose));
    out_pose->eye[0] = 4.0f;
    out_pose->eye[1] = 5.0f;
    out_pose->eye[2] = 6.0f;
    out_pose->target[0] = 1.0f;
    out_pose->target[1] = 2.0f;
    out_pose->target[2] = 3.0f;
    out_pose->fov_degrees = 70.0f;
    out_pose->near_plane = 0.1f;
    out_pose->far_plane = 120.0f;
    out_pose->aspect = 16.0f / 9.0f;
    return 1;
}

Camera camera_create(float fov_degrees, float aspect, float near_plane, float far_plane)
{
    Camera camera = {0};

    camera.fov_degrees = fov_degrees;
    camera.aspect = aspect;
    camera.near_plane = near_plane;
    camera.far_plane = far_plane;
    return camera;
}

void camera_look_at(Camera *camera, float px, float py, float pz, float tx, float ty, float tz)
{
    if (!camera)
        return;

    camera->position[0] = px;
    camera->position[1] = py;
    camera->position[2] = pz;
    camera->target[0] = tx;
    camera->target[1] = ty;
    camera->target[2] = tz;
}

void renderer_set_camera(Renderer *renderer, const Camera *camera)
{
    (void)renderer;

    g_set_camera_calls += 1;
    if (camera)
        g_last_camera = *camera;
}

static void reset_camera_follow_state(void)
{
    g_policy_compute_calls = 0;
    g_policy_compute_result = 1;
    g_set_camera_calls = 0;
    memset(&g_last_camera, 0, sizeof(g_last_camera));
}

static void test_camera_follow_guards(void **state)
{
    (void)state;
    Renderer *renderer = (Renderer *)(void *)0x1;
    World world = {0};
    float camera_eye[3] = {0};
    float camera_target[3] = {0};
    int camera_valid = 0;

    reset_camera_follow_state();

    runtime_camera_follow_player(NULL, &world, 1, 1280, 720, camera_eye, camera_target, &camera_valid);
    runtime_camera_follow_player(renderer, NULL, 1, 1280, 720, camera_eye, camera_target, &camera_valid);
    runtime_camera_follow_player(renderer, &world, 0, 1280, 720, camera_eye, camera_target, &camera_valid);
    runtime_camera_follow_player(renderer, &world, 1, 0, 720, camera_eye, camera_target, &camera_valid);
    runtime_camera_follow_player(renderer, &world, 1, 1280, 0, camera_eye, camera_target, &camera_valid);
    runtime_camera_follow_player(renderer, &world, 1, 1280, 720, NULL, camera_target, &camera_valid);
    runtime_camera_follow_player(renderer, &world, 1, 1280, 720, camera_eye, NULL, &camera_valid);
    runtime_camera_follow_player(renderer, &world, 1, 1280, 720, camera_eye, camera_target, NULL);

    BANANA_TEST_ASSERT_INT_EQ(g_policy_compute_calls,
                              0,
                              "guard failures must skip camera follow policy evaluation");
    BANANA_TEST_ASSERT_INT_EQ(g_set_camera_calls,
                              0,
                              "guard failures must never submit a camera to renderer");
}

static void test_camera_follow_player_and_policy_paths(void **state)
{
    (void)state;
    Renderer *renderer = (Renderer *)(void *)0x1;
    World world = {0};
    Entity player = { .id = 11, .active = 1 };
    float camera_eye[3] = {0};
    float camera_target[3] = {0};
    int camera_valid = 0;

    reset_camera_follow_state();

    runtime_camera_follow_player(renderer, &world, 11, 1280, 720, camera_eye, camera_target, &camera_valid);
    BANANA_TEST_ASSERT_INT_EQ(g_policy_compute_calls,
                              0,
                              "missing players must skip policy evaluation");

    world.entities[0] = &player;
    world.entity_count = 1;
    player.active = 0;

    runtime_camera_follow_player(renderer, &world, 11, 1280, 720, camera_eye, camera_target, &camera_valid);
    BANANA_TEST_ASSERT_INT_EQ(g_policy_compute_calls,
                              0,
                              "inactive players must skip policy evaluation");

    player.active = 1;
    g_policy_compute_result = 0;
    runtime_camera_follow_player(renderer, &world, 11, 1280, 720, camera_eye, camera_target, &camera_valid);

    BANANA_TEST_ASSERT_INT_EQ(g_policy_compute_calls,
                              1,
                              "active players should evaluate camera follow policy once");
    BANANA_TEST_ASSERT_INT_EQ(g_set_camera_calls,
                              0,
                              "policy failures must skip renderer camera submission");

    g_policy_compute_result = 1;
    runtime_camera_follow_player(renderer, &world, 11, 1280, 720, camera_eye, camera_target, &camera_valid);

    BANANA_TEST_ASSERT_INT_EQ(g_policy_compute_calls,
                              2,
                              "successful policy path should increment policy calls");
    BANANA_TEST_ASSERT_INT_EQ(g_set_camera_calls,
                              1,
                              "successful policy path must submit camera to renderer");
    BANANA_TEST_ASSERT_INT_EQ(camera_valid,
                              1,
                              "successful policy path must mark camera as valid");
    BANANA_TEST_ASSERT_FLOAT_EQ(camera_eye[0], 4.0f, 0.0001f,
                                "camera eye x should match follow policy output");
    BANANA_TEST_ASSERT_FLOAT_EQ(camera_target[2], 3.0f, 0.0001f,
                                "camera target z should match follow policy output");
    BANANA_TEST_ASSERT_FLOAT_EQ(g_last_camera.position[1], 5.0f, 0.0001f,
                                "camera look-at should project eye coordinates into render camera");
}

BANANA_TEST_MAIN(
    BANANA_TEST_CASE(test_camera_follow_guards),
    BANANA_TEST_CASE(test_camera_follow_player_and_policy_paths)
)
