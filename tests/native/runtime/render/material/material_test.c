#include "render/material.h"
#include "render/shader.h"

#include "../../support/test_support.h"

static void test_material_solid_sets_default_values(void **state)
{
    (void)state;
    Material material = material_solid(0.1f, 0.2f, 0.3f, 0.4f);

    BANANA_TEST_ASSERT_FLOAT_EQ(material.color[0], 0.1f, 0.0001f,
                                "material_solid must preserve red channel");
    BANANA_TEST_ASSERT_FLOAT_EQ(material.color[1], 0.2f, 0.0001f,
                                "material_solid must preserve green channel");
    BANANA_TEST_ASSERT_FLOAT_EQ(material.color[2], 0.3f, 0.0001f,
                                "material_solid must preserve blue channel");
    BANANA_TEST_ASSERT_FLOAT_EQ(material.color[3], 0.4f, 0.0001f,
                                "material_solid must preserve alpha channel");
    BANANA_TEST_ASSERT_FLOAT_EQ(material.roughness, 0.5f, 0.0001f,
                                "material_solid must initialize roughness default");
    BANANA_TEST_ASSERT_FLOAT_EQ(material.metallic, 0.0f, 0.0001f,
                                "material_solid must initialize metallic default");
    BANANA_TEST_ASSERT_FLOAT_EQ(material.use_texture, 0.0f, 0.0001f,
                                "material_solid must disable textures by default");
    BANANA_TEST_ASSERT_FLOAT_EQ(material.uv_scale, 1.0f, 0.0001f,
                                "material_solid must initialize uv scale default");
}

static void test_material_apply_runs_with_shader_instance(void **state)
{
    (void)state;
    Material material = material_solid(0.7f, 0.6f, 0.5f, 1.0f);
    Shader *shader = shader_create(NULL, NULL);

    BANANA_TEST_ASSERT_TRUE(shader != NULL,
                            "shader_create must return an instance for material_apply test path");

    material_apply(&material, shader);
    shader_destroy(shader);

    BANANA_TEST_ASSERT_TRUE(1,
                            "material_apply must execute without crashing when shader instance is provided");
}

BANANA_TEST_MAIN(
    BANANA_TEST_CASE(test_material_solid_sets_default_values),
    BANANA_TEST_CASE(test_material_apply_runs_with_shader_instance)
)
