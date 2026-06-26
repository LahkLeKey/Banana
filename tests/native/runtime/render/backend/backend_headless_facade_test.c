#include "render/backend.h"
#include "runtime/support/test_support.h"

#include <string.h>

static void test_backend_headless_facade_contract(void **state)
{
    BananaRenderBackend requested = BANANA_RENDER_BACKEND_UNKNOWN;
    BananaRenderBackend active = BANANA_RENDER_BACKEND_UNKNOWN;
    const char *headless_name = NULL;
    const char *status = NULL;

    (void)state;

    requested = banana_render_backend_requested();
    active = banana_render_backend_active();
    headless_name = banana_render_backend_name(BANANA_RENDER_BACKEND_HEADLESS);
    status = banana_render_backend_status();

    BANANA_TEST_ASSERT_INT_EQ((int)requested,
                              (int)BANANA_RENDER_BACKEND_HEADLESS,
                              "headless backend facade must request the headless backend without platform defines");
    BANANA_TEST_ASSERT_INT_EQ((int)active,
                              (int)BANANA_RENDER_BACKEND_HEADLESS,
                              "headless backend facade must report the headless backend as active without platform defines");
    BANANA_TEST_ASSERT_TRUE(headless_name != NULL && strcmp(headless_name, "headless") == 0,
                            "backend facade must expose the headless backend name");
    BANANA_TEST_ASSERT_TRUE(status != NULL && strcmp(status, "headless-stub") == 0,
                            "backend facade must report the headless stub status without platform backends");
    BANANA_TEST_ASSERT_TRUE(strcmp(banana_render_backend_name(BANANA_RENDER_BACKEND_UNKNOWN), "unknown") == 0,
                            "backend facade must keep the unknown backend label stable");
}

BANANA_TEST_MAIN(
    BANANA_TEST_CASE(test_backend_headless_facade_contract)
)