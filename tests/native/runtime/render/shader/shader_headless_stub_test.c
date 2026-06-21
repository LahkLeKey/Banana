#include "render/shader.h"
#include "../../support/test_support.h"

static void test_shader_headless_stub_contract(void **state)
{
    Shader *shader = NULL;
    float matrix[16] = {
        1.0f, 0.0f, 0.0f, 0.0f,
        0.0f, 1.0f, 0.0f, 0.0f,
        0.0f, 0.0f, 1.0f, 0.0f,
        0.0f, 0.0f, 0.0f, 1.0f,
    };

    (void)state;

    shader = shader_create("void main(){}", "void main(){}");
    BANANA_TEST_ASSERT_TRUE(shader != NULL,
                            "headless shader stub must allocate a shader handle");

    shader_bind(shader);
    shader_set_float(shader, "u_scalar", 1.25f);
    shader_set_int(shader, "u_index", 7);
    shader_set_vec3(shader, "u_vec", 1.0f, 2.0f, 3.0f);
    shader_set_mat4(shader, "u_mat", matrix);

    shader_destroy(shader);
}

BANANA_TEST_MAIN(
    BANANA_TEST_CASE(test_shader_headless_stub_contract)
)